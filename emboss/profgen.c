/* @source profgen application
**
** Generates various profiles for each alignment in a directory.
**
** @author: Copyright (C) Ranjeeva Ranasinghe (rranasin@hgmp.mrc.ac.uk)
** @author: Copyright (C) Jon Ison (jison@hgmp.mrc.ac.uk)
** @@
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
**  Application name
**  profgen
**
**  
**  
**  Summary
**  Generates various profiles for each alignment in a directory.
**  
**
**  
**  Input and Output
**  profgen reads a directory of scop family alignments file and generates one
**  of three types of profile for each alignment.  The type of profile may a 
**  simple frequency matrix, a Gribskov profile or a Hennikoff profile.
**  The scop Sunid (an integer) of a family will be used as the base name for
**  its profile file. The paths and extensions for the alignment (input) and
**  profile (output) files are provided by the user. 
**
**  
**   
**  Sister applications
**  A 'scop family alignment file' contains a sequence alignment of domains 
**  belonging to the same scop family.  The file is in embl-like format and is
**  annotated with records describing the SCOP classification of the family.  A
**  scop family alignment file generatd by scopalign will contain a structure-
**  based sequence alignment of domains of known structure only.  Such 
**  alignments can be extended with sequence relatives (of unknown structure)
**  to the family in question by using seqalign.
**  The profile output file is used by modelscan.
** 
**   
**  
**  Notes
**  Functions from the emboss application prophecy have been incorporated.
**  
**  
**  
**  Known bugs & warnings
**  
**  
**  
**  Description
**  This program is part of a suite of EMBOSS applications that directly or 
**  indirectly make use of the protein structure databases pdb and scop.  
**  This program is part of an experimental analysis pipeline described in an
**  accompanying document.  We provide the software in the hope that it will
**  be useful.  The applications were designed for specific research purposes
**  and may not be useful or reliable in contexts other than the described 
**  pipeline.  The development of the suite was coordinated by Jon Ison to
**  whom enquiries and bug reports should be sent (email jison@hgmp.mrc.ac.uk).
**  
**  Profgen provides a convenient means to generate various types of profile 
**  for scop families.  The profiles are descriptive of the family and can be
**  used to identify sequence relatives of the family. 
**  
**  
**  
**  Algorithm
**  
**  
**  
**  Usage 
**  An example of interactive use of profgen is shown below.
**  
**  Unix % 
**  Unix % profgen
**  Generates various profiles for each alignment in a directory.
**  Location of scop alignment files (input) [./]: /test_data/
**  Extension of scop alignment files [.ealign]: 
**  Profile type
**           F : Frequency
**           G : Gribskov
**           H : Henikoff
**  Select type [F]: g
**  Scoring matrix [Epprofile]: 
**  Gap opening penalty [3.0]: 
**  Gap extension penalty [0.3]: 
**  Location of Gribskov profile files (output) [./]: /test_data
**  Extension of Gribskov profile files [.gribs]: 
**  
**  A Gribskov profile was generated from each alignment with the file 
**  extension .ealign in the directory /test_data.  The residue 
**  substitution matrix Epprofile, a gap insertion penalty of 3.0 and a gap
**  extension penalty of 0.3 were used in generating the profiles.  Profile
**  files with the file extension .gribs were written to /test_data.
**  
**  The following command line would achieve the same result.
**  profgen  /test_data/ .ealign -type G -datafile  Epprofile -open 3.0 
**  -extension 0.3 -gbpfpath /test_data -gbpfextn .gribs
**  
**  
**  
**  Input file format
**  The format of the scop alignment file is described in scopalign.c
**  
**  
**  
**  Output file format
**  The format of the profile file is described in the prophecy documentation.
**  
**  
**  
**  Data files
**  profgen requires a residue substitution matrix.
**  
**  
**  
**  Diagnostic error messages
**  
**  
**  
**  Authors
**  Ranjeeva Ranasinghe (rranasin@hgmp.mrc.ac.uk)
**  Jon Ison (jison@hgmp.mrc.ac.uk)
**  
**  
**  
**  References
**  Please cite the authors.
**  
**  
**  
******************************************************************************/

