/******************************************************************************
** @source AJAX REG (ajax regular expression) functions
**
** Uses Henry Spencer's older regular expression library at present.
** The newer version appears to have some performance problems,
** especially in extracting substrings which we may need to use
** quite extensively.
**
** A major reason for adding this layer is to allow a future
** near-transparent switch to the POSIX version of the Henry Spencer
** library at some future date.
**
** Some extra tricks are added, such as simply returning the matched
** string or the nth substring.
**
** Possible extensions include a case-insensitive regcomp ... which
** would be useful in many places. Also a regcomp for prefices,
** suffices and exact matches which adds "^" and "$" to the pattern
** string.
**
** @author Copyright (C) 1998 Peter Rice
** @version 1.0
** @modified Jun 25 pmr First version
** @@
** @@
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public
** License along with this library; if not, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
******************************************************************************/

#include "ajax.h"
#include "ajhsreg.h"

static ajlong regAlloc = 0;
static ajlong regFree = 0;
static ajlong regFreeCount = 0;
static ajlong regCount = 0;
static ajlong regTotal = 0;

/* constructors */

/* @func ajHsRegComp **********************************************************
**
** Compiles a regular expression.
**
** @param [r] exp [AjPStr] Regular expression string.
** @return [AjPHsRegexp] Compiled regular expression.
** @@
******************************************************************************/

AjPHsRegexp ajHsRegComp(AjPStr exp)
{
    AjPHsRegexp ret;

    ret = hsregcomp(ajStrStr(exp));
    if(ret)
    {
	regAlloc += sizeof(ret);
	regCount ++;
	regTotal ++;
	ajDebug("ajHsRegComp %x size %d regexp '%S'\n",
		ret, (int) sizeof(ret), exp);
    }

    return ret;
}




/* @func ajHsRegCompC *********************************************************
**
** Compiles a regular expression.
**
** @param [r] exp [const char*] Regular expression character string.
** @return [AjPHsRegexp] Compiled regular expression.
** @@
******************************************************************************/

AjPHsRegexp ajHsRegCompC(const char* exp)
{
    return hsregcomp(exp);
}




/* execute expression match */

/* @func ajHsRegExec **********************************************************
**
** Execute a regular expression search.
** The expression must first have been compiled with ajHsRegComp or
** ajHsRegCompC.
**
** Internal data structures in the expression will be set to substrings
** which other functions can retrieve.
**
** @param [u] prog [AjPHsRegexp] Compiled regular expression.
** @param [r] str [AjPStr] String to be compared.
** @return [AjBool] ajTrue if a match was found.
** @@
******************************************************************************/

AjBool ajHsRegExec(AjPHsRegexp prog, AjPStr str)
{

    if(hsregexec(prog, ajStrStr(str)))
    {
	prog->orig = ajStrStr(str);
	return ajTrue;
    }

    prog->orig = NULL;

    return ajFalse;
}




/* @func ajHsRegExecC *********************************************************
**
** Execute a regular expression search.
** The expression must first have been compiled with ajHsRegComp or
** ajHsRegCompC.
**
** Internal data structures in the expression will be set to substrings
** which other functions can retrieve.
**
** @param [u] prog [AjPHsRegexp] Compiled regular expression.
** @param [r] str [const char*] String to be compared.
** @return [AjBool] ajTrue if a match was found.
** @@
******************************************************************************/

AjBool ajHsRegExecC(AjPHsRegexp prog, const char* str)
{

    if(hsregexec(prog, str))
    {
	prog->orig = str ;
	return ajTrue;
    }

    prog->orig = NULL;

    return ajFalse;
}




/* @func ajHsRegOffset ********************************************************
**
** After a successful regular expression match, uses the regular
** expression and the original string to calculate the offset
** of the match from the start of the string.
**
** This information is normally lost during processing.
**
** @param [r] rp [AjPHsRegexp] Compiled regular expression.
** @return [ajint] Offset of match from start of string.
**               -1 if the string and the expression do not match.
** @@
******************************************************************************/

ajint ajHsRegOffset(AjPHsRegexp rp)
{
    return (rp->startp[0] - rp->orig);
}




/* @func ajHsRegOffsetI *******************************************************
**
** After a successful regular expression match, uses the regular
** expression and the original string to calculate the offset
** of a substring from the start of the string.
**
** This information is normally lost during processing.
**
** @param [r] rp [AjPHsRegexp] Compiled regular expression.
** @param [r] isub [ajint] Substring number.
** @return [ajint] Offset of match from start of string.
**               -1 if the string and the expression do not match.
** @@
******************************************************************************/

ajint ajHsRegOffsetI(AjPHsRegexp rp, ajint isub)
{
    return (rp->startp[isub] - rp->orig);
}




