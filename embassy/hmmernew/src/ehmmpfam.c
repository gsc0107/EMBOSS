/* @source ehmmpfam application
**
** EMBOSS wrapper to hmmpfam from Sean Eddy's HMMER package v.2.3.2
** Search one or more sequences against an HMM database.
**
** @author Copyright (C) Jon Ison (jison@ebi.ac.uk)
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
******************************************************************************/

#include "emboss.h"





/* @prog ehmmpfam **********************************************************
**
** EMBOSS wrapper to hmmpfam from Sean Eddy's HMMER package v.2.3.2
** Search one or more sequences against an HMM database.
**
******************************************************************************/

int main(int argc, char **argv)
{
    /* ACD data item variables */
    AjPFile   hmmfile = NULL;
    AjPSeqall seqfile = NULL;
    AjBool        nuc = 0;
    ajint           A = 0;
    float           E = 0.0;
    float           T = 0.0;
    ajint           Z = 0;
    AjBool        acc = ajFalse;
    AjBool     compat = ajFalse;
    ajint         cpu = 0;
    AjBool      cutga = ajFalse;
    AjBool      cuttc = ajFalse;
    AjBool      cutnc = ajFalse;
    float      dome = 0.0;
    float      domt = 0.0;
    AjBool    forward = ajFalse;
    AjBool    nulltwo = ajFalse;
    AjBool        pvm = ajFalse;
    AjBool        xnu = ajFalse;
    AjPFile   outfile = NULL;

    /* Housekeeping variables */
    AjPStr        cmd = NULL;
    AjPStr        tmp = NULL;
    AjPStr        rnd = NULL;    
    AjPSeqout    rndo = NULL;    
    AjPSeq        seq = NULL;    



    /* ACD file processing */
    embInitPV("ehmmpfam",argc,argv,"HMMERNEW",VERSION);

    hmmfile = ajAcdGetInfile("hmmfile");
    seqfile = ajAcdGetSeqall("seqfile");
    nuc     = ajAcdGetBoolean("nuc");
    A       = ajAcdGetInt("A");
    E       = ajAcdGetFloat("E");
    T       = ajAcdGetFloat("T");
    Z       = ajAcdGetInt("Z");
    acc     = ajAcdGetBoolean("acc");
    compat  = ajAcdGetBoolean("compat");
    cpu     = ajAcdGetInt("cpu");
    cutga   = ajAcdGetBoolean("cutga");
    cuttc   = ajAcdGetBoolean("cuttc");
    cutnc   = ajAcdGetBoolean("cutnc");
    dome    = ajAcdGetFloat("dome");
    domt    = ajAcdGetFloat("domt");
    forward = ajAcdGetBoolean("forward");
    nulltwo = ajAcdGetBoolean("nulltwo");
        pvm = ajAcdGetBoolean("pvm");
        xnu = ajAcdGetBoolean("xnu");
    outfile = ajAcdGetOutfile("outfile");





    /* MAIN APPLICATION CODE */
    /* 1. Housekeeping */
    cmd = ajStrNew();
    tmp = ajStrNew();
    rnd = ajStrNew();

    /* 2. Re-write seqfile to a temporary file in a format (fasta) HMMER can understand.
       We cannot just pass the name of seqfile to HMMER as the name provided might be a 
       USA which HMMER would not understand. */
    ajFilenameSetTempname(&rnd);
    rndo = ajSeqoutNew();
    if(!ajSeqoutOpenFilename(rndo, rnd))
	ajFatal("Terminal ajSeqFileNewOut failure. Email EMBOSS helpdesk!\n");
    ajSeqoutSetFormatC(rndo, "fasta");

    while(ajSeqallNext(seqfile, &seq))
	ajSeqoutWriteSeq(rndo, seq);
    ajSeqoutClose(rndo);
    ajSeqoutDel(&rndo);


    /* 2. Build hmmpfam command line */
    /* Command line is built in this order: 
       i.  Application name.
       ii. HMMER 'options' (in order they appear in ACD file)
       iii.HMMER 'options' (that don't appear in ACD file)
       iv. HMMER & new parameters.
       */
    ajStrAssignS(&cmd, ajAcdGetpathC("hmmpfam"));
    if(nuc)
	ajStrAppendC(&cmd, " -n ");
    ajFmtPrintAppS(&cmd, " -A %d -E %f -T %f -Z %d", A, E, T, Z);
    if(acc)
	ajStrAppendC(&cmd, " --acc ");
    if(compat)
	ajStrAppendC(&cmd, " --compat ");
    if(cpu)
	ajFmtPrintAppS(&tmp, " --cpu %d ", cpu);
    if(cutga)
	ajStrAppendC(&cmd, " --cutga ");
    if(cuttc)
	ajStrAppendC(&cmd, " --cuttc ");
    if(cutnc)
	ajStrAppendC(&cmd, " --cutnc ");
    ajFmtPrintAppS(&cmd, " --domE %f --domT %f ", dome, domt);
    if(forward)
	ajStrAppendC(&cmd, " --forward ");
    if(nulltwo)
	ajStrAppendC(&cmd, " --null2 ");
    if(pvm)
	ajStrAppendC(&cmd, " --pvm ");
    if(xnu)
	ajStrAppendC(&cmd, " --xnu ");

    /* Note output redirected to outfile.
       rnd is the name of the rewritten seqfile.  
       MUST specify FASTA format explicitly. */
    ajFmtPrintAppS(&cmd, " --informat FASTA %s %S > %s", 
		   ajFileGetNameC(hmmfile),
		   rnd,
		   ajFileGetNameC(outfile));
    

    /* 3. Close ACD files. */
    ajFileClose(&hmmfile);
    ajSeqallDel(&seqfile);
    ajFileClose(&outfile);


    /* 4. Call hmmpfam.  Use C system call instead of ajSystem
       so that redirect in cmd works ok. */
    ajFmtPrint("\n%S\n\n", cmd);
    system(ajStrGetPtr(cmd));


    /* 5. Exit cleanly */
    ajFmtPrintS(&tmp, "rm %S", rnd);
    system(ajStrGetPtr(tmp)); 

    ajStrDel(&cmd);
    ajStrDel(&tmp);
    ajStrDel(&rnd);

    embExit();

    return 0;
}