#include "emboss.h"
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define AZ 27
#define GRIBSKOV_LENGTH 27
#define HENIKOFF_LENGTH 27


static void profgen_simple_matrix(AjPSeqset seqset, AjPFile outf, AjPStr name,
                   ajint threshold);
static void profgen_gribskov_profile(AjPSeqset seqset, float **sub,
                      AjPFile outf, AjPStr name, ajint threshold,
                      float gapopen, float gapextend, AjPStr *cons);
static void profgen_henikoff_profile(AjPSeqset seqset, AjPMatrixf imtx, float **sub,
                      ajint threshold, AjPSeqCvt cvt, AjPFile outf, AjPStr name,
                      float gapopen, float gapextend, AjPStr *cons);


/* @prog profgen****** *******************************************************
**
** Generates various profiles for each alignment in a directory.
**
******************************************************************************/

int main(int argc, char **argv)
{

    AjPStr infpath    = NULL;      /* path to directory containing extended alignments */
    AjPStr infextn    = NULL;      /* file extension of extended alignment files */
    AjPStr smpfpath   = NULL;      /* path to directory  for simple frequency profiles */
    AjPStr smpfextn   = NULL;      /* file extension for simple frequncy profiles */
    AjPStr gbpfpath   = NULL;      /* path to directory for Gribskov profiles */
    AjPStr gbpfextn   = NULL;      /* file extension for simple Grobskov profiles */
    AjPStr hnpfpath   = NULL;      /* path to directory for Henikoff profiles */
    AjPStr hnpfextn   = NULL;      /* file extension for Henikoff profiles */
    AjPStr tmp        = NULL;      /* temparary string variable */
    AjPStr filename   = NULL;      /* name of a extended alignment file */
    AjPStr cons       = NULL;
    AjPStr outfname   = NULL;      /* name of output file */
    /*AjPStr name       = NULL;       name of profile out file */

    char *p           = NULL;      /* pointer to type */

    AjPFile inf       = NULL;      /* file pointer to input extended alignment file */
    AjPFile outf      = NULL;      /* file pointer to output profile */

    AjPList list      = NULL;      /* a list of file names */
   
    AjPStr *type      = NULL;      /* the type of profile to be generated */

    ajint threshold   = 0;         /* only for frequency matrices */
    ajint i;                       /* loop iterator */
    ajint posdash     = 0;
    ajint posdot      = 0;

    float **sub       = NULL;
    float gapopen     = 0.0;       /* gap open penalty */
    float gapextend   = 0.0;       /* gap extention  penalty */

    AjPSeqCvt cvt     = NULL;
    AjPMatrixf matrix = NULL;      /* the matrix to be used */
    
    AjPSeqset  seqset = NULL;      /* input for profile generating functions */
    
    AjPSeq seq        = NULL;      /* sequence object */

    AjPScopalg scopalg= NULL;      /* for each input file */
            
    embInit("profgen",argc,argv);

    type      = ajAcdGetList("type");
    infpath   = ajAcdGetString("infpath");
    infextn   = ajAcdGetString("infextn");
    smpfpath  = ajAcdGetString("smpfpath");
    smpfextn  = ajAcdGetString("smpfextn");
    gbpfpath  = ajAcdGetString("gbpfpath");
    gbpfextn  = ajAcdGetString("gbpfextn");
    hnpfpath  = ajAcdGetString("hnpfpath");
    hnpfextn  = ajAcdGetString("hnpfextn");
    
    threshold = ajAcdGetInt("threshold");
    matrix    = ajAcdGetMatrixf("datafile");
    gapopen   = ajAcdGetFloat("open");
    gapextend = ajAcdGetFloat("extension");
  
    filename  = ajStrNew();
    outfname  = ajStrNew();
    tmp       = ajStrNew();
    cons      = ajStrNewC("");
  
    list      = ajListNew();

    gapopen   = ajRoundF(gapopen,8);
    gapextend = ajRoundF(gapextend,8);


    seq = ajSeqNew();

    /* Check directories */
    if((!ajFileDir(&infpath)) || (!(infextn)))
        ajFatal("Could not open extended alignment directory");    
    
    p = ajStrStr(*type);
    
    /* Create list of files in the path */
    ajStrAssC(&tmp, "*");               /* assign a wildcard to tmp */
        
    if((ajStrChar(infextn, 0)=='.'))    /* checks if the file extension starts with "." */
        ajStrApp(&tmp, infextn);        /* assign the acd input file extension to tmp */
 
    /* this picks up situations where the user has specified an extension without a "." */
    else
    {
        ajStrAppC(&tmp, ".");           /* assign "." to tmp */  
        ajStrApp(&tmp, infextn);        /* append tmp with a user specified extension */  
    }   

    /* all files containing extended alignments will be in a list */
    ajFileScan(infpath, tmp, &list, ajFalse, ajFalse, NULL, NULL, ajFalse, NULL);    

    
    /* read each each extended alignment file and run prophecy to generate profile */
    while(ajListPop(list,(void **)&filename))
    {  
        if((inf = ajFileNewIn(filename)) == NULL)
        {
            ajWarn("Could not open file %S\n",filename);
            continue;
        }

        else
        {
            ajXyzScopalgRead(inf,&scopalg);

            seqset   = ajSeqsetNew();
            
            /* fill the seqset */
            for(i=0; i<scopalg->N; i++)
            {
                ajStrAssS(&seq->Acc,scopalg->Codes[i]);
                ajStrAssS(&seq->Seq,scopalg->Seqs[i]);
                ajSeqsetApp(seqset,seq);
            }


            /* create simple frequency profile */
            if(*p=='F')
            {
                /* create an output filename */
                /* Add a '.' to outextn if one does not already exist */
		/* checks if the file extension starts with "." */
                if((ajStrChar(smpfextn, 0)!='.'))       
                    ajStrInsertC(&smpfextn, 0, ".");
                
                /* Create the name of the output file */
                posdash = ajStrRFindC(filename, "/");
                posdot  = ajStrRFindC(filename, ".");
                
                if(posdash >= posdot)
                    ajFatal("Could not create filename. Email rranasin@hgmp.mrc.ac.uk");
                else
                {
                    ajStrAppSub(&outfname, filename, posdash+1, posdot-1);
                    ajStrApp(&outfname,smpfextn);
                }
                
                /* create the output stream */
                outf = ajFileNewOutD(smpfpath,outfname);
                
                /* create a simple frequency  profile */
                profgen_simple_matrix(seqset,outf,outfname,threshold);

                /* close file and clean up*/
                ajFileClose(&outf);
                ajStrAssC(&outfname,"");
            }
	                
            else if(*p=='G')
            {

                /* create an output filename */
                /* Add a '.' to outextn if one does not already exist */
		/* checks if the file extension starts with "." */
                if((ajStrChar(gbpfextn, 0)!='.'))        
                    ajStrInsertC(&gbpfextn, 0, ".");
                
                /* Create the name of the output file */
                posdash = ajStrRFindC(filename, "/");
                posdot  = ajStrRFindC(filename, ".");
                
                if(posdash >= posdot)
                    ajFatal("Could not create filename. Email rranasin@hgmp.mrc.ac.uk");
                else
                {
                    ajStrAppSub(&outfname, filename, posdash+1, posdot-1);
                    ajStrApp(&outfname,gbpfextn);
                }
                
                /* create the output stream */
                outf = ajFileNewOutD(gbpfpath,outfname);

                /* create a Gribskov profile */
                profgen_gribskov_profile(seqset,sub,outf,outfname,
					 threshold,gapopen,gapextend,&cons);

                /* close file and clean up*/
                ajFileClose(&outf);
                ajStrAssC(&outfname,"");
            }
	    
            else if(*p=='H')
            {
                /* create an output filename */
                /* Add a '.' to outextn if one does not already exist */
		/* checks if the file extension starts with "." */
                if((ajStrChar(hnpfextn, 0)!='.'))       
                    ajStrInsertC(&hnpfextn, 0, ".");
                
                /* Create the name of the output file */
                posdash = ajStrRFindC(filename, "/");
                posdot  = ajStrRFindC(filename, ".");
                
                if(posdash >= posdot)
                    ajFatal("Could not create filename. Email rranasin@hgmp.mrc.ac.uk");
                else
                {
                    ajStrAppSub(&outfname, filename, posdash+1, posdot-1);
                    ajStrApp(&outfname,hnpfextn);
                }
                
                /* create the output stream */
                outf = ajFileNewOutD(hnpfpath,outfname);
                
                /* create a Henikoff profile */
                profgen_henikoff_profile(seqset,matrix,sub,threshold,
					 cvt,outf,outfname,gapopen,gapextend,&cons);
                
                /* close file and clean up*/
                ajFileClose(&outf);
                ajStrAssC(&outfname,"");
            }

            else
                ajFatal("Bad type selected\n");

            /* clean up jobs after each file */
            ajXyzScopalgDel(&scopalg);

            ajSeqsetDel(&seqset);
            ajFileClose(&inf);

	    ajStrDel(&filename);

            /* clean up the seqs */
        }
    }
    
    

    /*clean up */
    ajSeqDel(&seq);
    ajStrDel(&infpath);
    ajStrDel(&infextn);
    ajStrDel(&smpfpath);
    ajStrDel(&smpfextn);
    ajStrDel(&gbpfpath);
    ajStrDel(&gbpfextn);
    ajStrDel(&hnpfpath);
    ajStrDel(&hnpfextn);
    ajStrDel(&tmp);
    ajStrDel(&outfname);
    ajStrDel(&cons);
    
    ajListDel(&list);
    

    /* exit */ 
    ajExit();
    return 0;
}