/* @func ajHsRegOffsetC *******************************************************
**
** After a successful regular expression match, uses the regular
** expression and the original character string to calculate the offset
** of the match from the start of the string.
**
** This information is normally lost during processing.
**
** @param [r] rp [AjPHsRegexp] Compiled regular expression.
** @return [ajint] Offset of match from start of string.
**               -1 if the string and the expression do not match.
** @@
******************************************************************************/

ajint ajHsRegOffsetC(AjPHsRegexp rp)
{
    return (rp->startp[0] - rp->orig);
}




/* @func ajHsRegOffsetIC ******************************************************
**
** After a successful regular expression match, uses the regular
** expression and the original string to calculate the offset
** of a substring from the start of the string.
**
** This information is normally lost during processing.
**
** @param [r] rp [AjPHsRegexp] Compiled regular expression.
** @param [r] isub [ajint] Substring number.
** @return [ajint] Offset of match from start of string.
**               -1 if the string and the expression do not match.
** @@
******************************************************************************/

ajint ajHsRegOffsetIC(AjPHsRegexp rp, ajint isub)
{
    return (rp->startp[isub] - rp->orig);
}




/* @func ajHsRegLenI **********************************************************
**
** After a successful comparison, returns the length of a substring.
**
** @param [r] rp [AjPHsRegexp] Compiled regular expression.
** @param [r] isub [ajint] Substring number.
** @return [ajint] Substring length, or 0 if not found.
** @@
******************************************************************************/

ajint ajHsRegLenI(AjPHsRegexp rp, ajint isub)
{

    if(!rp->startp[isub])
	return 0;

    return (rp->endp[isub] - rp->startp[isub]);
}




/* @func ajHsRegPost **********************************************************
**
** After a successful match, returns the remainder of the string.
**
** @param [r] rp [AjPHsRegexp] Compiled regular expression.
** @param [w] post [AjPStr*] String to hold the result.
** @return [AjBool] ajTrue on success.
** @@
******************************************************************************/

AjBool ajHsRegPost(AjPHsRegexp rp, AjPStr* post)
{

    if(*rp->endp[0])
    {
	ajStrAssC(post, rp->endp[0]);
	return ajTrue;
    }

    ajStrDelReuse(post);

    return ajFalse;
}




/* @func ajHsRegPostC *********************************************************
**
** After a successful match, returns the remainder of the string.
** Result is a character string, which is set to point to the internal
** string data. This in turn is poart of the original string. If this
** changes then the results are undefined.
**
** @param [r] rp [AjPHsRegexp] Compiled regular expression.
** @param [w] post [const char**] Character string to hold the result.
** @return [AjBool] ajTrue on success.
** @@
******************************************************************************/

AjBool ajHsRegPostC(AjPHsRegexp rp, const char** post)
{
    if(*rp->endp[0])
    {
	*post = rp->endp[0];
	return ajTrue;
    }

    *post = 0;

    return ajFalse;
}




/* @func ajHsRegSubI **********************************************************
**
** After a successful match, returns a substring.
**
** @param [r] rp [AjPHsRegexp] Compiled regular expression.
** @param [r] isub [ajint] Substring number.
** @param [w] dest [AjPStr*] String to hold the result.
** @return [void]
** @@
******************************************************************************/

void ajHsRegSubI(AjPHsRegexp rp, ajint isub, AjPStr* dest)
{
    ajint ilen;

    if(!rp->startp[isub])
    {
	ajStrDelReuse(dest);
	return;
    }

    if(isub >= NSUBEXP)
    {
	ajStrDelReuse(dest);
	return;
    }
    ilen = rp->endp[isub] - rp->startp[isub];
    ajStrModL(dest, ilen+1);
    if(ilen)
	strncpy((*dest)->Ptr, rp->startp[isub], ilen);
    (*dest)->Len = ilen;
    (*dest)->Ptr[ilen] = '\0';

    return;
}




/* substitute substrings */

/* @func ajHsRegSub ***********************************************************
**
** Processes a source string including possible substring references
** escaped with backslashes. Results are written to another string.
**
** @param [r] rp [AjPHsRegexp] Compiled regular expression.
** @param [r] source [AjPStr] Source string.
** @param [w] dest [AjPStr*] Destination string.
** @return [void]
** @@
******************************************************************************/

void ajHsRegSub(AjPHsRegexp rp, AjPStr source, AjPStr* dest)
{
    ajHsRegSubC(rp, ajStrStr(source), dest);

    return;
}




/* @func ajHsRegSubC **********************************************************
**
** Processes a source string including possible substring references
** escaped with backslashes. Results are written to another string.
**
** @param [r] rp [AjPHsRegexp] Compiled regular expression.
** @param [r] source [const char*] Source string.
** @param [w] dest [AjPStr*] Destination string.
** @return [void]
** @@
******************************************************************************/

