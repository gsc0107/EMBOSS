/* @source needleall application
**
** Many-to-many pairwise alignment
**
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
#include <limits.h>
#include <math.h>




/* @prog needleall ***********************************************************
**
** Many-to-many pairwise sequence alignment using Needleman-Wunsch algorithm
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPAlign align;
    AjPSeqall seqall;
    AjPSeqset seqset;
    const AjPSeq seqa;
    AjPSeq seqb;
    AjPStr alga;
    AjPStr algb;
    AjPStr ss;

    ajuint    lena;
    ajuint    lenb;
    ajint     k;

    const char   *p;
    const char   *q;

    ajint start1 = 0;
    ajint start2 = 0;

    float *path;
    ajint *compass;
    float* ix;
    float* iy;
    float* m;

    AjPMatrixf matrix;
    AjPSeqCvt cvt = 0;
    float **sub;

    float gapopen;
    float gapextend;
    float endgapopen;
    float endgapextend;
    float minscore;
    ajulong maxarr = 1000;  /* arbitrary. realloc'd if needed */
    ajulong len;            

    float score;

    AjBool dobrief = ajTrue;
    AjBool endweight = ajFalse; /*whether end gap penalties should be applied*/

    float id   = 0.;
    float sim  = 0.;
    float idx  = 0.;
    float simx = 0.;

    AjPStr tmpstr = NULL;

    size_t stlen;

    embInit("needleall", argc, argv);

    matrix    = ajAcdGetMatrixf("datafile");
    seqset    = ajAcdGetSeqset("asequence");
    seqall    = ajAcdGetSeqall("bsequence");
    gapopen   = ajAcdGetFloat("gapopen");
    gapextend = ajAcdGetFloat("gapextend");
    endgapopen   = ajAcdGetFloat("endopen");
    endgapextend = ajAcdGetFloat("endextend");
    minscore = ajAcdGetFloat("minscore");
    dobrief   = ajAcdGetBoolean("brief");
    endweight   = ajAcdGetBoolean("endweight");

    align     = ajAcdGetAlign("outfile");

    gapopen = ajRoundF(gapopen, 8);
    gapextend = ajRoundF(gapextend, 8);

    AJCNEW(path, maxarr);
    AJCNEW(compass, maxarr);
    AJCNEW(m, maxarr);
    AJCNEW(ix, maxarr);
    AJCNEW(iy, maxarr);

    alga  = ajStrNew();
    algb  = ajStrNew();
    ss = ajStrNew();

    sub = ajMatrixfArray(matrix);
    cvt = ajMatrixfCvt(matrix);

    if(!ajAlignFormatShowsSequences(align))
        ajAlignSetExternal(align, ajTrue);
    
    while(ajSeqallNext(seqall,&seqb))
    {
        ajSeqTrim(seqb);
        lenb = ajSeqGetLen(seqb);

        for(k=0;k<ajSeqsetGetSize(seqset);k++)
        {
            seqa = ajSeqsetGetseqSeq(seqset, k);
            lena = ajSeqGetLen(seqa);


            if(lenb > (ULONG_MAX/(ajulong)(lena+1)))
                ajFatal("Sequences too big.");

            len = lena*lenb;

            if(len>maxarr)
            {
                stlen = (size_t) len;
                AJCRESIZETRY(path,stlen);
                AJCRESIZETRY(compass,stlen);
                AJCRESIZETRY(m,stlen);
                AJCRESIZETRY(ix,stlen);
                AJCRESIZETRY(iy,stlen);
                if(!path || !compass || !m || !ix || !iy)
                    ajDie("Sequences too big.");
                maxarr=len;
            }


            p = ajSeqGetSeqC(seqa);
            q = ajSeqGetSeqC(seqb);

            ajStrAssignC(&alga,"");
            ajStrAssignC(&algb,"");

            embAlignPathCalcWithEndGapPenalties(p, q, lena, lenb, gapopen,
                    gapextend, endgapopen, endgapextend, path,sub,cvt,
                    m, ix, iy, compass,ajFalse, endweight);

            if(!ajAlignFormatShowsSequences(align))
            {
                score = embAlignGetScoreNWMatrix(path, lena, lenb,
                        &start1, &start2, endweight);
            }
            else
            {
                score = embAlignWalkNWMatrixUsingCompass(path,seqa, seqb,
                        &alga, &algb, lena, lenb, &start1, &start2, compass,
                        endweight);
            }
            if (score > minscore){
                if(!ajAlignFormatShowsSequences(align))
                {
                    ajAlignDefineSS(align, seqa, seqb);
                    ajAlignSetScoreR(align, score);
                }
                else
                {
                    embAlignReportGlobal(align, seqa, seqb, alga, algb,
                            start1, start2,
                            gapopen, gapextend,
                            score, matrix,
                            ajSeqGetOffset(seqa), ajSeqGetOffset(seqb));
                }

                if(!dobrief)
                {
                    embAlignCalcSimilarity(alga,algb,sub,cvt,lena,lenb,&id,
                            &sim, &idx, &simx);
                    ajFmtPrintS(&tmpstr,"Longest_Identity = %5.2f%%\n",
                            id);
                    ajFmtPrintAppS(&tmpstr,"Longest_Similarity = %5.2f%%\n",
                            sim);
                    ajFmtPrintAppS(&tmpstr,"Shortest_Identity = %5.2f%%\n",
                            idx);
                    ajFmtPrintAppS(&tmpstr,"Shortest_Similarity = %5.2f%%",
                            simx);
                    ajAlignSetSubHeaderApp(align, tmpstr);
                }
                ajAlignWrite(align);
                ajAlignReset(align);
            }
        }
    }
    

    if(!ajAlignFormatShowsSequences(align))
    {
        ajMatrixfDel(&matrix);        
    }

    ajAlignClose(align);
    ajAlignDel(&align);
    

    ajSeqallDel(&seqall);
    ajSeqsetDel(&seqset);
    ajSeqDel(&seqb);

    AJFREE(compass);
    AJFREE(path);
    AJFREE(ix);
    AJFREE(iy);
    AJFREE(m);
    
    ajStrDel(&alga);
    ajStrDel(&algb);
    ajStrDel(&ss);
    ajStrDel(&tmpstr);

    embExit();

    return 0;
}
