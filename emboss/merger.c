/* @source merger application
**
** Merge two sequences after a global alignment
** @author: Copyright (C) Gary Williams (gwilliam@hgmp.mrc.ac.uk)
** @@
**
** Closely based on work by Alan Bleasby
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
******************************************************************************/

#include "emboss.h"




static void merger_Merge(AjPAlign align, AjPStr *merged,
			 const char *a, const char *b,
			 const AjPStr m, const AjPStr n,
			 ajint start1,
			 ajint start2, float score, AjBool mark,
			 const AjPSeqCvt cvt,
			 const char *namea, const char *nameb, ajint begina,
			 ajint beginb);

static float merger_quality(const char * seq, ajint pos, ajint window);

static AjBool merger_bestquality(const char * a, const char *b,
				 ajint apos, ajint bpos);




/* @prog merger ***************************************************************
**
** Merge two overlapping nucleic acid sequences
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPAlign align;
    AjPSeq a;
    AjPSeq b;
    AjPSeqout seqout;

    AjPStr m;
    AjPStr n;

    AjPStr merged = NULL;

    ajint lena;
    ajint lenb;

    const char   *p;
    const char   *q;

    ajint start1 = 0;
    ajint start2 = 0;

    float *path;
    ajint *compass;

    AjPMatrixf matrix;
    AjPSeqCvt  cvt = 0;
    float **sub;

    float gapopen;
    float gapextend;
    ajint maxarr = 1000;
    ajint len;				/* arbitrary. realloc'd if needed */

    float score;
    ajint begina;
    ajint beginb;

    embInit("merger", argc, argv);

    a         = ajAcdGetSeq("asequence");
    b         = ajAcdGetSeq("bsequence");
    seqout    = ajAcdGetSeqout("outseq");
    matrix    = ajAcdGetMatrixf("datafile");
    gapopen   = ajAcdGetFloat("gapopen");
    gapextend = ajAcdGetFloat("gapextend");
    align     = ajAcdGetAlign("outfile");

    gapopen = ajRoundF(gapopen, 8);
    gapextend = ajRoundF(gapextend, 8);

    AJCNEW(path, maxarr);
    AJCNEW(compass, maxarr);

    /*
    **  make the two sequences lowercase so we can show which one we are
    **  using in the merge by uppercasing it
    */

    ajSeqToLower(a);
    ajSeqToLower(b);

    m = ajStrNew();
    n = ajStrNew();

    sub = ajMatrixfArray(matrix);
    cvt = ajMatrixfCvt(matrix);

    begina = ajSeqBegin(a);
    beginb = ajSeqBegin(b);

    lena = ajSeqLen(a);
    lenb = ajSeqLen(b);
    len  = lena*lenb;

    if(len>maxarr)
    {

	ajDebug("merger: resize path, len to %d (%d * $d)\n",
		len, lena, lenb);

        AJCRESIZE(path,len);
        AJCRESIZE(compass,len);
        maxarr=len;
    }


    p = ajSeqChar(a);
    q = ajSeqChar(b);

    ajStrAssC(&m,"");
    ajStrAssC(&n,"");

    embAlignPathCalc(p,q,lena,lenb,gapopen,gapextend,path,sub,cvt,
		     compass, ajFalse);

    score = embAlignScoreNWMatrix(path,a,b,sub,cvt,lena,lenb,gapopen,compass,
				  gapextend,&start1,&start2);

    embAlignWalkNWMatrix(path,a,b,&m,&n,lena,lenb, &start1,&start2,gapopen,
			 gapextend,cvt,compass,sub);

    /*
    ** now construct the merged sequence, uppercase the bits of the two
    ** input sequences which are used in the merger
    */
    merger_Merge(align, &merged,p,q,m,n,start1,start2,score,1,cvt,
		 ajSeqName(a),ajSeqName(b),begina,beginb);

    embAlignReportGlobal(align, a, b, m, n,
			 start1, start2, gapopen, gapextend,
			 score, matrix, begina, beginb);

    ajAlignWrite(align);
    ajAlignReset(align);

    /* write the merged sequence */
    ajSeqReplace(a, merged);
    ajSeqWrite(seqout, a);
    ajSeqWriteClose(seqout);

    ajAlignClose(align);
    ajAlignDel(&align);

    AJFREE(compass);
    AJFREE(path);

    ajStrDel(&n);
    ajStrDel(&m);

    ajExit();

    return 0;
}