/* @funcstatic  profgen_simple_matrix ***************************************
**
** Undocumented.
**
** @param [?] seqset [AjPSeqset] Undocumented
** @param [?] outf [AjPFile] Undocumented
** @param [?] name [AjPStr] Undocumented
** @param [?] threshold [ajint] Undocumented
** @@
******************************************************************************/
static void profgen_simple_matrix(AjPSeqset seqset, AjPFile outf, AjPStr name,
                   ajint threshold)
{
    char *p;
    ajint nseqs;
    ajint mlen;
    ajint len;
    ajint i;
    ajint j;
    ajint x;
    ajint px;
    
    ajint maxscore;
    ajint score;
    ajint *matrix[AZ+2];
    AjPStr cons=NULL;
    
    nseqs = ajSeqsetSize(seqset);
    if(nseqs<2)
        ajFatal("Insufficient sequences (%d) to create a matrix",nseqs);

    mlen = ajSeqsetLen(seqset);
    
    /* Check sequences are the same length. Warn if not */
    for(i=0;i<nseqs;++i)
    {
        p = ajSeqsetSeq(seqset,i);
        if(strlen(p)!=mlen)
            ajWarn("Sequence lengths are not equal!");
    }
    
    for(i=0;i<AZ+2;++i)
        AJCNEW0(matrix[i], mlen);

    /* Load matrix */
    for(i=0;i<nseqs;++i)
    {
        p = ajSeqsetSeq(seqset,i);      
        len = strlen(p);
        for(j=0;j<len;++j)
        {
            x = toupper((ajint)*p++);
            ++matrix[ajAZToInt(x)][j];
        }
    }

    /* Get consensus sequence */
    cons = ajStrNew();
    for(i=0;i<mlen;++i)
    {
        px=x=-INT_MAX;
        for(j=0;j<AZ-1;++j)
            if(matrix[j][i]>x)
            {
                x=matrix[j][i];
                px=j;
            }
        ajStrAppK(&cons,(char)(px+'A'));
    }
    
    /* Find maximum score for matrix */
    maxscore=0;
    for(i=0;i<mlen;++i)
    {
        for(j=score=0;j<AZ;++j)
            score = AJMAX(score,matrix[j][i]);
        maxscore += score;
    }

    ajFmtPrintF(outf,"# Pure Frequency Matrix\n");
    ajFmtPrintF(outf,"# Columns are amino acid counts A->Z\n");
    ajFmtPrintF(outf,"# Rows are alignment positions 1->n\n");
    ajFmtPrintF(outf,"Simple\n");
    ajFmtPrintF(outf,"Name\t\t%s\n",ajStrStr(name));
    ajFmtPrintF(outf,"Length\t\t%d\n",mlen);
    ajFmtPrintF(outf,"Maximum score\t%d\n",maxscore);
    ajFmtPrintF(outf,"Thresh\t\t%d\n",threshold);
    ajFmtPrintF(outf,"Consensus\t%s\n",ajStrStr(cons));


    for(i=0;i<mlen;++i)
    {
        for(j=0;j<AZ;++j)
            ajFmtPrintF(outf,"%-2d ",matrix[j][i]);
        ajFmtPrintF(outf,"\n");
    }
    
    for(i=0;i<AZ+2;++i)
        AJFREE (matrix[i]);

    ajStrDel(&cons);

    return;
}

