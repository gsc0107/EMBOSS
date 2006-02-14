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
    AjBool          n = 0;
    ajint           A = 0;
    float         E = 0.0;
    float         T = 0.0;
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



    /* ACD file processing */
    ajNamInit("emboss");
    ajAcdInitP("ehmmpfam",argc,argv,"HMMERNEW");

    hmmfile = ajAcdGetInfile("hmmfile");
    seqfile = ajAcdGetSeqall("seqfile");
    n       = ajAcdGetBool("n");
    A       = ajAcdGetInt("A");
    E       = ajAcdGetFloat("E");
    T       = ajAcdGetFloat("T");
    Z       = ajAcdGetInt("Z");
    acc     = ajAcdGetBool("acc");
    compat  = ajAcdGetBool("compat");
    cpu     = ajAcdGetInt("cpu");
    cutga   = ajAcdGetBool("cutga");
    cuttc   = ajAcdGetBool("cuttc");
    cutnc   = ajAcdGetBool("cutnc");
    dome    = ajAcdGetFloat("dome");
    domt    = ajAcdGetFloat("domt");
    forward = ajAcdGetBool("forward");
    nulltwo = ajAcdGetBool("nulltwo");
        pvm = ajAcdGetBool("pvm");
        xnu = ajAcdGetBool("xnu");
    outfile = ajAcdGetOutfile("outfile");





    /* MAIN APPLICATION CODE */
    /* 1. Housekeeping */
    cmd = ajStrNew();
    tmp = ajStrNew();


    /* 2. Build hmmpfam command line */
    /* Command line is built in this order: 
       i.  Application name.
       ii. HMMER 'options' (in order they appear in ACD file)
       iii.HMMER 'options' (that don't appear in ACD file)
       iv. HMMER & new parameters.
       */
    ajFmtPrintS(&cmd, "hmmpfam");
    if(n)
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

    /* Note output redirected to outfile */
    ajFmtPrintAppS(&cmd, " %s %S > %s", 
		   ajFileName(hmmfile),
		   ajSeqallGetFilename(seqfile),
		   ajFileName(outfile));
    

    /* 3. Close ACD files. */
    ajFileClose(&hmmfile);
    ajSeqallDel(&seqfile);
    ajFileClose(&outfile);


    /* 4. Call hmmpfam.  Use C system call instead of ajSystem
       so that redirect in cmd works ok. */
    ajFmtPrint("\n%S\n\n", cmd);
    system(ajStrGetPtr(cmd));
    




    /* Exit cleanly */
    ajStrDel(&cmd);
    ajStrDel(&tmp);

    ajExit();

    return 0;
}