void ajHsRegSubC(AjPHsRegexp rp, const char* source, AjPStr* dest)
{
    register regexp * const prog;
    register char *src;
    register char *dst;
    register char c;
    register ajint no;
    register size_t len;
    register size_t dstfree;

    prog = (regexp *)rp;
    src = (char *)source;

    ajStrMod(dest);
    dst = (*dest)->Ptr;
    dstfree = (*dest)->Res - 1;

    if(prog == NULL || source == NULL)
    {
	hsregerror("NULL parameter to regsub");
	return;
    }

    if((ajuint)((unsigned char)*(prog->program)) != MAGIC)
    {
	hsregerror("damaged regexp");
	return;
    }

    while((c = *src++) != '\0')
    {
	if(c == '&')
	    no = 0;
	else if(c == '\\' && isdigit((ajint)*src))
	    no = *src++ - '0';
	else
	    no = -1;

	if(no < 0)
	{
	    /* Ordinary character. */
	    if(c == '\\' && (*src == '\\' || *src == '&'))
		c = *src++;
	    if(!dstfree)
	    {
		dstfree += (*dest)->Res;
		(*dest)->Len = (*dest)->Res - 1;
		(*dest)->Ptr[(*dest)->Len] = '\0';
		ajStrModL(dest, (*dest)->Res*2);
		dst = &(*dest)->Ptr[(*dest)->Len];
		ajStrTrace(*dest);
	    }
	    *dst++ = c;
	    dstfree--;
	}
	else if(prog->startp[no] != NULL && prog->endp[no] != NULL &&
		prog->endp[no] > prog->startp[no])
	{
	    len = prog->endp[no] - prog->startp[no];
	    if(dstfree < len)
	    {
		(*dest)->Len = (*dest)->Res - dstfree - 1;
		(*dest)->Ptr[(*dest)->Len] = '\0';
		if(len > (*dest)->Res)
		{
		    ajStrModL(dest, (*dest)->Res+len);
		    dstfree += len;
		    ajStrTrace(*dest);
		}
		else
		{
		    dstfree += (*dest)->Res;
		    ajStrModL(dest, (*dest)->Res*2);
		    ajStrTrace(*dest);
		}
		dst = &(*dest)->Ptr[(*dest)->Len];
	    }
	    strncpy(dst, prog->startp[no], len);
	    dst += len;
	    dstfree -= len;
	    if(*(dst-1) == '\0')
	    {
		/* strncpy hit NUL. */
		hsregerror("damaged match string");
		return;
	    }
	}
    }
    *dst++ = '\0';
    (*dest)->Len = (*dest)->Res - dstfree - 1;

    return;
}




/* destructor */

/* @func ajHsRegFree **********************************************************
**
** Clears and frees a compiled regular expression.
**
** @param [w] pexp [AjPHsRegexp*] Compiled regular expression.
** @return [void]
** @@
******************************************************************************/

void ajHsRegFree(AjPHsRegexp* pexp)
{
    if(!pexp)
	return;
    if(!*pexp)
	return;
    ajDebug("ajHsRegFree %x size regexp %d\n", *pexp,
	    (ajint) sizeof(*pexp));
    regFreeCount += 1;
    regFree += sizeof(**pexp);
    regTotal --;
    hsregfree(pexp);
    *pexp = NULL;

    return;
}




/* @func ajHsRegTrace *********************************************************
**
** Traces a compiled regular expression with debug calls.
**
** @param [r] exp [AjPHsRegexp] Compiled regular expression.
** @return [void]
** @@
******************************************************************************/

void ajHsRegTrace(AjPHsRegexp exp)
{
    ajint isub;
    ajint ilen;
    ajint ipos;
    static AjPStr str = NULL;

    ajDebug("  REGEXP trace\n");
    for(isub=0; isub < NSUBEXP; isub++)
    {
	if(exp->startp[isub])
	{
	    ilen = exp->endp[isub] - exp->startp[isub];
	    ajStrModL(&str, ilen+1);
	    strncpy(str->Ptr, exp->startp[isub], ilen);
	    str->Len = ilen;
	    str->Ptr[ilen] = '\0';
	    if(!isub)
	    {
		ajDebug(" original string '%s'\n", exp->orig);
		ajDebug("    string match '%S'\n", str);
	    }
	    else
	    {
		ipos = exp->startp[isub] - exp->startp[0];
		ajDebug("    substring %2d '%S' at %d\n", isub, str, ipos);
	    }
	}
    }
    ajDebug("\n");
    ajStrDelReuse(&str);

    return;
}




/* @func ajHsRegExit **********************************************************
**
** Prints a summary of regular expression (AjPHsRegexp) usage with debug calls
**
** @return [void]
** @@
******************************************************************************/

void ajHsRegExit(void)
{

    ajDebug("Regexp usage (bytes): %ld allocated, %ld freed, %ld in use\n",
	    regAlloc, regFree, regAlloc - regFree);
    ajDebug("Regexp usage (number): %ld allocated, %ld freed %ld in use\n",
	    regTotal, regFreeCount, regCount);

    return;
}