/* @funcstatic merger_Merge ***************************************************
**
** Print a global alignment
** Nucleotides or proteins as needed.
**
** @param [w] align [AjPAlign] Alignment object
** @param [w] ms [AjPStr *] output merged sequence
** @param [r] a [const char *] complete first sequence
** @param [r] b [const char *] complete second sequence
** @param [r] m [const AjPStr] Walk alignment for first sequence
** @param [r] n [const AjPStr] Walk alignment for second sequence
** @param [r] start1 [ajint] start of alignment in first sequence
** @param [r] start2 [ajint] start of alignment in second sequence
** @param [r] score [float] alignment score from AlignScoreX
** @param [r] mark [AjBool] mark matches and conservatives
** @param [r] cvt [const AjPSeqCvt] conversion table for matrix
** @param [r] namea [const char *] name of first sequence
** @param [r] nameb [const char *] name of second sequence
** @param [r] begina [ajint] first sequence offset
** @param [r] beginb [ajint] second sequence offset
**
** @return [void]
******************************************************************************/

static void merger_Merge(AjPAlign align, AjPStr *ms,
			 const char *a, const char *b,
			 const AjPStr m, const AjPStr n, ajint start1,
			 ajint start2, float score, AjBool mark,
			 const AjPSeqCvt cvt,
			 const char *namea, const char *nameb,
			 ajint begina, ajint beginb)
{
    ajint apos;
    ajint bpos;
    ajint i;
    AjPStr mm = NULL;
    AjPStr nn = NULL;

    char *p;
    char *q;

    ajint olen;				/* length of walk alignment */

    /* lengths of the sequences after the aligned region */
    ajint alen;
    ajint blen;
    static AjPStr tmpstr;

    mm = ajStrNewS(m);
    nn = ajStrNewS(n);

    p    = ajStrStrMod(&mm);
    q    = ajStrStrMod(&nn);
    olen = ajStrLen(mm);

    /* output the left hand side */
    if(start1 > start2)
    {
	for(i=0; i<start1; i++)
	    ajStrAppK(ms, a[i]);

	if(start2)
	{
	    ajFmtPrintAppS(&tmpstr, "WARNING: *************************"
			   "********\n");
	    ajFmtPrintAppS(&tmpstr, "The region of alignment only starts at "
			   "position %d of sequence %s\n", start2+1, nameb);
	    ajFmtPrintAppS(&tmpstr, "Only the sequence of %s is being used "
			   "before this point\n\n", namea);
	    ajAlignSetTailApp(align, tmpstr);
	    ajStrDel(&tmpstr);
	}
    }
    else if(start2 > start1)
    {
	for(i=0; i<start2; i++)
	    ajStrAppK(ms, b[i]);

	if(start1)
	{
	    ajFmtPrintAppS(&tmpstr, "WARNING: **************************"
			   "*******\n");
	    ajFmtPrintAppS(&tmpstr, "The region of alignment only starts at "
			   "position %d of sequence %s\n", start1+1, namea);
	    ajFmtPrintAppS(&tmpstr, "Only the sequence of %s is being used "
			   "before this point\n\n", nameb);
	    ajAlignSetTailApp(align, tmpstr);
	    ajStrDel(&tmpstr);
	}
    }
    else if(start1 && start2)
    {
	/* both the same length and > 1 before the aligned region */
	ajFmtPrintAppS(&tmpstr,
			      "WARNING: *********************************\n");
	ajFmtPrintAppS(&tmpstr, "There is an equal amount of unaligned "
		       "sequence (%d) at the start of the sequences.\n",
		       start1);
	ajFmtPrintAppS(&tmpstr, "Sequence %s is being arbitrarily chosen "
		       "for the merged sequence\n\n", namea);
	ajAlignSetTailApp(align, tmpstr);
	ajStrDel(&tmpstr);

	for(i=0; i<start1; i++)
	    ajStrAppK(ms, a[i]);
    }

    /* header */
    ajFmtPrintS(&tmpstr, "%s position base\t\t%s position base\t"
		"Using\n", namea, nameb);
    ajAlignSetTailApp(align, tmpstr);

    /* make the merged sequence
    **
    **  point to the start of the alignment in the complete unaligned
    **  sequences
    */
    apos = start1;
    bpos = start2;

    for(i=0; i<olen; i++)
    {
	if(p[i]=='.' || p[i]==' ' || q[i]=='.' || q[i]==' ')
	{				/* gap! */
	    if(merger_bestquality(a, b, apos, bpos))
	    {
		p[i] = toupper((ajint)p[i]);
		if(p[i] != '.' && p[i] != ' ')
		    ajStrAppK(ms, p[i]);
		ajFmtPrintS(&tmpstr, "\t%d\t'%c'\t\t%d\t'%c'\t\t'%c'\n",
			    apos+1, p[i],bpos+1, q[i], p[i]);
		ajAlignSetTailApp(align, tmpstr);
	    }
	    else
	    {
		q[i] = toupper((ajint)q[i]);
		if(q[i] != '.' && q[i] != ' ')
		    ajStrAppK(ms, q[i]);
		ajFmtPrintS(&tmpstr, "\t%d\t'%c'\t\t%d\t'%c'\t\t'%c'\n",
			    apos+1, p[i],bpos+1, q[i], q[i]);
		ajAlignSetTailApp(align, tmpstr);
	    }

	}
	else if(p[i]=='n' || p[i]=='N')
	{
	    q[i] = toupper((ajint)q[i]);
	    ajStrAppK(ms, q[i]);
	}
	else if(q[i]=='n' || q[i]=='N')
	{
	    p[i] = toupper((ajint)p[i]);
	    ajStrAppK(ms, p[i]);
	}
	else if(p[i] != q[i])
	{
	    /*
	    **  get the sequence with the best quality and use the base
	    **  from that one
	    */
	    if(merger_bestquality(a, b, apos, bpos))
	    {
		p[i] = toupper((ajint)p[i]);
		ajStrAppK(ms, p[i]);
		ajFmtPrintS(&tmpstr, "\t%d\t'%c'\t\t%d\t'%c'\t\t'%c'\n",
			    apos+1, p[i],bpos+1, q[i], p[i]);
		ajAlignSetTailApp(align, tmpstr);
	    }
	    else
	    {
		q[i] = toupper((ajint)q[i]);
		ajStrAppK(ms, q[i]);
		ajFmtPrintS(&tmpstr, "\t%d\t'%c'\t\t%d\t'%c'\t\t'%c'\n",
			    apos+1, p[i],bpos+1, q[i], q[i]);
		ajAlignSetTailApp(align, tmpstr);
	    }

	}
	else
	    ajStrAppK(ms, p[i]);

	/* update the positions in the unaligned complete sequences */
	if(p[i] != '.' &&  p[i] != ' ') apos++;
	if(q[i] != '.' &&  q[i] != ' ') bpos++;
    }

    /* output the right hand side */
    alen = strlen(&a[apos]);
    blen = strlen(&b[bpos]);

    if(alen > blen)
    {
	ajStrAppC(ms, &a[apos]);
	if(blen)
	{
	    ajFmtPrintAppS(&tmpstr, "WARNING: ***************************"
			   "******\n");
	    ajFmtPrintAppS(&tmpstr, "The region of alignment ends at "
			   "position %d of sequence %s\n",
			   bpos+1, nameb);
	    ajFmtPrintAppS(&tmpstr, "Only the sequence of %s is being used "
			   "after this point\n\n", namea);
	    ajAlignSetTailApp(align, tmpstr);
	    ajStrDel(&tmpstr);
	}

    }

    if(blen > alen)
    {
	ajStrAppC(ms, &b[bpos]);
	if(alen)
	{
	    ajFmtPrintAppS(&tmpstr, "WARNING: ***************************"
			   "******\n");
	    ajFmtPrintAppS(&tmpstr, "The region of alignment ends at "
			   "position %d of sequence %s\n",
			   apos+1, namea);
	    ajFmtPrintAppS(&tmpstr, "Only the sequence of %s is being used "
			   "after this point\n\n", nameb);
	    ajAlignSetTailApp(align, tmpstr);
	    ajStrDel(&tmpstr);
	}
    }
    else if(alen && blen)
    {	/* both the same length and > 1 */
	ajFmtPrintAppS(&tmpstr, "WARNING: ************************"
		       "*********\n");
	ajFmtPrintAppS(&tmpstr, "There is an equal amount of unaligned "
		       "sequence (%d) at the end of the sequences.\n",
		       alen);
	ajFmtPrintAppS(&tmpstr, "Sequence %s is being arbitrarily chosen "
		       "for the merged sequence\n\n", namea);
	ajStrAppC(ms, &a[apos]);
	ajAlignSetTailApp(align, tmpstr);
	ajStrDel(&tmpstr);
    }

    return;
}




