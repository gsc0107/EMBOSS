/* @source seqfraggle application
**
** Removes fragments from DHF files (domain hits files) or other files of 
** sequences.
**
** @author: Copyright (C) Matt Blades (mblades@hgmp.mrc.ac.uk)
** @author: Copyright (C) Jon Ison (jison@hgmp.mrc.ac.uk)
** @@
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
**

*******************************************************************************
** 
**  SEQFRAGGLE Documentation
**  See http://wwww.emboss.org
**  
**  Please cite the authors and EMBOSS.
**  
**  Rice P, Bleasby A.J.  2000.  EMBOSS - The European Molecular Biology Open 
**  Software Suite.  Trends in Genetics, 15:276-278.  
**  See also http://www.uk.embnet.org/Software/EMBOSS
**  
**  Email jison@rfcgr.mrc.ac.uk.
**  
**  NOTES
**  
****************************************************************************/




#include "emboss.h"

void seqfraggle_getlengths(AjPHitlist hitlist, ajint *num_hits, AjPInt *seq_len_sort, AjPInt *seq_len, AjPInt *seq_ok);
void seqfraggle_getlengths_other(AjPStr temp,  AjPSeqset *seqset, ajint *num_hits, AjPInt *seq_len_sort, AjPInt *seq_len, AjPInt *seq_ok);



/* @prog seqfraggle *******************************************************
**
** Removes fragments from DHF files (domain hits files) or other files of 
** sequences.
**
******************************************************************************/
int main(int argc, char **argv)
{
    AjPList      dhfin          = NULL;  /* Hits files for input                   */
    AjPDir       dhfout         = NULL;  /* Hits files for output                  */

    AjPStr      temp            = NULL;  /* Temp string                            */
    AjPStr      temp2           = NULL;  /* Temp string                            */
    AjPStr      exec            = NULL;  /* The UNIX command line to be executed   */    

    ajint       thresh          = 0;     /* Threshold for definition of fragments  */
    ajint       x               = 0;     /* Loop counters                          */
    ajint       y               = 0;     /* Loop counters                          */
    ajint       median          = 0;     /* Median length of sequence hits         */
    ajint       mid             = 0;     /* Middle value of seq_len array          */
    ajint       num_hits        = 0;     /* Number of hits in file                 */

    float       score           = 0.0;   /* Float for storing length/median value  */
    
    
    AjPInt      seq_len_sort    = NULL;  /* Array to hold length of each hit seq   */
    AjPInt      seq_len         = NULL;  /* Array to hold sorted lengths           */
    AjPInt      seq_ok          = NULL;  /* Array indicating if length is > thresh 
					    If seq_ok array element == 1 then the 
					    sequence is output  */

    AjPFile     hitsPtr         = NULL;  /* Pointer to hits file                   */
    AjPFile     dhfoutPtr      = NULL;  /* Pointer to hits output file            */
     
    AjPHitlist  hitlist         = NULL;  /* Hitlist structure                      */

    AjPSeqset   seqset          = NULL;
    AjPSeqout   seqout          = NULL;
    const AjPSeq      tmpseq          = NULL;


    

    /* Assign strings and list */
    exec         = ajStrNew();


    /* Read data from acd */
    ajNamInit("emboss");
    ajAcdInitP("seqfraggle",argc,argv,"DOMAINATRIX"); 
    dhfin      = ajAcdGetDirlist("dhfin");
    dhfout     = ajAcdGetDirectory("dhfout");
    thresh      = ajAcdGetInt("thresh");
    
/*
    line    = ajStrNew();
    while(ajListPop(dhfin,(void **)&temp))
    {
        if((hitsPtr=ajFileNewIn(temp))==NULL)
        {
            ajFileClose(&hitsPtr);
            ajWarn("Could not open hits file %S", temp);
            ajStrDel(&temp); 
            continue;       
        }

        ajFmtPrint("Processing %S\n", temp);

	if((hitlist = embHitlistReadFasta(hitsPtr)))
	    embHitlistDel(&hitlist);
	else
	    ajFatal("Oooops\n");

	while(ajFileReadLine(hitsPtr,&line));
	
	ajFileClose(&hitsPtr);
    }
    ajStrDel(&line);

    ajExit();
*/

    /* Start of main application loop                         */
    /* determine median length of sequences in each hits file */
    while(ajListPop(dhfin,(void **)&temp))
    {
        /* Open hits file */
        if((hitsPtr=ajFileNewIn(temp))==NULL)
        {
            ajWarn("Could not open hits file %S", temp);
            ajStrDel(&temp); 
            continue;       
        }

        ajFmtPrint("Processing %S\n", temp);

	if((hitlist = embHitlistReadFasta(hitsPtr)))
	    seqfraggle_getlengths(hitlist, &num_hits, &seq_len_sort, &seq_len, &seq_ok);
	else
	    seqfraggle_getlengths_other(temp, &seqset, &num_hits, &seq_len_sort, &seq_len, &seq_ok);

	
	/* if num_hits > 1 then seq_len_sort, seq_len & seq_ok all have to be freed.
	   seqset ALWAYS has to be freed */
	
	if(!num_hits)
	{
	    ajWarn("No hits in hits file %S", temp);
	    ajFileClose(&hitsPtr);
	    if(hitlist)
		embHitlistDel(&hitlist);
	    if(seqset)
		ajSeqsetDel(&seqset);
	    continue;
	}
	


        if(num_hits > 1)
        {
            /* Calculate median length */
            /* Reorder seq_len_sort array into ascending order */
            for(x=0;x<num_hits;x++)
                ajSortIntInc((ajint *) ajIntInt(seq_len_sort), num_hits);

        
            /* If num_hits == even then the median = average of middle two values */
            if((num_hits % 2) == 0)
            {
                mid = (num_hits / 2);
                median = (((ajIntGet(seq_len_sort, mid-1)) + (ajIntGet(seq_len_sort, mid))) / 2); 
            }

            /* else if num == odd number, then median = middle value */ 
            else
            {
                mid = (num_hits / 2);
                median = ajIntGet(seq_len_sort, mid); 
            }
        

            y = 0;
            /* Create array of 1's and 0's */
            for(x=0;x<num_hits;x++)
            {
                score = ((((float)ajIntGet(seq_len, x) / (float)median)) * 100);
                if(score < thresh) 
                    ajIntPut(&seq_ok, x, 0);
                else if(score >= thresh)
                {
                    ajIntPut(&seq_ok, x, 1);
                    y++;
                }
            }
        }
	
	

	/* Create output file */
	if(hitlist)
	{
	    ajStrAssS(&temp2, temp);
	    ajFileDirExtnTrim(&temp2);
	    dhfoutPtr = ajFileNewOutDir(dhfout, temp2);
	}
	else
	{
	    ajStrAssS(&temp2, temp);
	    ajFileDirExtnTrim(&temp2);
	    ajStrInsert(&temp2, 0, ajDirName(dhfout));
	    ajStrAssS(&temp2, ajDirExt(dhfout));
	    ajSeqoutUsa(&seqout, temp2);
	}


	/* Write output file */
	if(hitlist)
	{
	    if(num_hits > 1)
		embHitlistWriteSubsetFasta(dhfoutPtr, hitlist, seq_ok);
	    else
		embHitlistWriteFasta(dhfoutPtr, hitlist);
	}
	else
	{
	    if(num_hits > 1)
	    {
		for(x=0; x<num_hits; x++)
		    if(ajIntGet(seq_ok, x) == 1)
		    {
			tmpseq = ajSeqsetGetSeq(seqset, x);
			ajSeqWrite(seqout, tmpseq);
		    }
	    }
	    else
	    {
		for(x=0; x<num_hits; x++)
		{
		    tmpseq = ajSeqsetGetSeq(seqset, x);
		    ajSeqWrite(seqout, tmpseq);
		}
	    }
	}


	/* Close input and output files and tidy up */
	if(seq_len)
	    ajIntDel(&seq_len);
	if(seq_len_sort)
	    ajIntDel(&seq_len_sort);
	if(seq_ok)
	    ajIntDel(&seq_ok);
	if(seqset)
	    ajSeqsetDel(&seqset);
	if(hitlist)
	{	
	    embHitlistDel(&hitlist);
	    ajFileClose(&dhfoutPtr);
	}

	ajFileClose(&hitsPtr);
	
	ajStrDel(&temp); 
    }
    

    /* Tidy up */
    /* Delete strings and list */
    ajListDel(&dhfin);
    ajDirDel(&dhfout);
    ajStrDel(&exec);


    /* Return */
    ajExit();
    return 0;
}




