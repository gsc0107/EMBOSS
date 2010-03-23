/* @source esignalp application
**
** Wrapper for SIGNALP
**
** @author Copyright (C) Alan Bleasby
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


/* @prog esignalp ************************************************************
**
** Wrapper for SIGNALP
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPStr    cl     = NULL;
    AjPSeqset seqset = NULL;
    AjPSeqout seqout = NULL;
    AjPStr    type   = NULL;
    AjBool    plot   = ajFalse;
    AjPStr    fn     = NULL;
    AjPStr    stmp   = NULL;
    AjPStr    method = NULL;
    AjPStr    format = NULL;
    AjPStr    gmode  = NULL;
    AjPStr    synver = NULL;
    AjBool    keep   = ajFalse;
    ajint     trunc  = 0;

    

    AjPStr  outfname = NULL;
    
    
    embInitPV("esignalp", argc, argv, "CBSTOOLS", VERSION);


    seqset  = ajAcdGetSeqset("sequence");
    type    = ajAcdGetListSingle("type");
    method  = ajAcdGetListSingle("method");
    format  = ajAcdGetListSingle("format");
    gmode   = ajAcdGetListSingle("gmode");
    synver  = ajAcdGetListSingle("synver");
    keep    = ajAcdGetBoolean("keepall");
    trunc   = ajAcdGetInt("truncate");
    outfname= ajAcdGetOutfileName("outfile");    

    cl   = ajStrNewS(ajAcdGetpathC("signalp"));
    fn   = ajStrNew();
    stmp = ajStrNew();
    
    ajStrAppendC(&cl, " -t");

    ajFilenameSetTempname(&fn);
    seqout = ajSeqoutNew();
    if(!ajSeqoutOpenFilename(seqout, fn))
	ajFatal("Cannot open temporary file %S",fn);
    ajSeqoutSetFormatC(seqout, "fasta");
    ajSeqoutWriteSet(seqout,seqset);
    ajSeqoutClose(seqout);

    if(ajStrMatchC(type,"euk"))
        ajStrAppendC(&cl," euk");
    else if(ajStrMatchC(type,"gramp"))
        ajStrAppendC(&cl," ""gram+""");
    else
        ajStrAppendC(&cl," ""gram-""");

    if(plot)
        ajStrAppendC(&cl," -g");

    ajFmtPrintS(&stmp," -m %S",method);
    ajStrAppendS(&cl,stmp);

    ajFmtPrintS(&stmp," -f %S",format);
    ajStrAppendS(&cl,stmp);

    if(!ajStrMatchC(gmode,"none"))
    {
        ajFmtPrintS(&stmp," -graphics %S",gmode);
        ajStrAppendS(&cl,stmp);
    }
    
    ajFmtPrintS(&stmp," -synver %S",synver);
    ajStrAppendS(&cl,stmp);


    ajFmtPrintS(&stmp," %S",fn);
    ajStrAppendS(&cl,stmp);


    if(keep)
        ajStrAppendC(&cl," -keepall");

    if(trunc)
    {
        ajFmtPrintS(&stmp," -trunc %d",trunc);
        ajStrAppendS(&cl,stmp);
    }
    
#if 0
    ajFmtPrint("%S\n",cl);
#endif

#if 1
    ajSysExecOutnameAppendS(cl, outfname);
#endif

    ajSysFileUnlinkS(fn);

    ajStrDel(&cl);
    ajStrDel(&type);
    ajStrDel(&method);
    ajStrDel(&format);
    ajStrDel(&gmode);
    ajStrDel(&synver);
    ajStrDel(&stmp);
    ajStrDel(&fn);
    ajSeqoutDel(&seqout);
    ajSeqsetDel(&seqset);
    ajStrDel(&outfname);

    embExit();

    return 0;
}
