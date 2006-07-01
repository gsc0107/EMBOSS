 /* @source restrict application
 **
 ** Reports restriction enzyme cleavage sites
 ** @author Copyright (C) Alan Bleasby (ableasby@hgmp.mrc.ac.uk)
 ** @@
 **
 ** The author wishes to thank Helge Horch for important
 ** discussions and suggestions in the production of this program
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
 *****************************************************************************/

#include "emboss.h"
#include <string.h>
#include <limits.h>

#define ENZDATA "REBASE/embossre.enz"
#define EQUDATA "embossre.equ"


#define EQUGUESS 3500	  /* Estimate of number of equivalent names */


static void restrict_reportHits(AjPReport report, const AjPSeq seq,
				AjPFeattable TabRpt, AjPList l,
				ajint hits, ajint begin, ajint end,
				AjBool ambiguity, ajint mincut, ajint maxcut,
				AjBool plasmid, AjBool blunt, AjBool sticky,
				ajint sitelen, AjBool limit,
				const AjPTable table,
				AjBool alpha, AjBool frags,
				AjBool nameit, AjBool ifrag);
static void restrict_printHits(AjPFile outf, AjPList l, const AjPStr name,
			       ajint hits, ajint begin, ajint end,
			       AjBool ambiguity, ajint mincut, ajint maxcut,
			       AjBool plasmid, AjBool blunt, AjBool sticky,
			       ajint sitelen, AjBool limit,
			       const AjPTable table,
			       AjBool alpha, AjBool frags,
			       AjBool nameit);
static void restrict_read_equiv(AjPFile equfile, AjPTable table);
static void restrict_read_file_of_enzyme_names(AjPStr *enzymes);
static ajint restrict_enzcompare(const void *a, const void *b);




/* @prog restrict *************************************************************
**
** Finds restriction enzyme cleavage sites
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPSeqall seqall;
    AjPSeq seq     = NULL;
    AjPStr enzymes = NULL;
    AjPFile outf   = NULL;
    AjPReport report    = NULL;
    AjPFeattable TabRpt = NULL;
    ajint begin;
    ajint end;
    ajint min;
    ajint max;
    ajint sitelen;
    AjBool alpha;
    AjBool single;
    AjBool blunt;
    AjBool sticky;
    AjBool ambiguity;
    AjBool plasmid;
    AjBool commercial;
    AjBool nameit;
    AjBool limit;
    AjBool frags;

    AjPFile enzfile = NULL;
    AjPFile equfile = NULL;

    AjPStr name = NULL;

    AjPTable table = NULL;

    ajint hits;


    AjPList l;
    AjBool  ifrag;

    
    embInit("restrict", argc, argv);

    seqall     = ajAcdGetSeqall("sequence");
    enzymes    = ajAcdGetString("enzymes");
    report     = ajAcdGetReport("outfile");
    min        = ajAcdGetInt("min");
    max        = ajAcdGetInt("max");
    sitelen    = ajAcdGetInt("sitelen");
    blunt      = ajAcdGetBool("blunt");
    sticky     = ajAcdGetBool("sticky");
    single     = ajAcdGetBool("single");
    alpha      = ajAcdGetBool("alphabetic");
    ambiguity  = ajAcdGetBool("ambiguity");
    plasmid    = ajAcdGetBool("plasmid");
    commercial = ajAcdGetBool("commercial");
    limit      = ajAcdGetBool("limit");
    frags      = ajAcdGetBool("fragments");
    nameit     = ajAcdGetBool("name");
    enzfile      = ajAcdGetDatafile("datafile");
    ifrag      = ajAcdGetBool("solofragment");
    
    /* obsolete. Can be uncommented in acd file and here to reuse */

    /* outf      = ajAcdGetOutfile("originalfile"); */

    if (!blunt  && !sticky)
	ajFatal("Blunt/Sticky end cutters shouldn't both be disabled.");


    if(single)
	max = min = 1;

    table = ajStrTableNew(EQUGUESS);


    /* read the local file of enzymes names */
    restrict_read_file_of_enzyme_names(&enzymes);

    if(!enzfile)
    {
	ajFileDataNewC(ENZDATA,&enzfile);
	if(!enzfile)
	    ajFatal("Cannot locate enzyme file. Run REBASEEXTRACT");
    }

    if(limit)
    {
	ajFileDataNewC(EQUDATA,&equfile);
	if(!equfile)
	    limit = ajFalse;
	else
	{
	    restrict_read_equiv(equfile,table);
	    ajFileClose(&equfile);
	}
    }



    while(ajSeqallNext(seqall, &seq))
    {
	begin = ajSeqallBegin(seqall);
	end   = ajSeqallEnd(seqall);
	ajFileSeek(enzfile,0L,0);

	TabRpt = ajFeattableNewSeq(seq);

	l = ajListNew();
	hits = embPatRestrictMatch(seq,begin,end,enzfile,enzymes,sitelen,
				   plasmid,ambiguity,min,max,blunt,sticky,
				   commercial,l);

	ajDebug("Restrict found %d hits\n", hits);

	if(hits)
	{
	    name = ajStrNewC(ajSeqGetNameC(seq));
	    restrict_reportHits(report, seq, TabRpt,
				l,hits,begin,end,
				ambiguity,min,max,
				plasmid,blunt,sticky,sitelen,
				limit,table,
				alpha,frags,nameit,ifrag);
	    if(outf)
		restrict_printHits(outf,l,name,hits,begin,end,
				   ambiguity,min,max,
				   plasmid,blunt,sticky,sitelen,
				   limit,table,
				   alpha,frags,nameit);
	    ajStrDel(&name);
	}

	ajReportWrite(report, TabRpt, seq);
	ajFeattableDel(&TabRpt);

	ajListFree(&l);
    }


    ajListDel(&l);
    ajSeqDel(&seq);
    ajFileClose(&enzfile);
    ajFileClose(&outf);

    ajReportClose(report);
    ajReportDel(&report);

    ajSeqallDel(&seqall);
    ajStrDel(&enzymes);

    ajStrTableFree(&table);

    embExit();

    return 0;
}




