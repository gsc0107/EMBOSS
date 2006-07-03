/* @source preg application
**
** Protein regular expression (perl style)
**
** @author Copyright (C) Peter Rice
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




/* @prog preg *****************************************************************
**
** Regular expression search of a protein sequence
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPSeqall seqall;
    AjPRegexp patexp;
    AjPReport report;
    AjPFeattable feat=NULL;
    AjPFeature sf = NULL;
    AjPSeq seq = NULL;
    AjPStr str = NULL;
    AjPStr tmpstr = NULL;
    AjPStr substr = NULL;
    ajint ioff;
    ajint ipos;
    ajint ilen;

    embInit("preg", argc, argv);

    report = ajAcdGetReport("outfile");
    seqall = ajAcdGetSeqall("sequence");
    patexp = ajAcdGetRegexpSingle("pattern");

    ajFmtPrintAppS (&tmpstr, "Pattern: %S\n", ajAcdGetValue("pattern"));
    ajReportSetHeader (report, tmpstr);

    while(ajSeqallNext(seqall, &seq))
    {
	ipos = 1;
	ajStrAssignS(&str, ajSeqGetSeqS(seq));
	ajStrFmtUpper(&str);
	ajDebug("Testing '%s' len: %d %d\n",
		ajSeqGetNameC(seq), ajSeqGetLen(seq), ajStrGetLen(str));
        feat = ajFeattableNewProt(ajSeqGetNameS(seq));

	while(ajStrGetLen(str) && ajRegExec(patexp, str))
	{
	    ioff = ajRegOffset(patexp);
	    ilen = ajRegLenI(patexp, 0);
	    if(ioff || ilen)
	    {
		ajRegSubI(patexp, 0, &substr);
		ajRegPost(patexp, &tmpstr);
		ajStrAssignS(&str, tmpstr);
		ipos += ioff;
		sf = ajFeatNewII (feat,ipos,ipos+ilen-1);
		ipos += ilen;
	    }
	    else
	    {
		ipos++;
		ajStrCutStart(&str, 1);
	    }
	}
        (void) ajReportWrite (report,feat,seq);
        ajFeattableDel(&feat);
    }

    ajReportClose(report);
    ajReportDel(&report);

    ajSeqallDel(&seqall);
    ajSeqDel(&seq);
    ajRegFree(&patexp);

    ajStrDel(&str);
    ajStrDel(&tmpstr);
    ajStrDel(&substr);

    embExit();

    return 0;
}