void seqfraggle_getlengths(AjPHitlist hitlist, ajint *num_hits, AjPInt *seq_len_sort, AjPInt *seq_len, AjPInt *seq_ok)
{
    ajint       len             = 0;     /* length of sequence hit                 */
    ajint       x               = 0;     /* Loop counters                          */


    *num_hits = hitlist->N;

    if(hitlist->N > 1)
    {
	*seq_len_sort = ajIntNewL(hitlist->N);
	*seq_len      = ajIntNewL(hitlist->N);
	*seq_ok       = ajIntNewL(hitlist->N);

	ajIntPut(seq_len_sort, hitlist->N-1, 0);  
	ajIntPut(seq_len, hitlist->N-1, 0);       
	ajIntPut(seq_ok, hitlist->N-1, 0);

	for(x=0; x<hitlist->N; x++)
	{
	    len = hitlist->hits[x]->End - (hitlist->hits[x]->Start-1);
	    ajIntPut(seq_len, x, len);
	    ajIntPut(seq_len_sort, x, len);
	}
	
	return;    
    }
    
    return;
}







void seqfraggle_getlengths_other(AjPStr temp,  AjPSeqset *seqset, ajint *num_hits, AjPInt *seq_len_sort, AjPInt *seq_len, AjPInt *seq_ok)
{
    ajint           len       = 0;        /* length of sequence hit                 */
    ajint           x         = 0;        /* Loop counters                          */
    AjPSeqin        seqin     = NULL;
    const AjPSeq    tmpseq    = NULL;
    
  

    /* Read sequence set (rather than domain hits file) */
    *seqset = ajSeqsetNew();
    /* Set the filename via the USA. ajSeqsetRead interprets it to find the filename. */
    seqin  = ajSeqinNew();
    ajSeqinUsa(&seqin, temp);
    if(!(ajSeqsetRead(*seqset, seqin)))
	ajFatal("SeqsetRead failed in seqsearch_psialigned");

    
    *num_hits = ajSeqsetSize(*seqset);
    

    if(*num_hits > 1)
    {
	*seq_len_sort = ajIntNewL(*num_hits);
	*seq_len      = ajIntNewL(*num_hits);
	*seq_ok       = ajIntNewL(*num_hits);

	ajIntPut(seq_len_sort, *num_hits-1, 0);  
	ajIntPut(seq_len, *num_hits-1, 0);       
	ajIntPut(seq_ok, *num_hits-1, 0);

	for(x=0; x<*num_hits; x++)
	{
	    tmpseq = ajSeqsetGetSeq(*seqset, x);
	    
    	    len = ajSeqLen(tmpseq);
	    ajIntPut(seq_len, x, len);
	    ajIntPut(seq_len_sort, x, len);
	}
	
	ajSeqinDel(&seqin);

	return;
    }


    ajSeqinDel(&seqin);
    
    return;
}