/* @funcstatic restrict_printHits *********************************************
**
** Print restriction sites
**
** @param [w] outf [AjPFile] outfile
** @param [u] l [AjPList] hits
** @param [r] name [const AjPStr] sequence name
** @param [r] hits [ajint] number of hits
** @param [r] begin [ajint] start position
** @param [r] end [ajint] end position
** @param [r] ambiguity [AjBool] allow ambiguities
** @param [r] mincut [ajint] minimum cuts
** @param [r] maxcut [ajint] maximum cuts
** @param [r] plasmid [AjBool] circular
** @param [r] blunt [AjBool] allow blunt cutters
** @param [r] sticky [AjBool] allow sticky cutters
** @param [r] sitelen [ajint] length of cut site
** @param [r] limit [AjBool] limit count
** @param [r] table [const AjPTable] supplier table
** @param [r] alpha [AjBool] alphabetic sort
** @param [r] frags [AjBool] show fragment lengths
** @param [r] nameit [AjBool] show name
** @@
******************************************************************************/

static void restrict_printHits(AjPFile outf, AjPList l, const AjPStr name,
			       ajint hits, ajint begin, ajint end,
			       AjBool ambiguity, ajint mincut, ajint maxcut,
			       AjBool plasmid, AjBool blunt, AjBool sticky,
			       ajint sitelen, AjBool limit,
			       const AjPTable table,
			       AjBool alpha, AjBool frags,
			       AjBool nameit)
{
    EmbPMatMatch m = NULL;
    AjPStr ps = NULL;
    ajint *fa = NULL;
    ajint *fx = NULL;
    ajint fc   = 0;
    ajint fn   = 0;
    ajint fb   = 0;
    ajint last = 0;

    AjPStr value = NULL;

    ajint i;
    ajint c = 0;

    ps = ajStrNew();
    fn = 0;


    ajFmtPrintF(outf,"# Restrict of %S from %d to %d\n#\n",
		name,begin,end);
    ajFmtPrintF(outf,"# Minimum cuts per enzyme: %d\n",mincut);
    ajFmtPrintF(outf,"# Maximum cuts per enzyme: %d\n",maxcut);
    ajFmtPrintF(outf,"# Minimum length of recognition site: %d\n",
		sitelen);
    if(blunt)
	ajFmtPrintF(outf,"# Blunt ends allowed\n");

    if(sticky)
	ajFmtPrintF(outf,"# Sticky ends allowed\n");

    if(plasmid)
	ajFmtPrintF(outf,"# DNA is circular\n");
    else
	ajFmtPrintF(outf,"# DNA is linear\n");

    if(!ambiguity)
	ajFmtPrintF(outf,"# No ambiguities allowed\n");
    else
	ajFmtPrintF(outf,"# Ambiguities allowed\n");



    hits = embPatRestrictRestrict(l,hits,!limit,alpha);

    if(limit)
	for(i=0;i<hits;++i)
	{
	    ajListPop(l,(void **)&m);
	    value = ajTableGet(table,m->cod);
	    if(value)
		ajStrAssignS(&m->cod,value);
	    ajListPushApp(l,(void *)m);
	}

    
    if(alpha && limit)
	ajListSort2(l,embPatRestrictNameCompare, embPatRestrictStartCompare);


    if(frags)
    {
	fa = AJALLOC(hits*2*sizeof(ajint));
	fx = AJALLOC(hits*2*sizeof(ajint));
    }


    ajFmtPrintF(outf,"# Number of hits: %d\n",hits);
    ajFmtPrintF(outf,"# Base Number\tEnzyme\t\tSite\t\t5'\t3'\t[5'\t3']\n");
    for(i=0;i<hits;++i)
    {
	ajListPop(l,(void **)&m);
	if(!plasmid && m->circ12)
	{
	    embMatMatchDel(&m);
	    continue;
	}


	if(limit)
	{
	    value = ajTableGet(table,m->cod);

	    if(value)
		ajStrAssignS(&m->cod,value);
	}


	ajFmtPrintF(outf,"\t%-d\t%-16s%-16s%d\t%d\t",m->start,
		    ajStrGetPtr(m->cod),ajStrGetPtr(m->pat),m->cut1,
		    m->cut2);
	if(frags)
	    fa[fn++] = m->cut1;

	if(m->cut3 || m->cut4)
	{
	    if(plasmid || !m->circ34)
	    {
		if(frags)
		    fa[fn++] = m->cut3;
		ajFmtPrintF(outf,"%d\t%d",m->cut3,m->cut4);
	    }
	}

	if(nameit)
	    ajFmtPrintF(outf,"  %S",*name);

	ajFmtPrintF(outf,"\n");
	embMatMatchDel(&m);
    }



    if(frags)
    {
	ajSortIntInc(fa,fn);
	ajFmtPrintF(outf,"\n\nFragment lengths:\n");

	if(!fn || (fn==1 && plasmid))
	    ajFmtPrintF(outf,"    %d\n",end-begin+1);
	else
	{
	    last = -1;
	    fb = 0;
	    for(i=0;i<fn;++i)
	    {
		if((c=fa[i])!=last)
		    fa[fb++] = c;
		last = c;
	    }
	    fn = fb;

	    /* Calc lengths */
	    for(i=0;i<fn-1;++i)
		fx[fc++] = fa[i+1]-fa[i];

	    if(!plasmid)
	    {
		fx[fc++] = fa[0]-begin+1;
		fx[fc++] = end-fa[fn-1];
	    }
	    else
		fx[fc++] = (fa[0]-begin+1)+(end-fa[fn-1]);
	    ajSortIntDec(fx,fc);
	    for(i=0;i<fc;++i)
		ajFmtPrintF(outf,"    %d\n",fx[i]);
	}
	AJFREE(fa);
	AJFREE(fx);
    }

    ajStrDel(&ps);

    return;
}




