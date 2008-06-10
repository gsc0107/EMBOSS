/* @source echlorop application
**
** Wrapper for chlorop
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


/* @prog echlorop ************************************************************
**
** Wrapper for chlorop
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPSeqset seqset = NULL;
    AjPStr    cl     = NULL;
    AjPSeqout seqout = NULL;

    AjBool    full   = ajFalse;

    AjPStr    ofn    = NULL;
    AjPStr    fn     = NULL;
    AjPStr    stmp   = NULL;
    
    AjPFile outf = NULL;
    
    
    embInitP("echlorop", argc, argv, "CBSTOOLS");


    seqset  = ajAcdGetSeqset("sequence");
    outf    = ajAcdGetOutfile("outfile");
    full    = ajAcdGetBoolean("full");
    
    cl   = ajStrNewC("chlorop ");
    fn   = ajStrNew();
    stmp = ajStrNew();
    


    ajStrAssignC(&fn, ajFileTempName(NULL));
    seqout = ajSeqoutNew();
    if(!ajSeqoutOpenFilename(seqout, fn))
	ajFatal("Cannot open temporary file %S",fn);
    ajSeqoutSetFormatC(seqout, "fasta");
    ajSeqoutWriteSet(seqout,seqset);
    ajSeqoutClose(seqout);

    if(full)
        ajStrAppendC(&cl," -F");

    ajFmtPrintS(&stmp," %S",fn);
    ajStrAppendS(&cl,stmp);

    ofn = ajFileGetName(outf);
    ajFmtPrintS(&stmp," > %S",ofn);
    ajStrAppendS(&cl,stmp);
    ajFileClose(&outf);

#if 0
   ajFmtPrint("%S\n",cl);
#endif

#if 1
   if(system(ajStrGetPtr(cl)) == -1)
       ajFatal("Command %S failed",cl);
#endif

    ajSysFileUnlink(fn);

    ajStrDel(&cl);
    ajStrDel(&fn);
    ajStrDel(&stmp);
    ajSeqoutDel(&seqout);
    ajSeqsetDel(&seqset);

    embExit();

    return 0;
}