/* @funcstatic  profgen_gribskov_profile ************************************
**
** Undocumented.
**
** @param [?] seqset [AjPSeqset] Undocumented
** @param [?] sub [float**] Undocumented
** @param [?] outf [AjPFile] Undocumented
** @param [?] name [AjPStr] Undocumented
** @param [?] threshold [ajint] Undocumented
** @param [?] gapopen [float] Undocumented
** @param [?] gapextend [float] Undocumented
** @param [?] cons [AjPStr*] Undocumented
** @@
******************************************************************************/
static void profgen_gribskov_profile(AjPSeqset seqset, float **sub,
                      AjPFile outf, AjPStr name, ajint threshold,
                      float gapopen, float gapextend, AjPStr *cons)
{
    AjPMatrixf matrix=0;
    AjPSeqCvt cvt=0;
    AjPStr mname=NULL;
    
    float **mat;
    ajint nseqs;
    ajint mlen;
    ajint i;
    ajint j;
    static char *valid="ACDEFGHIKLMNPQRSTVWY";
    char *p;
    char *q;
    float score;
    float sum;
    ajint   gsum;
    float   mmax;
    float   pmax;
    float   psum;
    ajint  start;
    ajint  end;
    ajint  pos;
    float  x;
    ajint  px;
    
    float **weights;
    ajint *gaps;

    
    mname=ajStrNewC("Epprofile");
    ajMatrixfRead(&matrix,mname);
    ajStrDel(&mname);
                 
    nseqs = ajSeqsetSize(seqset);
    mlen  = ajSeqsetLen(seqset);

    sub = ajMatrixfArray(matrix);
    cvt = ajMatrixfCvt(matrix);



    /* Set gaps to be maximum length of gap that can occur
     * including that position
     */
    AJCNEW(gaps, mlen);
    for(i=0;i<mlen;++i)
    {
        gsum=0;
        for(j=0;j<nseqs;++j)
        {
            p=ajSeqsetSeq(seqset,j);
            if(i>=strlen(p))
                continue;
            if(ajAZToInt(p[i])!=27)       /* if not a gap */
                continue;
            pos = i;
            while(pos>-1 && ajAZToInt(p[pos])==27)
                --pos;
            start = ++pos;
            pos=i;
            while(pos<mlen && ajAZToInt(p[pos])==27)
                ++pos;
            end = pos-1;
            gsum = AJMAX(gsum, (end-start)+1);
        }
        gaps[i]=gsum;
    }


    /* get maximum score in scoring matrix */
    mmax=0.0;
    p=valid;
    while(*p)
    {
        q=valid;
        while(*q)
        {
            mmax=(mmax>sub[ajSeqCvtK(cvt,*p)][ajSeqCvtK(cvt,*q)]) ? mmax :
                sub[ajSeqCvtK(cvt,*p)][ajSeqCvtK(cvt,*q)];
            ++q;
        }
        ++p;
    }


    /* Create the weight matrix and zero it */
    AJCNEW(weights, mlen);
    for(i=0;i<mlen;++i)
      AJCNEW0(weights[i], GRIBSKOV_LENGTH+1);

    /*
     *  count the number of times each residue occurs at each
     *  position in the alignment
     */
    for(i=0;i<mlen;++i)
        for(j=0;j<nseqs;++j)
        {
            p=ajSeqsetSeq(seqset,j);
            if(i>=strlen(p))
                continue;
            weights[i][ajAZToInt(p[i])] += ajSeqsetWeight(seqset,j);
        }


    px = -INT_MAX;
    for(i=0;i<mlen;++i)
    {
        x = (float)-INT_MAX;
        for(j=0;j<AZ-1;++j)
            if(weights[i][j]>x)
            {
                x=weights[i][j];
                px=j;
            }
        ajStrAppK(cons,(char)(px+'A'));
    }
    
    
    /* Now normalise the weights */
    for(i=0;i<mlen;++i)
        for(j=0;j<GRIBSKOV_LENGTH;++j)
            weights[i][j] /= (float)nseqs;


    /* Create the profile matrix n*GRIBSKOV_LENGTH and zero it */
    AJCNEW(mat, mlen);
    for(i=0;i<mlen;++i)
        AJCNEW0(mat[i],GRIBSKOV_LENGTH);

    /* Fill the profile with aa scores */
    for(i=0;i<mlen;++i)
        for(p=valid;*p;++p)
        {
            sum = 0.0;
            q = valid;
            while(*q)
            {
                score = weights[i][ajAZToInt(*q)];
                score *= (float)(sub[ajSeqCvtK(cvt,*p)][ajSeqCvtK(cvt,*q)]);
                sum += score;
                ++q;
            }
            mat[i][ajAZToInt(*p)] = sum;
        }

    /* Calculate gap penalties */
    for(i=0;i<mlen;++i)
        mat[i][GRIBSKOV_LENGTH-1]= (mmax / (gapopen+gapextend+(float)gaps[i]));


    /* Get maximum matrix score */
    psum=0.0;
    for(i=0;i<mlen;++i)
    {
        pmax= (float)-INT_MAX;
        for(j=0;j<AZ;++j)
            pmax=(pmax>mat[i][j]) ? pmax : mat[i][j];
        psum+=pmax;
    }
    
    /* Print matrix */

    ajFmtPrintF(outf,"# Gribskov Protein Profile\n");
    ajFmtPrintF(outf,"# Columns are amino acids A->Z\n");
    ajFmtPrintF(outf,"# Last column is indel penalty\n");
    ajFmtPrintF(outf,"# Rows are alignment positions 1->n\n");
    ajFmtPrintF(outf,"Gribskov\n");
    ajFmtPrintF(outf,"Name\t\t%s\n",ajStrStr(name));
    ajFmtPrintF(outf,"Matrix\t\tpprofile\n");
    ajFmtPrintF(outf,"Length\t\t%d\n",mlen);
    ajFmtPrintF(outf,"Max_score\t%.2f\n",psum);
    ajFmtPrintF(outf,"Threshold\t%d\n",threshold);
    ajFmtPrintF(outf,"Gap_open\t%.2f\n",gapopen);
    ajFmtPrintF(outf,"Gap_extend\t%.2f\n",gapextend);
    ajFmtPrintF(outf,"Consensus\t%s\n",ajStrStr(*cons));
    
    for(i=0;i<mlen;++i)
    {
        for(j=0;j<GRIBSKOV_LENGTH;++j)
            ajFmtPrintF(outf,"%.2f ",mat[i][j]);
        ajFmtPrintF(outf,"%.2f\n",mat[i][GRIBSKOV_LENGTH-1]);
    }


    for(i=0;i<mlen;++i)
    {
        AJFREE (mat[i]);
        AJFREE (weights[i]);
    }
    AJFREE (mat);
    AJFREE (weights);

    AJFREE (gaps);

    ajMatrixfDel(&matrix);
    

    return;
}