/* @funcstatic restrict_reportHits ********************************************
**
** Print restriction sites
**
** @param [u] report [AjPReport] report
** @param [r] seq [const AjPSeq] sequence object
** @param [u] TabRpt [AjPFeattable] feature table object to store results
** @param [u] l [AjPList] hits
** @param [r] hits [ajint] number of hits
** @param [r] begin [ajint] start position
** @param [r] end [ajint] end position
** @param [r] ambiguity [AjBool] allow ambiguities
** @param [r] mincut [ajint] minimum cuts
** @param [r] maxcut [ajint] maximum cuts
** @param [r] plasmid [AjBool] circular
** @param [r] blunt [AjBool] allow blunt cutters
** @param [r] sticky [AjBool] allow sticky cutters
** @param [r] sitelen [ajint] length of cut site
** @param [r] limit [AjBool] limit count
** @param [r] table [const AjPTable] supplier table
** @param [r] alpha [AjBool] alphabetic sort
** @param [r] frags [AjBool] show fragment lengths
** @param [r] nameit [AjBool] show name
** @param [r] ifrag [AjBool] show fragments for individual REs
** @@
******************************************************************************/

static void restrict_reportHits(AjPReport report, const AjPSeq seq,
				AjPFeattable TabRpt, AjPList l,
				ajint hits, ajint begin, ajint end,
				AjBool ambiguity, ajint mincut, ajint maxcut,
				AjBool plasmid, AjBool blunt, AjBool sticky,
				ajint sitelen, AjBool limit,
				const AjPTable table,
				AjBool alpha, AjBool frags,
				AjBool nameit, AjBool ifrag)
{
    AjPFeature gf  = NULL;
    EmbPMatMatch m = NULL;
    AjPStr ps = NULL;
    ajint *fa = NULL;
    ajint *fx = NULL;
    ajint fc = 0;
    ajint fn = 0;
    ajint fb = 0;
    ajint last = 0;

    AjPStr value   = NULL;
    AjPStr tmpStr  = NULL;
    AjPStr fthit   = NULL;
    AjPStr fragStr = NULL;
    AjPStr codStr  = NULL;
    AjPStr patStr  = NULL;
    
    ajint i;
    ajint j;
    
    ajint c = 0;
    ajint len;
    ajint cend;
    
    AjPInt farray = NULL;
    ajint  nfrags;



    farray  = ajIntNew();
    ps      = ajStrNew();
    fragStr = ajStrNew();
    codStr  = ajStrNew();
    patStr  = ajStrNew();
    
    fn = 0;
    len = ajSeqGetLen(seq);
    
    ajStrAssignC(&fthit, "hit");

    ajFmtPrintAppS(&tmpStr,"Minimum cuts per enzyme: %d\n",mincut);
    ajFmtPrintAppS(&tmpStr,"Maximum cuts per enzyme: %d\n",maxcut);
    ajFmtPrintAppS(&tmpStr,"Minimum length of recognition site: %d\n",
		sitelen);
    if(blunt)
	ajFmtPrintAppS(&tmpStr,"Blunt ends allowed\n");

    if(sticky)
	ajFmtPrintAppS(&tmpStr,"Sticky ends allowed\n");

    if(plasmid)
	ajFmtPrintAppS(&tmpStr,"DNA is circular\n");
    else
	ajFmtPrintAppS(&tmpStr,"DNA is linear\n");

    if(!ambiguity)
	ajFmtPrintAppS(&tmpStr,"No ambiguities allowed\n");
    else
	ajFmtPrintAppS(&tmpStr,"Ambiguities allowed\n");


    hits = embPatRestrictRestrict(l,hits,!limit,alpha);

    if(frags)
    {
	fa = AJALLOC(hits*2*sizeof(ajint));
	fx = AJALLOC(hits*2*sizeof(ajint));
    }

    ajReportSetHeader(report, tmpStr);


    if(limit)
	for(i=0;i<hits;++i)
	{
	    ajListPop(l,(void **)&m);
	    value = ajTableGet(table,m->cod);
	    if(value)
		ajStrAssignS(&m->cod,value);
	    ajListPushApp(l,(void *)m);
	}
    
    if(alpha && limit)
	ajListSort2(l,embPatRestrictNameCompare, embPatRestrictStartCompare);

    for(i=0;i<hits;++i)
    {
	ajListPop(l,(void **)&m);
	ajListPushApp(l,(void *)m);	/* Might need for ifrag display */
	if(!plasmid && m->circ12)
	    continue;

	if(m->forward)
	    cend = m->start + ajStrGetLen(m->pat) - 1;
	else
	    cend = m->start - ajStrGetLen(m->pat) + 1;
	

	gf = ajFeatNewII (TabRpt,
			   m->start, cend);
	ajFmtPrintS(&tmpStr, "*enzyme %S", m->cod);
	ajFeatTagAdd(gf,  NULL, tmpStr);
	ajFmtPrintS(&tmpStr, "*site %S", m->pat);
	ajFeatTagAdd(gf,  NULL, tmpStr);
	ajFmtPrintS(&tmpStr, "*5prime %d", m->cut1);
	ajFeatTagAdd(gf,  NULL, tmpStr);
	ajFmtPrintS(&tmpStr, "*3prime %d", m->cut2);
	ajFeatTagAdd(gf,  NULL, tmpStr);

	if(frags)
	    fa[fn++] = m->cut1;

	if(m->cut3 || m->cut4)
	{
	    if(plasmid || !m->circ34)
	    {
		if(frags)
		    fa[fn++] = m->cut3;
		ajFmtPrintS(&tmpStr, "*5primerev %d", m->cut3);
		ajFeatTagAdd(gf,  NULL, tmpStr);
		ajFmtPrintS(&tmpStr, "*3primerev %d", m->cut4);
		ajFeatTagAdd(gf,  NULL, tmpStr);
	    }
	}
    }


    ajStrAssignC(&tmpStr, "");


    if(frags)
    {
        ajStrAssignC(&tmpStr, "");
	ajSortIntInc(fa,fn);
	ajFmtPrintAppS(&tmpStr,"Fragment lengths:\n");

	if(!fn || (fn==1 && plasmid))
	    ajFmtPrintAppS(&tmpStr,"    %d\n",end-begin+1);
	else
	{
	    last = -1;
	    fb = 0;
	    for(i=0;i<fn;++i)
	    {
		if((c=fa[i])!=last)
		    fa[fb++]=c;
		last=c;
	    }
	    fn = fb;

	    /* Calc lengths */
	    for(i=0;i<fn-1;++i)
		fx[fc++] = fa[i+1]-fa[i];

	    if(!plasmid)
	    {
		fx[fc++] = fa[0]-begin+1;
		fx[fc++] = end-fa[fn-1];
	    }
	    else
		fx[fc++] = (fa[0]-begin+1)+(end-fa[fn-1]);
	    ajSortIntDec(fx,fc);

	    for(i=0;i<fc;++i)
		ajFmtPrintAppS(&tmpStr,"    %d\n",fx[i]);
	}

	AJFREE(fa);
	AJFREE(fx);
    }


    if(ifrag)
    {
	ajListSort(l,restrict_enzcompare);

	nfrags = 0;
	ajStrAssignC(&fragStr,"");

	for(i=0;i<hits;++i)
	{
	    ajListPop(l,(void **)&m);
	    ajListPushApp(l,(void *)m);

	    if(!plasmid && m->circ12)
		continue;


	    if(limit)
	    {
		value = ajTableGet(table,m->cod);

		if(value)
		    ajStrAssignS(&m->cod,value);
	    }


	    if(!ajStrGetLen(codStr))
	    {
		ajStrAssignS(&patStr,m->pat);
		ajStrAssignS(&codStr,m->cod);
	    }


	    if(ajStrMatchS(codStr,m->cod))
	    {
		if(m->cut3 || m->cut4)
		    if (plasmid || !m->circ34)
			ajIntPut(&farray,nfrags++,m->cut3);
		ajIntPut(&farray,nfrags++,m->cut1);
	    }
	    else
	    {
		ajFmtPrintAppS(&fragStr,"\n%S:\n[%S]",
			       codStr,patStr);
		ajStrAssignS(&codStr,m->cod);
		ajStrAssignS(&patStr,m->pat);
		
		ajSortIntInc(ajIntInt(farray),nfrags);


		fa   = AJALLOC((nfrags+1)*sizeof(ajint));		
		last = 0;
		for(j=0;j<nfrags;++j)
		{
		    fa[j] = ajIntGet(farray,j) - last;
		    last  = ajIntGet(farray,j);
		}

		if(!(nfrags && plasmid))
		{
		    fa[j] = len - ajIntGet(farray,j-1);
		    ++nfrags;
		}
		else
		{
		    if(nfrags == 1)
			fa[0] = len;
		    else
			fa[0] += (len - ajIntGet(farray,j-1));
		}

		ajSortIntInc(fa,nfrags);

		for(j=0;j<nfrags;++j)
		{
		    if(!(j%6))
			ajFmtPrintAppS(&fragStr,"\n");
		    ajFmtPrintAppS(&fragStr,"\t%d",fa[j]);
		}
		ajFmtPrintAppS(&fragStr,"\n");

		AJFREE(fa);
		nfrags = 0;
		if(m->cut3 || m->cut4)
		    if(plasmid || !m->circ34)
			ajIntPut(&farray,nfrags++,m->cut3);
		ajIntPut(&farray,nfrags++,m->cut1);

	    }
	}


	if(nfrags)
	{
	    ajFmtPrintAppS(&fragStr,"\n%S:\n[%S]",
			   codStr,m->pat);
	    ajStrAssignS(&codStr,m->cod);

	    ajSortIntInc(ajIntInt(farray),nfrags);


	    fa = AJALLOC((nfrags+1)*sizeof(ajint));		
	    last = 0;
	    for(j=0;j<nfrags;++j)
	    {
		fa[j] = ajIntGet(farray,j) - last;
		last  = ajIntGet(farray,j);
	    }

	    if(!(nfrags && plasmid))
	    {
		fa[j] = len - ajIntGet(farray,j-1);
		++nfrags;
	    }
	    else
	    {
		if(nfrags == 1)
		    fa[0] = len;
		else
		    fa[0] += (len - ajIntGet(farray,j-1));
	    }

	    ajSortIntInc(fa,nfrags);

	    for(j=0;j<nfrags;++j)
	    {
		if(!(j%6))
		    ajFmtPrintAppS(&fragStr,"\n");
		ajFmtPrintAppS(&fragStr,"\t%d",fa[j]);
	    }
	    ajFmtPrintAppS(&fragStr,"\n");
	    AJFREE(fa);
	}
    }
    


    if(ifrag)
	ajStrAppendS(&tmpStr,fragStr);
    
    ajReportSetTail (report, tmpStr);

    

    for(i=0;i<hits;++i)
    {
	ajListPop(l,(void **)&m);
	embMatMatchDel(&m);
    }
    
    ajIntDel(&farray);
    ajStrDel(&fragStr);
    ajStrDel(&codStr);
    ajStrDel(&patStr);
    ajStrDel(&ps);
    ajStrDel(&tmpStr);
    ajStrDel(&fthit);

    return;
}