/* @funcstatic merger_bestquality *********************************************
**
** Return ajTrue if the first sequence has the best quality
** If both sequences have the same quality, pick the first
**
** @param [r] a [const char*] First sequence
** @param [r] b [const char*] Second sequence
** @param [r] apos [ajint] Position in first sequence
** @param [r] bpos [ajint] Position in second sequence
** @return [AjBool] ajTrue = first sequence is the best quality at this point
**
******************************************************************************/

static AjBool merger_bestquality(const char * a, const char *b,
				 ajint apos, ajint bpos)
{
    float qa;
    float qb;

    qa = merger_quality(a, apos, 5);
    qb = merger_quality(b, bpos, 5);

    if(qa == qb)
    {
	/* both have the same quality, use a larger window */
	qa = merger_quality(a, apos, 10);
	qb = merger_quality(b, bpos, 10);
    }

    if(qa == qb)
    {
	/* both have the same quality, use a larger window */
	qa = merger_quality(a, apos, 20);
	qb = merger_quality(b, bpos, 20);
    }

    if(qa >= qb)
	/*  both have the same quality, use the first sequence */
	return ajTrue;

    return ajFalse;
}




/* @funcstatic merger_quality *************************************************
**
** Calculate the quality of a window of a sequence
**
** quality = sequence value/length under window either side of a position
**
** sequence value = sum of points in that subsequence
**
** good bases = 2 points
**
** ambiguous bases = 1 point
**
** N's = 0 points
**
** off end of the sequence = 0 points
**
** THIS HEAVILY DISCRIMINATES AGAINST THE IFFY BITS AT THE END OF
** SEQUENCE READS
**
** @param [r] seq [const char*] Sequence
** @param [r] pos [ajint] Position
** @param [r] window [ajint] Window size
** @return [float] quality of the window
**
******************************************************************************/

static float merger_quality(const char * seq, ajint pos, ajint window)
{
    ajint value = 0;
    ajint i;

    for(i=pos; i<pos+window && i < strlen(seq); i++)
	if(strchr("aAcCgGtTuU", seq[i]))
	    /* good bases count for two points */
	    value+=2;
	else if(strchr("mrwsykvhdbMRWSYKVHDB", seq[i]))
	    /* ambiguous bases count for only one point */
	    value++;

    for(i=pos-1; i>pos-window && i>=0; i--)
	if(strchr("aAcCgGtTuU", seq[i]))
	    /* good bases count for two points */
	    value+=2;
	else if(strchr("mrwsykvhdbMRWSYKVHDB", seq[i]))
	    /* ambiguous bases count for only one point */
	    value++;

    return (double)value/(double)(window*2+1);
}