/* @funcstatic  profgen_henikoff_profile ***********************************
**
** Undocumented.
**
** @param [?] seqset [AjPSeqset] Undocumented
** @param [?] matrix [AjPMatrixf] Undocumented
** @param [?] sub [float**] Undocumented
** @param [?] threshold [ajint] Undocumented
** @param [?] cvt [AjPSeqCvt] Undocumented
** @param [?] outf [AjPFile] Undocumented
** @param [?] name [AjPStr] Undocumented
** @param [?] gapopen [float] Undocumented
** @param [?] gapextend [float] Undocumented
** @param [?] cons [AjPStr*] Undocumented
** @@
******************************************************************************/
static void profgen_henikoff_profile(AjPSeqset seqset, AjPMatrixf matrix, float **sub,
                      ajint threshold, AjPSeqCvt cvt, AjPFile outf, AjPStr name,
                      float gapopen, float gapextend, AjPStr *cons)
{
    float **mat;
    ajint nseqs;
    ajint mlen;
    ajint i;
    ajint j;
    static char *valid="ACDEFGHIKLMNPQRSTVWY";
    char *p;
    char *q;
    float score;
    float sum;
    float psum;
    float pmax;
    ajint   gsum;
    ajint   mmax;
    ajint  start;
    ajint  end;
    ajint  pos;
    
    float **weights;
    ajint *gaps;
    ajint *pcnt;

    float x;
    ajint px;
    

    nseqs = ajSeqsetSize(seqset);
    mlen  = ajSeqsetLen(seqset);

    sub = ajMatrixfArray(matrix);
    cvt = ajMatrixfCvt(matrix);


    /* Set gaps to be maximum length of gap that can occur
     * including that position
     */
    AJCNEW(gaps, mlen);
    for(i=0;i<mlen;++i)
    {
        gsum=0;
        for(j=0;j<nseqs;++j)
        {
            p=ajSeqsetSeq(seqset,j);
            if(i>=strlen(p))
                continue;
            if(ajAZToInt(p[i])!=27)
                continue; /* if not a gap */
            pos = i;
            while(pos>-1 && ajAZToInt(p[pos])==27)
                --pos;
            start = ++pos;
            pos=i;
            while(pos<mlen && ajAZToInt(p[pos])==27)
                ++pos;
            end = pos-1;
            gsum = AJMAX(gsum, (end-start)+1);
        }
        gaps[i]=gsum;
    }

    /* get maximum score in scoring matrix */
    mmax=0;
    p=valid;
    while(*p)
    {
        q=valid;
        while(*q)
        {
            mmax=(mmax>sub[ajSeqCvtK(cvt,*p)][ajSeqCvtK(cvt,*q)]) ? mmax :
                sub[ajSeqCvtK(cvt,*p)][ajSeqCvtK(cvt,*q)];
            ++q;
        }
        ++p;
    }


    /* Create the weight matrix and zero it */
    AJCNEW(weights, mlen);
    for(i=0;i<mlen;++i)
        AJCNEW0(weights[i],HENIKOFF_LENGTH+1);

    /*
     *  count the number of times each residue occurs at each
     *  position in the alignment
     */
    for(i=0;i<mlen;++i)
        for(j=0;j<nseqs;++j)
        {
            p=ajSeqsetSeq(seqset,j);
            if(i>=strlen(p))
                continue;
            weights[i][ajAZToInt(p[i])] += ajSeqsetWeight(seqset,j);
        }

    px = -INT_MAX;
    for(i=0;i<mlen;++i)
    {
        x=(float)-INT_MAX;
        for(j=0;j<AZ-1;++j)
            if(weights[i][j]>x)
            {
                x = weights[i][j];
                px=j;
            }
        ajStrAppK(cons,(char)(px+'A'));
    }
    


    /* Count the number of different residues at each position */

    AJCNEW0(pcnt, mlen);

    for(i=0;i<mlen;++i)
        for(j=0;j<HENIKOFF_LENGTH-1;++j)
            if(weights[i][j])
                ++pcnt[i];
    
    /* weights = 1/(num diff res * count of residues at that position */
    for(i=0;i<mlen;++i)
        for(j=0;j<HENIKOFF_LENGTH-1;++j)
            if(weights[i][j])
                weights[i][j] = 1.0/(weights[i][j]*(float)pcnt[i]);


    /* Create the profile matrix n*HENIKOFF_LENGTH */
    AJCNEW(mat, mlen);
    for(i=0;i<mlen;++i)
      AJCNEW0(mat[i],HENIKOFF_LENGTH);
    
    /* Fill the profile with aa scores */
    for(i=0;i<mlen;++i)
        for(p=valid;*p;++p)
        {
            sum = 0.0;
            q = valid;
            while(*q)
            {
                score = weights[i][ajAZToInt(*q)];
                score *= sub[ajSeqCvtK(cvt,*p)][ajSeqCvtK(cvt,*q)];
                sum += score;
                ++q;
            }
            mat[i][ajAZToInt(*p)] = sum;
        }

    /* Calculate gap penalties */
    for(i=0;i<mlen;++i)
        mat[i][HENIKOFF_LENGTH-1]=(mmax / (gapopen+gapextend+
                                          (float)gaps[i]));


    /* Get maximum matrix score */
    psum=0.0;
    for(i=0;i<mlen;++i)
    {
        pmax= (float)-INT_MAX;
        for(j=0;j<HENIKOFF_LENGTH-1;++j)
            pmax=(pmax>mat[i][j]) ? pmax : mat[i][j];
        psum+=pmax;
    }

    /* Print matrix */

    ajFmtPrintF(outf,"# Henikoff Protein Profile\n");
    ajFmtPrintF(outf,"# Columns are amino acids A->Z\n");
    ajFmtPrintF(outf,"# Last column is indel penalty\n");
    ajFmtPrintF(outf,"# Rows are alignment positions 1->n\n");
    ajFmtPrintF(outf,"Henikoff\n");
    ajFmtPrintF(outf,"Name\t\t%s\n",ajStrStr(name));
    ajFmtPrintF(outf,"Matrix\t\t%s\n",ajStrStr(ajMatrixfName(matrix)));
    ajFmtPrintF(outf,"Length\t\t%d\n",mlen);
    ajFmtPrintF(outf,"Max_score\t%.2f\n",psum);
    ajFmtPrintF(outf,"Threshold\t%d\n",threshold);
    ajFmtPrintF(outf,"Gap_open\t%.2f\n",gapopen);
    ajFmtPrintF(outf,"Gap_extend\t%.2f\n",gapextend);
    ajFmtPrintF(outf,"Consensus\t%s\n",ajStrStr(*cons));

    for(i=0;i<mlen;++i)
    {
        for(j=0;j<HENIKOFF_LENGTH;++j)
            ajFmtPrintF(outf,"%.2f ",mat[i][j]);
        ajFmtPrintF(outf,"%.2f\n",mat[i][j-1]);
    }
    

    for(i=0;i<mlen;++i)
    {
        AJFREE (mat[i]);
        AJFREE (weights[i]);
    }
    AJFREE (mat);
    AJFREE (weights);
    AJFREE (gaps);
    AJFREE (pcnt);
    return;
}