/* @funcstatic restrict_read_equiv ********************************************
**
** Read table of equivalents
**
** @param [u] equfile [AjPFile] equivalent name file
** @param [w] table [AjPTable]  equivalent names
** @@
******************************************************************************/


static void restrict_read_equiv(AjPFile equfile, AjPTable table)
{
    AjPStr line;
    AjPStr key;
    AjPStr value;

    const char *p;

    line = ajStrNew();

    while(ajFileReadLine(equfile,&line))
    {
	p = ajStrGetPtr(line);

	if(!*p || *p=='#' || *p=='!')
	    continue;

	p     = ajSysStrtok(p," \t\n");
	key   = ajStrNewC(p);
	p     = ajSysStrtok(NULL," \t\n");
	value = ajStrNewC(p);
	ajTablePut(table,(const void *)key, (void *)value);
    }

    ajStrDel(&line);

    return;
}




/* @funcstatic restrict_read_file_of_enzyme_names *****************************
**
** If the list of enzymes starts with a '@' if opens that file, reads in
** the list of enzyme names and replaces the input string with the enzyme names
**
** @param [u] enzymes [AjPStr*] enzymes to search for or 'all' or '@file'
** @return [void]
** @@
******************************************************************************/

static void restrict_read_file_of_enzyme_names(AjPStr *enzymes)
{
    AjPFile file = NULL;
    AjPStr line;
    const char *p = NULL;

    if(ajStrFindC(*enzymes, "@") == 0)
    {
	ajStrTrimC(enzymes, "@");	/* remove the @ */
	file = ajFileNewIn(*enzymes);
	if(file == NULL)
	    ajFatal ("Cannot open the file of enzyme names: '%S'", enzymes);

	/* blank off the enzyme file name and replace with the enzyme names */
	ajStrSetClear(enzymes);
	line = ajStrNew();
	while(ajFileReadLine(file, &line))
	{
	    p = ajStrGetPtr(line);

	    if(!*p || *p == '#' || *p == '!')
		continue;

	    ajStrAppendS(enzymes, line);
	    ajStrAppendC(enzymes, ",");
	}
	ajStrDel(&line);

	ajFileClose(&file);
    }

    return;
}




/* @funcstatic restrict_enzcompare *******************************************
**
** Comparison function for ajListSort
**
** @param [r] a [const void*] enzyme1
** @param [r] b [const void*] enzyme2
** @return [ajint] 0 = bases match
** @@
******************************************************************************/


static ajint restrict_enzcompare(const void *a, const void *b)
{
    return strcmp((*(EmbPMatMatch*)a)->cod->Ptr,
		  (*(EmbPMatMatch*)b)->cod->Ptr);
}
