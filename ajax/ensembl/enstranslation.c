/******************************************************************************
** @source Ensembl Translation functions.
**
** @author Copyright (C) 1999 Ensembl Developers
** @author Copyright (C) 2006 Michael K. Schuster
** @modified 2009 by Alan Bleasby for incorporation into EMBOSS core
** @version $Revision: 1.6 $
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

/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */

#include "enstranslation.h"
#include "ensdatabaseentry.h"
#include "ensexon.h"
#include "enssequenceedit.h"
#include "enstranscript.h"




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */

static const char *translationProteinfeatureDomainName[] =
{
    "pfscan",
    "scanprosite",
    "superfamily",
    "pfam",
    "smart",
    "tigrfam",
    "pirsf",
    "prints",
    NULL
};




/******************************************************************************
 **
 ** Ensembl Sequence Edits for Ensembl Translations are a sub-set of
 ** Ensembl Attributes that provide information about post-translational
 ** modifications of the Translation sequence. Attributes with the following
 ** codes are Sequence Edits on the Translation-level.
 **
 ** initial_met:     Set the first amino acid to methionine
 ** _selenocysteine: Start and end position of a selenocysteine
 ** amino_acid_sub:  Amino acid substitution mainly for FlyBase
 **
 ******************************************************************************/

static const char *translationSequenceEditCode[] =
{
    "initial_met",
    "_selenocysteine",
    "amino_acid_sub",
    NULL
};

static AjPTable translationCache = NULL;




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

extern EnsPDatabaseentryadaptor
ensRegistryGetDatabaseentryadaptor(EnsPDatabaseadaptor dba);

extern EnsPProteinfeatureadaptor
ensRegistryGetProteinfeatureadaptor(EnsPDatabaseadaptor dba);

extern EnsPTranscriptadaptor
ensRegistryGetTranscriptadaptor(EnsPDatabaseadaptor dba);

extern EnsPTranslationadaptor
ensRegistryGetTranslationadaptor(EnsPDatabaseadaptor dba);

static void translationCacheClear(void **key, void **value, void *cl);

static AjBool translationAdaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                              const AjPStr statement,
                                              EnsPAssemblymapper am,
                                              EnsPSlice slice,
                                              AjPList translations);

#if AJFALSE
static void *translationAdaptorCacheReference(void *value);

static void translationAdaptorCacheDelete(void **value);

static ajuint translationAdaptorCacheSize(const void *value);
#endif

static AjBool translationAdaptorFetchAllByIdentifiers(
    const EnsPTranslationadaptor tla,
    AjPStr identifiers,
    AjPList translations);




/* @filesection enstranslation ************************************************
**
** @nam1rule ens Function belongs to the Ensembl library
**
******************************************************************************/




/* @func ensTranslationInit ***************************************************
**
** Initialises the Ensembl Translation.
**
** @return [void]
** @@
******************************************************************************/

void ensTranslationInit(void)
{
    translationCache = MENSTABLEUINTNEW(0);

    return;
}




/* @funcstatic translationCacheClear ******************************************
**
** An ajTableMapDel 'apply' function to clear the AJAX Translation cache.
** This function deletes the AJAX unsigned integer key data and the
** AJAX Translation value data.
**
** @param [u] key [void**] AJAX unsigned integer key data address
** @param [u] value [void**] AJAX Translation value data address
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void translationCacheClear(void **key, void **value, void *cl)
{
    if(!key)
        return;

    if(!*key)
        return;

    if(!value)
        return;

    if(!*value)
        return;

    (void) cl;

    AJFREE(*key);

    ajTrnDel((AjPTrn *) value);

    return;
}




/* @func ensTranslationExit ***************************************************
**
** Frees the Ensembl Translation.
**
** @return [void]
** @@
******************************************************************************/

void ensTranslationExit(void)
{
    ajTableMapDel(translationCache, translationCacheClear, NULL);

    ajTableFree(&translationCache);

    return;
}




/* @func ensTranslationCacheGetTranslation ************************************
**
** Get an AJAX Translation from the internal Translation cache.
**
** AJAX Translation objects require a data file to be read upon each creation.
** To avoid file system operations AJAX Translation objects are kept in a
** cache and can be used directly. Calling ensTranslationExit will free all
** AJAX Translation objects from the cache.
**
** @param [r] codontable [ajint] Codon table number
**
** @return [AjPTrn] AJAX Translation or NULL
** @@
******************************************************************************/

AjPTrn ensTranslationCacheGetTranslation(ajint codontable)
{
    ajint *Pcodontable = NULL;

    AjPTrn translation = NULL;

    translation = (AjPTrn)
        ajTableFetch(translationCache, (const void *) &codontable);

    if(translation)
        return translation;

    translation = ajTrnNewI(codontable);

    if(translation)
    {
        AJNEW0(Pcodontable);

        *Pcodontable = codontable;

        ajTablePut(translationCache,
                   (void *) Pcodontable,
                   (void *) translation);
    }

    return translation;
}




/* @datasection [EnsPTranslation] Translation *********************************
**
** Functions for manipulating Ensembl Translation objects
**
** @cc Bio::EnsEMBL::Translation CVS Revision: 1.67
**
** @nam2rule Translation
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Translation by pointer.
** It is the responsibility of the user to first destroy any previous
** Translation. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPTranslation]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPTranslation] Ensembl Translation
** @argrule Ref object [EnsPTranslation] Ensembl Translation
**
** @valrule * [EnsPTranslation] Ensembl Translation
**
** @fcategory new
******************************************************************************/




/* @func ensTranslationNew ****************************************************
**
** Default Ensembl Translation constructor.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] tla [EnsPTranslationadaptor] Ensembl Translation Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Translation::new
** @param [u] transcript [EnsPTranscript] Ensembl Transcript
** @param [u] startexon [EnsPExon] Ensembl Exon in which the start coordinate
**                                 is annotated
** @param [u] endexon [EnsPExon] Ensembl Exon in which the end coordinate
**                               is annotated
** @param [r] start [ajuint] Start coordinate relative to the start Exon
** @param [r] end [ajuint] Start coordinate relative to the end Exon
** @param [u] sequence [AjPStr] Translation sequence
** @param [u] stableid [AjPStr] Stable identifier
** @param [r] version [ajuint] Version
** @param [u] cdate [AjPStr] Creation date
** @param [u] mdate [AjPStr] Modification date
**
** @return [EnsPTranslation] Ensembl Translation or NULL
** @@
******************************************************************************/

EnsPTranslation ensTranslationNew(EnsPTranslationadaptor tla,
                                  ajuint identifier,
                                  EnsPTranscript transcript,
                                  EnsPExon startexon,
                                  EnsPExon endexon,
                                  ajuint start,
                                  ajuint end,
                                  AjPStr sequence,
                                  AjPStr stableid,
                                  ajuint version,
                                  AjPStr cdate,
                                  AjPStr mdate)
{
    EnsPTranslation translation = NULL;

    if(!transcript)
        return NULL;

    if(!startexon)
        return NULL;

    if(!endexon)
        return NULL;

    AJNEW0(translation);

    translation->Use = 1;

    translation->Identifier = identifier;

    translation->Adaptor = tla;

    translation->Transcript = ensTranscriptNewRef(transcript);

    translation->StartExon = ensExonNewRef(startexon);

    translation->EndExon = ensExonNewRef(endexon);

    translation->Start = start;

    translation->End = end;

    if(stableid)
        translation->StableIdentifier = ajStrNewRef(stableid);

    if(cdate)
        translation->CreationDate = ajStrNewRef(cdate);

    if(mdate)
        translation->ModificationDate = ajStrNewRef(mdate);

    translation->Version = version;

    translation->Attributes = NULL;

    translation->DatabaseEntries = NULL;

    translation->Proteinfeatures = NULL;

    if(sequence)
        translation->Sequence = ajStrNewRef(sequence);

    translation->TranscriptStart = 0;

    translation->TranscriptEnd = 0;

    translation->SliceStart = 0;

    translation->SliceEnd = 0;

    /* Link a weak reference from the Transcript to this Translation. */

    ensTranscriptLinkTranslation(transcript, translation);

    return translation;
}




/* @func ensTranslationNewObj *************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPTranslation] Ensembl Translation
**
** @return [EnsPTranslation] Ensembl Translation or NULL
** @@
******************************************************************************/

EnsPTranslation ensTranslationNewObj(const EnsPTranslation object)
{
    AjIList iter = NULL;

    EnsPAttribute attribute = NULL;

    EnsPDatabaseentry dbe = NULL;

    EnsPProteinfeature pf = NULL;

    EnsPTranslation translation = NULL;

    if(!object)
        return NULL;

    AJNEW0(translation);

    translation->Use = 1;

    translation->Identifier = object->Identifier;

    translation->Adaptor = object->Adaptor;

    translation->Transcript = ensTranscriptNewRef(object->Transcript);

    translation->StartExon = ensExonNewRef(object->StartExon);

    translation->EndExon = ensExonNewRef(object->EndExon);

    translation->Start = object->Start;

    translation->End = object->End;

    if(object->CreationDate)
        translation->CreationDate = ajStrNewRef(object->CreationDate);

    if(object->ModificationDate)
        translation->ModificationDate = ajStrNewRef(object->ModificationDate);

    if(object->StableIdentifier)
        translation->StableIdentifier = ajStrNewRef(object->StableIdentifier);

    translation->Version = object->Version;

    /* Copy the List of Ensembl Attributes. */

    if(object->Attributes && ajListGetLength(object->Attributes))
    {
        translation->Attributes = ajListNew();

        iter = ajListIterNew(object->Attributes);

        while(!ajListIterDone(iter))
        {
            attribute = (EnsPAttribute) ajListIterGet(iter);

            ajListPushAppend(translation->Attributes,
                             (void *) ensAttributeNewRef(attribute));
        }

        ajListIterDel(&iter);
    }
    else
        translation->Attributes = NULL;

    /* Copy the List of Ensembl Database Entries. */

    if(object->DatabaseEntries && ajListGetLength(object->DatabaseEntries))
    {
        translation->DatabaseEntries = ajListNew();

        iter = ajListIterNew(object->DatabaseEntries);

        while(!ajListIterDone(iter))
        {
            dbe = (EnsPDatabaseentry) ajListIterGet(iter);

            ajListPushAppend(translation->DatabaseEntries,
                             (void *) ensDatabaseentryNewRef(dbe));
        }

        ajListIterDel(&iter);
    }
    else
        translation->DatabaseEntries = NULL;

    /* Copy the List of Ensembl Protein Features. */

    if(object->Proteinfeatures && ajListGetLength(object->Proteinfeatures))
    {
        translation->Proteinfeatures = ajListNew();

        iter = ajListIterNew(object->Proteinfeatures);

        while(!ajListIterDone(iter))
        {
            pf = (EnsPProteinfeature) ajListIterGet(iter);

            ajListPushAppend(translation->Proteinfeatures,
                             (void *) ensProteinfeatureNewRef(pf));
        }

        ajListIterDel(&iter);
    }
    else
        translation->Proteinfeatures = NULL;

    if(object->Sequence)
        translation->Sequence = ajStrNewRef(object->Sequence);

    translation->TranscriptStart = object->TranscriptStart;

    translation->TranscriptEnd = object->TranscriptEnd;

    translation->SliceStart = object->SliceStart;

    translation->SliceEnd = object->SliceEnd;

    return translation;
}




/* @func ensTranslationNewRef *************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] translation [EnsPTranslation] Ensembl Translation
**
** @return [EnsPTranslation] Ensembl Translation or NULL
** @@
******************************************************************************/

EnsPTranslation ensTranslationNewRef(EnsPTranslation translation)
{
    if(!translation)
        return NULL;

    translation->Use++;

    return translation;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Translation.
**
** @fdata [EnsPTranslation]
** @fnote None
**
** @nam3rule Del Destroy (free) a Translation object
**
** @argrule * Ptranslation [EnsPTranslation*] Translation object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensTranslationDel ****************************************************
**
** Default destructor for an Ensembl Translation.
**
** @param [d] Ptranslation [EnsPTranslation*] Ensembl Translation address
**
** @return [void]
** @@
******************************************************************************/

void ensTranslationDel(EnsPTranslation *Ptranslation)
{
    EnsPTranslation pthis = NULL;

    EnsPAttribute attribute = NULL;

    EnsPDatabaseentry dbe = NULL;

    EnsPProteinfeature pf = NULL;

    if(!Ptranslation)
        return;

    if(!*Ptranslation)
        return;

    pthis = *Ptranslation;

    pthis->Use--;

    if(pthis->Use)
    {
        *Ptranslation = NULL;

        return;
    }

    /* Unlink the weak reference from the Transcript to this Translation. */

    ensTranscriptUnlinkTranslation(pthis->Transcript, *Ptranslation);

    ensTranscriptDel(&pthis->Transcript);

    ensExonDel(&pthis->StartExon);
    ensExonDel(&pthis->EndExon);

    ajStrDel(&pthis->StableIdentifier);
    ajStrDel(&pthis->CreationDate);
    ajStrDel(&pthis->ModificationDate);

    /* Clear and delete the AJAX List of Ensembl Attributes. */

    while(ajListPop(pthis->Attributes, (void **) &attribute))
        ensAttributeDel(&attribute);

    ajListFree(&pthis->Attributes);

    /* Clear and delete the AJAX List of Ensembl Database Entries. */

    while(ajListPop((*Ptranslation)->DatabaseEntries, (void **) &dbe))
        ensDatabaseentryDel(&dbe);

    ajListFree(&pthis->DatabaseEntries);

    /* Clear and delete the AJAX List of Ensembl Protein Features. */

    while(ajListPop(pthis->Proteinfeatures, (void **) &pf))
        ensProteinfeatureDel(&pf);

    ajListFree(&pthis->Proteinfeatures);

    ajStrDel(&pthis->Sequence);

    AJFREE(pthis);

    *Ptranslation = NULL;

    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Translation object.
**
** @fdata [EnsPTranslation]
** @fnote None
**
** @nam3rule Get Return Translation attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Translation Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetTranscript Return the Transcript
** @nam4rule GetStartExon Return the start Exon
** @nam4rule GetEndExon Return the end Exon
** @nam4rule GetStart Return the start coordinate
** @nam4rule GetEnd Return the end coordinate
** @nam4rule GetStableIdentifier Return the stable identifier
** @nam4rule GetVersion Return the version
** @nam4rule GetCreationDate Return the creation date
** @nam4rule GetModificationDate Return the modification date
** @nam4rule GetAttributes Return all Ensembl Attributes
** @nam4rule GetDatabaseEntries Return all Ensembl Database Entries
** @nam4rule GetProteinfeatures Return all Ensembl Protein Features
**
** @argrule * translation [const EnsPTranslation] Translation
**
** @valrule Adaptor [EnsPTranslationadaptor] Ensembl Translation Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Transcript [EnsPTranscript] Ensembl Transcript
** @valrule StartExon [EnsPExon] Start Exon
** @valrule EndExon [EnsPExon] End Exon
** @valrule Start [ajuint] Start coordinate
** @valrule End [ajuint] End coordinate
** @valrule StableIdentifier [AjPStr] Stable identifier
** @valrule Version [ajuint] Version
** @valrule CreationDate [AjPStr] Creation date
** @valrule ModificationDate [AjPStr] Creation date
** @valrule GetAttributes [const AjPList] AJAX List of Ensembl Attributes
** @valrule GetDatabaseEntries [const AjPList] AJAX List of
**                                             Ensembl Database Entries
** @valrule GetProteinfeatures [const AjPList] AJAX List of
**                                             Ensembl Protein Features
**
** @fcategory use
******************************************************************************/




/* @func ensTranslationGetAdaptor *********************************************
**
** Get the Ensembl Translation Adaptor element of an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [r] translation [const EnsPTranslation] Ensembl Translation
**
** @return [const EnsPTranslationadaptor] Ensembl Translation Adaptor or NULL
** @@
******************************************************************************/

const EnsPTranslationadaptor ensTranslationGetAdaptor(
    const EnsPTranslation translation)
{
    if(!translation)
        return NULL;

    return translation->Adaptor;
}




/* @func ensTranslationGetIdentifier ******************************************
**
** Get the SQL database-internal identifier element of an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [r] translation [const EnsPTranslation] Ensembl Translation
**
** @return [ajuint] SQL database-internal identifier
** @@
******************************************************************************/

ajuint ensTranslationGetIdentifier(const EnsPTranslation translation)
{
    if(!translation)
        return 0;

    return translation->Identifier;
}




/* @func ensTranslationGetTranscript ******************************************
**
** Get the Ensembl Transcript of an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Translation::???
** @param [r] translation [const EnsPTranslation] Ensembl Translation
**
** @return [EnsPTranscript] Ensembl Transcript or NULL
** @@
******************************************************************************/

EnsPTranscript ensTranslationGetTranscript(const EnsPTranslation translation)
{
    if(!translation)
        return 0;

    return translation->Transcript;
}




/* @func ensTranslationGetStartExon *******************************************
**
** Get the start Ensembl Exon element of an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Translation::start_Exon
** @param [r] translation [const EnsPTranslation] Ensembl Translation
**
** @return [EnsPExon] Start Ensembl Exon
** @@
******************************************************************************/

EnsPExon ensTranslationGetStartExon(const EnsPTranslation translation)
{
    if(!translation)
        return 0;

    return translation->StartExon;
}




/* @func ensTranslationGetEndExon *********************************************
**
** Get the end Ensembl Exon element of an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Translation::end_Exon
** @param [r] translation [const EnsPTranslation] Ensembl Translation
**
** @return [EnsPExon] End Ensembl Exon
** @@
******************************************************************************/

EnsPExon ensTranslationGetEndExon(const EnsPTranslation translation)
{
    if(!translation)
        return 0;

    return translation->EndExon;
}




/* @func ensTranslationGetStart ***********************************************
**
** Get the start element of an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Translation::start
** @param [r] translation [const EnsPTranslation] Ensembl Translation
**
** @return [ajuint] Translation start coodinate, which is a position within
**                  the start Exon element
** @@
******************************************************************************/

ajuint ensTranslationGetStart(const EnsPTranslation translation)
{
    if(!translation)
        return 0;

    return translation->Start;
}




/* @func ensTranslationGetEnd *************************************************
**
** Get the end element of an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Translation::end
** @param [r] translation [const EnsPTranslation] Ensembl Translation
**
** @return [ajuint] Translation end coodinate, which is a position within
**                  the end Exon element
** @@
******************************************************************************/

ajuint ensTranslationGetEnd(const EnsPTranslation translation)
{
    if(!translation)
        return 0;

    return translation->End;
}




/* @func ensTranscriptGetStableIdentifier *************************************
**
** Get the stable identifier element of an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Translation::stable_id
** @param [r] transcript [const EnsPTranslation] Ensembl Translation
**
** @return [AjPStr] Stable identifier
** @@
******************************************************************************/

AjPStr ensTranslationGetStableIdentifier(const EnsPTranslation translation)
{
    if(!translation)
        return NULL;

    return translation->StableIdentifier;
}




/* @func ensTranscriptGetVersion **********************************************
**
** Get the version element of an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Translation::version
** @param [r] translation [const EnsPTranslation] Ensembl Translation
**
** @return [ajuint] Version
** @@
******************************************************************************/

ajuint ensTranslationGetVersion(const EnsPTranslation translation)
{
    if(!translation)
        return 0;

    return translation->Version;
}




/* @func ensTranslationGetCreationDate ****************************************
**
** Get the creation date element of an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Translation::created_date
** @param [r] translation [const EnsPTranslation] Ensembl Translation
**
** @return [AjPStr] Creation date
** @@
******************************************************************************/

AjPStr ensTranslationGetCreationDate(const EnsPTranslation translation)
{
    if(!translation)
        return NULL;

    return translation->CreationDate;
}




/* @func ensTranslationGetModificationDate ************************************
**
** Get the modification date element of an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Translation::modified_date
** @param [r] translation [const EnsPTranslation] Ensembl Translation
**
** @return [AjPStr] Modification date
** @@
******************************************************************************/

AjPStr ensTranslationGetModificationDate(const EnsPTranslation translation)
{
    if(!translation)
        return NULL;

    return translation->ModificationDate;
}




/* @func ensTranslationGetAttributes ******************************************
**
** Get all Ensembl Attributes of an Ensembl Translation.
**
** This is not a simple accessor function, it will fetch Ensembl Attributes
** from the Ensembl Core database in case the AJAX List is empty.
**
** @cc Bio::EnsEMBL::Translation::get_all_Attributes
** @param [u] translation [EnsPTranslation] Ensembl Translation
**
** @return [const AjPList] AJAX List of Ensembl Attributes
** @@
******************************************************************************/

const AjPList ensTranslationGetAttributes(EnsPTranslation translation)
{
    EnsPDatabaseadaptor dba = NULL;

    if(ajDebugTest("ensTranslationGetAttributes"))
        ajDebug("ensTranslationGetAttributes\n"
                "  translation %p\n",
                translation);

    if(!translation)
        return NULL;

    if(translation->Attributes)
        return translation->Attributes;
    else
        translation->Attributes = ajListNew();

    if(!translation->Adaptor)
    {
        ajDebug("ensTranslationGetAttributes cannot fetch "
                "Ensembl Attributes for a Translation without a "
                "Translation Adaptor.\n");

        return NULL;
    }

    dba = ensTranslationadaptorGetDatabaseadaptor(translation->Adaptor);

    if(!dba)
    {
        ajDebug("ensTranslationGetAttributes cannot fetch "
                "Ensembl Attributes for a Translation without a "
                "Database Adaptor set in the Translation Adaptor.\n");

        return NULL;
    }

    ensAttributeadaptorFetchAllByTranslation(dba,
                                             translation,
                                             (const AjPStr) NULL,
                                             translation->Attributes);

    return translation->Attributes;
}




/* @func ensTranslationGetDatabaseEntries *************************************
**
** Get all Ensembl Database Entries of an Ensembl Translation.
**
** This is not a simple accessor function, it will fetch
** Ensembl Database Entries from the Ensembl Core database in case the
** AJAX List is empty.
**
** @cc Bio::EnsEMBL::Translation::get_all_DBEntries
** @param [u] translation [EnsPTranslation] Ensembl Translation
**
** @return [const AjPList] AJAX List of Ensembl Database Entries
** @@
******************************************************************************/

const AjPList ensTranslationGetDatabaseEntries(EnsPTranslation translation)
{
    AjPStr objtype = NULL;

    EnsPDatabaseadaptor dba = NULL;

    EnsPDatabaseentryadaptor dbea = NULL;

    if(!translation)
        return NULL;

    if(translation->DatabaseEntries)
        return translation->DatabaseEntries;
    else
        translation->DatabaseEntries = ajListNew();

    if(!translation->Adaptor)
    {
        ajDebug("ensTranslationGetDatabaseEntries cannot fetch "
                "Ensembl Database Entries for a Translation without a "
                "Translation Adaptor.\n");

        return NULL;
    }

    dba = ensTranslationadaptorGetDatabaseadaptor(translation->Adaptor);

    if(!dba)
    {
        ajDebug("ensTranslationGetDatabaseEntries cannot fetch "
                "Ensembl Database Entries for a Translation without a "
                "Database Adaptor set in the Translation Adaptor.\n");

        return NULL;
    }

    dbea = ensRegistryGetDatabaseentryadaptor(dba);

    objtype = ajStrNewC("Translation");

    ensDatabaseentryadaptorFetchAllByObjectType(dbea,
                                                translation->Identifier,
                                                objtype,
                                                (AjPStr) NULL,
                                                ensEExternaldatabaseTypeNULL,
                                                translation->DatabaseEntries);

    ajStrDel(&objtype);

    return translation->DatabaseEntries;
}




/* @func ensTranslationGetProteinfeatures *************************************
**
** Get all Ensembl Protein Features of an Ensembl Translation.
**
** This is not a simple accessor function, it will fetch
** Ensembl Protein Features from the Ensembl Core database in case the
** AJAX List is empty.
**
** @cc Bio::EnsEMBL::Translation::get_all_Proteinfeatures
** @param [u] translation [EnsPTranslation] Ensembl Translation
**
** @return [const AjPList] AJAX List of Ensembl Protein Features
** @@
******************************************************************************/

const AjPList ensTranslationGetProteinfeatures(EnsPTranslation translation)
{
    EnsPDatabaseadaptor dba = NULL;

    EnsPProteinfeatureadaptor pfa = NULL;

    if(!translation)
        return NULL;

    if(translation->Proteinfeatures)
        return translation->Proteinfeatures;
    else
        translation->Proteinfeatures = ajListNew();

    if(!translation->Adaptor)
    {
        ajDebug("ensTranslationGetDatabaseEntries cannot fetch "
                "Ensembl Database Entries for a Translation without a "
                "Translation Adaptor.\n");

        return NULL;
    }

    dba = ensTranslationadaptorGetDatabaseadaptor(translation->Adaptor);

    if(!dba)
    {
        ajDebug("ensTranslationGetDatabaseEntries cannot fetch "
                "Ensembl Database Entries for a Translation without a "
                "Database Adaptor set in the Translation Adaptor.\n");

        return NULL;
    }

    pfa = ensRegistryGetProteinfeatureadaptor(dba);

    ensProteinfeatureadaptorFetchAllByTranslationIdentifier(
        pfa,
        translation->Identifier,
        translation->Proteinfeatures);

    return translation->Proteinfeatures;
}




/* @func ensTranslationGetTranscriptStart *************************************
**
** Get the start coordinate of an Ensembl Translation in
** Transcript coordinates.
**
** @cc Bio::EnsEMBL::Translation::cdna_start
** @param [u] translation [EnsPTranslation] Ensembl Translation
**
** @return [ajuint] Translation start coordinate relative to the Transcript
** @@
******************************************************************************/

ajuint ensTranslationGetTranscriptStart(EnsPTranslation translation)
{
    if(!translation)
        return 0;

    if(!translation->Transcript)
    {
        ajDebug("ensTranslationGetTranscriptStart requires a Transcript to be "
                "set in the Translation.\n");

        return 0;
    }

    if(!translation->TranscriptStart)
    {
        translation->TranscriptStart =
            ensExonGetTranscriptCodingStart(translation->StartExon,
                                            translation->Transcript);
    }

    return translation->TranscriptStart;
}




/* @func ensTranslationGetTranscriptEnd ***************************************
**
** Get the end coordinate of an Ensembl Translation in Transcript coordinates.
**
** @cc Bio::EnsEMBL::Translation::cdna_end
** @param [u] translation [EnsPTranslation] Ensembl Translation
**
** @return [ajuint] Translation end coordinate relative to the Transcript
** @@
******************************************************************************/

ajuint ensTranslationGetTranscriptEnd(EnsPTranslation translation)
{
    if(!translation)
        return 0;

    if(!translation->Transcript)
    {
        ajDebug("ensTranslationGetTranscriptEnd requires a Transcript to be "
                "set in the Translation.\n");

        return 0;
    }

    if(!translation->TranscriptEnd)
    {
        translation->TranscriptEnd =
            ensExonGetTranscriptCodingEnd(translation->EndExon,
                                          translation->Transcript);
    }

    return translation->TranscriptEnd;
}




/* @func ensTranslationGetSliceStart ******************************************
**
** Get the start position of the Translation in Slice coordinates.
**
** @cc Bio::EnsEMBL::Translation::genomic_start
** @param [u] translation [EnsPTranslation] Ensembl Translation
**
** @return [ajuint] Translation start coordinate relative to the Slice
** @@
******************************************************************************/

ajuint ensTranslationGetSliceStart(EnsPTranslation translation)
{
    EnsPFeature feature = NULL;

    if(!translation)
        return 0;

    if(!translation->SliceStart)
    {
        feature = ensExonGetFeature(translation->StartExon);

        if(ensFeatureGetStrand(feature) >= 0)
        {
            translation->SliceStart =
                ensFeatureGetStart(feature) + (translation->Start - 1);
        }
        else
        {
            feature = ensExonGetFeature(translation->EndExon);

            translation->SliceStart =
                ensFeatureGetEnd(feature) - (translation->End - 1);
        }
    }

    return translation->SliceStart;
}




/* @func ensTranslationGetSliceEnd ********************************************
**
** Get the end position of the Translation in Slice coordinates.
**
** @cc Bio::EnsEMBL::Translation::genomic_end
** @param [u] translation [EnsPTranslation] Ensembl Translation.
**
** @return [ajuint] Translation end coordinate relative to the Slice
** @@
******************************************************************************/

ajuint ensTranslationGetSliceEnd(EnsPTranslation translation)
{
    EnsPFeature feature = NULL;

    if(!translation)
        return 0;

    if(!translation->SliceEnd)
    {
        feature = ensExonGetFeature(translation->EndExon);

        if(ensFeatureGetStrand(feature) >= 0)
        {
            translation->SliceEnd =
                ensFeatureGetStart(feature) + (translation->End - 1);
        }
        else
        {
            feature = ensExonGetFeature(translation->StartExon);

            translation->SliceEnd =
                ensFeatureGetEnd(feature) - (translation->Start - 1);
        }
    }

    return translation->SliceEnd;
}




/* @func ensTranslationGetMemSize *********************************************
**
** Get the memory size in bytes of an Ensembl Translation.
**
** @param [r] translation [const EnsPTranslation] Ensembl Translation
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensTranslationGetMemSize(const EnsPTranslation translation)
{
    ajuint size = 0;

    AjIList iter = NULL;

    EnsPAttribute attribute = NULL;

    EnsPDatabaseentry dbe = NULL;

    EnsPProteinfeature pf = NULL;

    if(!translation)
        return 0;

    size += (ajuint) sizeof (EnsOTranslation);

    size += ensTranscriptGetMemSize(translation->Transcript);

    size += ensExonGetMemSize(translation->StartExon);

    size += ensExonGetMemSize(translation->EndExon);

    if(translation->Sequence)
    {
        size += (ajuint) sizeof (AjOStr);

        size += ajStrGetRes(translation->Sequence);
    }

    if(translation->StableIdentifier)
    {
        size += (ajuint) sizeof (AjOStr);

        size += ajStrGetRes(translation->StableIdentifier);
    }

    if(translation->CreationDate)
    {
        size += (ajuint) sizeof (AjOStr);

        size += ajStrGetRes(translation->CreationDate);
    }

    if(translation->ModificationDate)
    {
        size += (ajuint) sizeof (AjOStr);

        size += ajStrGetRes(translation->ModificationDate);
    }

    /* Summarise the AJAX List of Ensembl Attributes. */

    if(translation->Attributes)
    {
        size += (ajuint) sizeof (AjOList);

        iter = ajListIterNewread(translation->Attributes);

        while(!ajListIterDone(iter))
        {
            attribute = (EnsPAttribute) ajListIterGet(iter);

            size += ensAttributeGetMemSize(attribute);
        }

        ajListIterDel(&iter);
    }

    /* Summarise the AJAX List of Ensembl Database Entries. */

    if(translation->DatabaseEntries)
    {
        size += (ajuint) sizeof (AjOList);

        iter = ajListIterNewread(translation->DatabaseEntries);

        while(!ajListIterDone(iter))
        {
            dbe = (EnsPDatabaseentry) ajListIterGet(iter);

            size += ensDatabaseentryGetMemSize(dbe);
        }

        ajListIterDel(&iter);
    }

    /* Summarise the AJAX List of Ensembl Protein Features. */

    if(translation->Proteinfeatures)
    {
        size += (ajuint) sizeof (AjOList);

        iter = ajListIterNewread(translation->Proteinfeatures);

        while(!ajListIterDone(iter))
        {
            pf = (EnsPProteinfeature) ajListIterGet(iter);

            size += ensProteinfeatureGetMemSize(pf);
        }

        ajListIterDel(&iter);
    }

    return size;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an Ensembl Translation object.
**
** @fdata [EnsPTranslation]
** @fnote None
**
** @nam3rule Set Set one element of a Translation
** @nam4rule SetAdaptor Set the Ensembl Translation Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetTranscript Set the Ensembl Transcript
** @nam4rule SetStartExon Set the start Exon
** @nam4rule SetEndExon Set the end Exon
** @nam4rule SetStart Set the start coordinate
** @nam4rule SetEnd Set the end coordinate
** @nam4rule SetStableIdentifier Set the stable identifier
** @nam4rule SetVersion Set the version
** @nam4rule SetCreationDate Set the creation date
** @nam4rule SetModificationDate Set the modification date
**
** @argrule * translation [EnsPTranslation] Ensembl Translation object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensTranslationSetAdaptor *********************************************
**
** Set the Ensembl Translation Adaptor element of an Ensembl Translation.
**
** @param [u] translation [EnsPTranslation] Ensembl Translation
** @param [u] tla [EnsPTranslationadaptor] Ensembl Translation Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationSetAdaptor(EnsPTranslation translation,
                                EnsPTranslationadaptor tla)
{
    if(!translation)
        return ajFalse;

    translation->Adaptor = tla;

    return ajTrue;
}




/* @func ensTranslationSetIdentifier ******************************************
**
** Set the SQL database-internal identifier element of an Ensembl Translation.
**
** @param [u] translation [EnsPTranslation] Ensembl Translation
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationSetIdentifier(EnsPTranslation translation,
                                   ajuint identifier)
{
    if(!translation)
        return ajFalse;

    translation->Identifier = identifier;

    return ajTrue;
}




/* @func ensTranslationSetTranscript ******************************************
**
** Set the Ensembl Transcript element of an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Translation::???
** @param [u] translation [EnsPTranslation] Ensembl Translation
** @param [u] transcript [EnsPTranscript] Ensembl Transcript
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationSetTranscript(EnsPTranslation translation,
                                   EnsPTranscript transcript)
{
    if(ajDebugTest("ensTranslationSetTranscript"))
    {
        ajDebug("ensTranslationSetTranscript\n"
                "  translation %p\n"
                "  transcript %p\n",
                translation,
                transcript);

        ensTranslationTrace(translation, 1);

        ensTranscriptTrace(transcript, 1);
    }

    if(!translation)
        return ajFalse;

    if(!transcript)
        return ajFalse;

    /* Replace the current Transcript. */

    ensTranscriptUnlinkTranslation(translation->Transcript, translation);

    ensTranscriptDel(&(translation->Transcript));

    translation->Transcript = ensTranscriptNewRef(transcript);

    ensTranscriptLinkTranslation(translation->Transcript, translation);

    /* Clear internal caches that depend on Transcript coordinates. */

    ajStrDel(&translation->Sequence);

    translation->TranscriptStart = 0;
    translation->TranscriptEnd   = 0;
    translation->SliceStart      = 0;
    translation->SliceEnd        = 0;

    return ajTrue;
}




/* @func ensTranslationSetStartExon *******************************************
**
** Set the start Ensembl Exon element of an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Translation::start_Exon
** @param [u] translation [EnsPTranslation] Ensembl Translation
** @param [u] exon [EnsPExon] Ensembl Exon
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationSetStartExon(EnsPTranslation translation, EnsPExon exon)
{
    if(ajDebugTest("ensTranslationSetStartExon"))
    {
        ajDebug("ensTranslationSetStartExon\n"
                "  translation %p\n"
                "  exon %p\n",
                translation,
                exon);

        ensTranslationTrace(translation, 1);

        ensExonTrace(exon, 1);
    }

    if(!translation)
        return ajFalse;

    if(!exon)
        return ajFalse;

    /* Replace the current Exon. */

    if(translation->StartExon)
        ensExonDel(&(translation->StartExon));

    translation->StartExon = ensExonNewRef(exon);

    /* Clear internal caches that depend on Exon coordinates. */

    ajStrDel(&translation->Sequence);

    translation->TranscriptStart = 0;
    translation->TranscriptEnd   = 0;
    translation->SliceStart      = 0;
    translation->SliceEnd        = 0;

    return ajTrue;
}




/* @func ensTranslationSetEndExon *********************************************
**
** Set the end Ensembl Exon element of an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Translation::end_Exon
** @param [u] translation [EnsPTranslation] Ensembl Translation
** @param [u] exon [EnsPExon] Ensembl Exon
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationSetEndExon(EnsPTranslation translation, EnsPExon exon)
{
    if(ajDebugTest("ensTranslationSetEndExon"))
    {
        ajDebug("ensTranslationSetEndExon\n"
                "  translation %p\n"
                "  exon %p\n",
                translation,
                exon);

        ensTranslationTrace(translation, 1);

        ensExonTrace(exon, 1);
    }

    if(!translation)
        return ajFalse;

    if(!exon)
        return ajFalse;

    /* Replace the current Exon. */

    if(translation->EndExon)
        ensExonDel(&(translation->EndExon));

    translation->EndExon = ensExonNewRef(exon);

    /* Clear internal caches that depend on Exon coordinates. */

    ajStrDel(&translation->Sequence);

    translation->TranscriptStart = 0;
    translation->TranscriptEnd   = 0;
    translation->SliceStart      = 0;
    translation->SliceEnd        = 0;

    return ajTrue;
}




/* @func ensTranslationSetStart ***********************************************
**
** Set the start element of an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Translation::start
** @param [u] translation [EnsPTranslation] Ensembl Translation
** @param [r] start [ajuint] Start coordinate
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationSetStart(EnsPTranslation translation, ajuint start)
{
    if(ajDebugTest("ensTranslationSetStart"))
    {
        ajDebug("ensTranslationSetStart\n"
                "  translation %p\n"
                "  start %u\n",
                translation,
                start);

        ensTranslationTrace(translation, 1);
    }

    if(!translation)
        return ajFalse;

    translation->Start = start;

    /* Clear internal caches that depend on Exon coordinates. */

    ajStrDel(&translation->Sequence);

    translation->TranscriptStart = 0;
    translation->TranscriptEnd   = 0;
    translation->SliceStart      = 0;
    translation->SliceEnd        = 0;

    return ajTrue;
}




/* @func ensTranslationSetEnd *************************************************
**
** Set the end element of an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Translation::end
** @param [u] translation [EnsPTranslation] Ensembl Translation
** @param [r] end [ajuint] End coordinate
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationSetEnd(EnsPTranslation translation, ajuint end)
{
    if(ajDebugTest("ensTranslationSetEnd"))
    {
        ajDebug("ensTranslationSetEnd\n"
                "  translation %p\n"
                "  end %u\n",
                translation,
                end);

        ensTranslationTrace(translation, 1);
    }

    if(!translation)
        return ajFalse;

    translation->End = end;

    /* Clear internal caches that depend on Exon coordinates. */

    ajStrDel(&translation->Sequence);

    translation->TranscriptStart = 0;
    translation->TranscriptEnd   = 0;
    translation->SliceStart      = 0;
    translation->SliceEnd        = 0;

    return ajTrue;
}




/* @func ensTranslationSetStableIdentifier ************************************
**
** Set the stable identifier element of an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Translation::stable_id
** @param [u] translation [EnsPTranslation] Ensembl Translation
** @param [r] stableid [AjPStr] Stable identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationSetStableIdentifier(EnsPTranslation translation,
                                         AjPStr stableid)
{
    if(ajDebugTest("ensTranslationSetStableIdentifier"))
    {
        ajDebug("ensTranslationSetStableIdentifier\n"
                "  translation %p\n"
                "  stableid '%S'\n",
                translation,
                stableid);

        ensTranslationTrace(translation, 1);
    }

    if(!translation)
        return ajFalse;

    ajStrDel(&translation->StableIdentifier);

    translation->StableIdentifier = ajStrNewRef(stableid);

    return ajTrue;
}




/* @func ensTranslationSetVersion *********************************************
**
** Set the version element of an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Translation::version
** @param [u] translation [EnsPTranslation] Ensembl Translation
** @param [r] version [ajuint] Version
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationSetVersion(EnsPTranslation translation, ajuint version)
{
    if(ajDebugTest("ensTranslationSetVersion"))
    {
        ajDebug("ensTranslationSetVersion\n"
                "  translation %p\n"
                "  version %u\n",
                translation,
                version);

        ensTranslationTrace(translation, 1);
    }

    if(!translation)
        return ajFalse;

    translation->Version = version;

    return ajTrue;
}




/* @func ensTranslationSetCreationDate ****************************************
**
** Set the creation date element of an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Translation::created_date
** @param [u] translation [EnsPTranslation] Ensembl Translation
** @param [r] cdate [AjPStr] Creation date
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationSetCreationDate(EnsPTranslation translation, AjPStr cdate)
{
    if(ajDebugTest("ensTranslationSetCreationDate"))
    {
        ajDebug("ensTranslationSetCreationDate\n"
                "  translation %p\n"
                "  cdate '%S'\n",
                translation,
                cdate);

        ensTranslationTrace(translation, 1);
    }

    if(!translation)
        return ajFalse;

    ajStrDel(&translation->CreationDate);

    translation->CreationDate = ajStrNewRef(cdate);

    return ajTrue;
}




/* @func ensTranslationSetModificationDate ************************************
**
** Set the modification date element of an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Translation::created_date
** @param [u] translation [EnsPTranslation] Ensembl Translation
** @param [r] mdate [AjPStr] Modification date
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationSetModificationDate(EnsPTranslation translation,
                                         AjPStr mdate)
{
    if(ajDebugTest("ensTranslationSetModificationDate"))
    {
        ajDebug("ensTranslationSetModificationDate\n"
                "  translation %p\n"
                "  mdate '%S'\n",
                translation,
                mdate);

        ensTranslationTrace(translation, 1);
    }

    if(!translation)
        return ajFalse;

    ajStrDel(&translation->ModificationDate);

    translation->ModificationDate = ajStrNewRef(mdate);

    return ajTrue;
}




/* @func ensTranslationAddAttribute *******************************************
**
** Add an Ensembl Attribute to an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Translation::add_Attributes
** @param [u] translation [EnsPTranslation] Ensembl Translation
** @param [u] attribute [EnsPAttribute] Ensembl Attribute
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
** Adding an Ensembl Attribute also clears the sequence cache.
******************************************************************************/

AjBool ensTranslationAddAttribute(EnsPTranslation translation,
                                  EnsPAttribute attribute)
{
    if(!translation)
        return ajFalse;

    if(!attribute)
        return ajFalse;

    if(!translation->Attributes)
        translation->Attributes = ajListNew();

    ajListPushAppend(translation->Attributes,
                     (void *) ensAttributeNewRef(attribute));

    /* Clear the sequence cache. */

    if(translation->Sequence)
        ajStrDel(&translation->Sequence);

    return ajTrue;
}




/* @func ensTranslationAddDatabaseentry ***************************************
**
** Add an Ensembl Database Entry to an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Translation::add_DBEntry
** @param [u] translation [EnsPTranslation] Ensembl Translation
** @param [u] dbe [EnsPDatabaseentry] Ensembl Database Entry
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationAddDatabaseentry(EnsPTranslation translation,
                                      EnsPDatabaseentry dbe)
{
    if(!translation)
        return ajFalse;

    if(!dbe)
        return ajFalse;

    if(!translation->DatabaseEntries)
        translation->DatabaseEntries = ajListNew();

    ajListPushAppend(translation->DatabaseEntries,
                     (void *) ensDatabaseentryNewRef(dbe));

    return ajTrue;
}




/* @func ensTranslationAddProteinfeature **************************************
**
** Add an Ensembl Protein Feature to an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Translation::add_Proteinfeature
** @param [u] translation [EnsPTranslation] Ensembl Translation
** @param [u] pf [EnsPProteinfeature] Ensembl Protein Feature
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationAddProteinfeature(EnsPTranslation translation,
                                       EnsPProteinfeature pf)
{
    if(!translation)
        return ajFalse;

    if(!pf)
        return ajFalse;

    if(!translation->Proteinfeatures)
        translation->Proteinfeatures = ajListNew();

    ajListPushAppend(translation->Proteinfeatures,
                     (void *) ensProteinfeatureNewRef(pf));

    return ajTrue;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Translation object.
**
** @fdata [EnsPTranslation]
** @nam3rule Trace Report Ensembl Translation elements to debug file
**
** @argrule Trace translation [const EnsPTranslation] Ensembl Translation
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensTranslationTrace **************************************************
**
** Trace an Ensembl Translation.
**
** @param [r] translation [const EnsPTranslation] Ensembl Translation
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationTrace(const EnsPTranslation translation, ajuint level)
{
    AjIList iter = NULL;

    AjPStr indent = NULL;

    EnsPAttribute attribute = NULL;

    EnsPDatabaseentry dbe = NULL;

    EnsPProteinfeature pf = NULL;

    if(!translation)
        return ajFalse;

    indent = ajStrNew();

    ajStrAppendCountK(&indent, ' ', level * 2);

    ajDebug("%SensTranslationTrace %p\n"
            "%S  Use %u\n"
            "%S  Identifier %u\n"
            "%S  Adaptor %p\n"
            "%S  Transcript %p\n"
            "%S  StartExon %p\n"
            "%S  EndExon %p\n"
            "%S  Start %u\n"
            "%S  End %u\n"
            "%S  StableIdentifier '%S'\n"
            "%S  CreationDate '%S'\n"
            "%S  ModificationDate '%S'\n"
            "%S  Version %u\n"
            "%S  Attributes %p\n"
            "%S  DatabaseEntries %p\n"
            "%S  Proteinfeatures %p\n"
            "%S  Sequence %p\n"
            "%S  TranscriptStart %u\n"
            "%S  TranscriptEnd %u\n"
            "%S  SliceStart %u\n"
            "%S  SliceEnd %u\n",
            indent, translation,
            indent, translation->Use,
            indent, translation->Identifier,
            indent, translation->Adaptor,
            indent, translation->Transcript,
            indent, translation->StartExon,
            indent, translation->EndExon,
            indent, translation->Start,
            indent, translation->End,
            indent, translation->StableIdentifier,
            indent, translation->CreationDate,
            indent, translation->ModificationDate,
            indent, translation->Version,
            indent, translation->Attributes,
            indent, translation->DatabaseEntries,
            indent, translation->Proteinfeatures,
            indent, translation->Sequence,
            indent, translation->TranscriptStart,
            indent, translation->TranscriptEnd,
            indent, translation->SliceStart,
            indent, translation->SliceEnd);

    ensTranscriptTrace(translation->Transcript, level + 1);

    ensExonTrace(translation->StartExon, level + 1);

    ensExonTrace(translation->EndExon, level + 1);

    /* Trace the AJAX List of Ensembl Attributes. */

    if(translation->Attributes)
    {
        ajDebug("%S    AJAX List %p of Ensembl Attributes\n",
                indent, translation->Attributes);

        iter = ajListIterNewread(translation->Attributes);

        while(!ajListIterDone(iter))
        {
            attribute = (EnsPAttribute) ajListIterGet(iter);

            ensAttributeTrace(attribute, level + 2);
        }

        ajListIterDel(&iter);
    }

    /* Trace the AJAX List of Ensembl Database Entries. */

    if(translation->DatabaseEntries)
    {
        ajDebug("%S    AJAX List %p of Ensembl Database Entries\n",
                indent, translation->Attributes);

        iter = ajListIterNewread(translation->DatabaseEntries);

        while(!ajListIterDone(iter))
        {
            dbe = (EnsPDatabaseentry) ajListIterGet(iter);

            ensDatabaseentryTrace(dbe, level + 2);
        }

        ajListIterDel(&iter);
    }

    /* Trace the AJAX List of Ensembl Protein Features. */

    if(translation->Proteinfeatures)
    {
        ajDebug("%S    AJAX List %p of Ensembl Protein Features\n",
                indent, translation->Proteinfeatures);

        iter = ajListIterNewread(translation->Proteinfeatures);

        while(!ajListIterDone(iter))
        {
            pf = (EnsPProteinfeature) ajListIterGet(iter);

            ensProteinfeatureTrace(pf, level + 2);
        }

        ajListIterDel(&iter);
    }

    ajStrDel(&indent);

    return ajTrue;
}




/* @func ensTranslationFetchAllAttributes *************************************
**
** Fetch all Ensembl Attributes of an Ensembl Translation.
**
** The caller is responsible for deleting the Ensembl Attributes before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Translation::get_all_Attributes
** @param [u] translation [EnsPTranslation] Ensembl Translation
** @param [r] code [const AjPStr] Ensembl Attribute code
** @param [r] attributes [AjPList] AJAX List of Ensembl Attributes
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationFetchAllAttributes(EnsPTranslation translation,
                                        const AjPStr code,
                                        AjPList attributes)
{
    AjBool match = AJFALSE;

    const AjPList list = NULL;
    AjIList iter       = NULL;

    EnsPAttribute attribute = NULL;

    if(ajDebugTest("ensTranslationFetchAllAttributes"))
        ajDebug("ensTranslationFetchAllAttributes\n"
                "  translation %p\n"
                "  code '%S'\n"
                "  attributes %p\n",
                translation,
                code,
                attributes);

    if(!translation)
        return ajFalse;

    if(!attributes)
        return ajFalse;

    list = ensTranslationGetAttributes(translation);

    iter = ajListIterNewread(list);

    while(!ajListIterDone(iter))
    {
        attribute = (EnsPAttribute) ajListIterGet(iter);

        if(code)
        {
            if(ajStrMatchCaseS(code, ensAttributeGetCode(attribute)))
                match = ajTrue;
            else
                match = ajFalse;
        }
        else
            match = ajTrue;

        if(match)
            ajListPushAppend(attributes,
                             (void *) ensAttributeNewRef(attribute));
    }

    ajListIterDel(&iter);

    return ajTrue;
}




/* @func ensTranslationFetchAllDatabaseEntries ********************************
**
** Fetch all Ensembl Database Entries of an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Translation::get_all_DBEntries
** @param [u] translation [EnsPTranslation] Ensembl Translation
** @param [r] name [const AjPStr] Ensembl External Database name
** @param [r] type [AjEnum] Ensembl External Database type
** @param [u] dbes [AjPList] AJAX List of Ensembl Database Entries
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationFetchAllDatabaseEntries(EnsPTranslation translation,
                                             const AjPStr name,
                                             AjEnum type,
                                             AjPList dbes)
{
    AjBool namematch = AJFALSE;
    AjBool typematch = AJFALSE;

    const AjPList list = NULL;
    AjIList iter       = NULL;

    EnsPDatabaseentry dbe = NULL;

    if(!translation)
        return ajFalse;

    if(!dbes)
        return ajFalse;

    list = ensTranslationGetDatabaseEntries(translation);

    iter = ajListIterNewread(list);

    while(!ajListIterDone(iter))
    {
        dbe = (EnsPDatabaseentry) ajListIterGet(iter);

        if(name)
        {
            if(ajStrMatchCaseS(name, ensDatabaseentryGetDbName(dbe)))
                namematch = ajTrue;
            else
                namematch = ajFalse;
        }
        else
            namematch = ajTrue;

        if(type)
        {
            if(type == ensDatabaseentryGetType(dbe))
                typematch = ajTrue;
            else
                typematch = ajFalse;
        }
        else
            typematch = ajTrue;

        if(namematch && typematch)
            ajListPushAppend(dbes, (void *) ensDatabaseentryNewRef(dbe));
    }

    ajListIterDel(&iter);

    return ajTrue;
}




/* @func ensTranslationFetchAllProteinfeatures ********************************
**
** Fetch all Ensembl Protein Features of an Ensembl Translation.
**
** @cc Bio::EnsEMBL::Translation::get_all_Proteinfeatures
** @param [u] translation [EnsPTranslation] Ensembl Translation
** @param [r] name [const AjPStr] Ensembl Analysis name
** @param [u] pfs [AjPList] AJAX List of Ensembl Protein Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationFetchAllProteinfeatures(EnsPTranslation translation,
                                             const AjPStr name,
                                             AjPList pfs)
{
    AjBool match = AJFALSE;

    const AjPList list = NULL;

    AjIList iter = NULL;

    EnsPAnalysis analysis = NULL;

    EnsPFeature feature = NULL;

    EnsPFeaturepair fp = NULL;

    EnsPProteinfeature pf = NULL;

    if(!translation)
        return ajFalse;

    if(!pfs)
        return ajFalse;

    list = ensTranslationGetProteinfeatures(translation);

    iter = ajListIterNewread(list);

    while(!ajListIterDone(iter))
    {
        pf = (EnsPProteinfeature) ajListIterGet(iter);

        if(name)
        {
            fp = ensProteinfeatureGetFeaturepair(pf);

            feature = ensFeaturepairGetSourceFeature(fp);

            analysis = ensFeatureGetAnalysis(feature);

            if(ajStrMatchCaseS(name, ensAnalysisGetName(analysis)))
                match = ajTrue;
            else
                match = ajFalse;
        }
        else
            match = ajTrue;

        if(match)
            ajListPushAppend(pfs, (void *) ensProteinfeatureNewRef(pf));
    }

    ajListIterDel(&iter);

    return ajTrue;
}




/* @func ensTranslationFetchAllProteinfeaturesDomain **************************
**
** Fetch all Ensembl Protein Features of an Ensembl Translation that are
** associated to the InterPro conserved domain database.
**
** The cooresponding Ensembl Analysis names for Protein Features have to be
** defined in the static const char* translationProteinfeatureDomainNames[]
** array.
**
** @cc Bio::EnsEMBL::Translation::get_all_DomainFeatures
** @param [u] translation [EnsPTranslation] Ensembl Translation
** @param [u] pfs [AjPList] AJAX List of Ensembl Protein Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationFetchAllProteinfeaturesDomain(EnsPTranslation translation,
                                                   AjPList pfs)
{
    register ajuint i = 0;

    AjPStr name = NULL;

    if(!translation)
        return ajFalse;

    if(!pfs)
        return ajFalse;

    name = ajStrNew();

    for(i = 0; translationProteinfeatureDomainName[i]; i++)
    {
        ajStrAssignC(&name, translationProteinfeatureDomainName[i]);

        ensTranslationFetchAllProteinfeatures(translation, name, pfs);
    }

    ajStrDel(&name);

    return ajTrue;
}




/* @func ensTranslationFetchAllSequenceEdits **********************************
**
** Fetch all Ensembl Sequence Edits for an Ensembl Transcript.
**
** The corresponding Ensembl Attribute codes have to be defined in the
** static const char *translationSequenceEditCode array.
**
** The caller is responsible for deleting the Ensembl Sequence Edits before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Translation::get_all_SeqEdits
** @param [u] translation [EnsPTranslation] Ensembl Translation
** @param [r] ses [AjPList] AJAX List of Ensembl Sequence Edits
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationFetchAllSequenceEdits(EnsPTranslation translation,
                                           AjPList ses)
{
    register ajuint i = 0;

    AjPList attributes = NULL;

    AjPStr code = NULL;

    EnsPAttribute at = NULL;

    EnsPSequenceEdit se = NULL;

    if(!translation)
        return ajFalse;

    if(!ses)
        return ajFalse;

    code = ajStrNew();

    attributes = ajListNew();

    for(i = 0; translationSequenceEditCode[i]; i++)
    {
        ajStrAssignC(&code, translationSequenceEditCode[i]);

        ensTranslationFetchAllAttributes(translation, code, attributes);
    }

    while(ajListPop(attributes, (void **) &at))
    {
        se = ensSequenceEditNewA(at);

        ajListPushAppend(ses, (void *) se);

        ensAttributeDel(&at);
    }

    ajListFree(&attributes);

    ajStrDel(&code);

    return ajTrue;
}




/* @func ensTranslationFetchDisplayIdentifier *********************************
**
** Fetch the display identifier of an Ensembl Translation.
** This will return the stable identifier, the SQL database-internal identifier
** or the Translation memory address in this order.
** The caller is responsible for deleting the AJAX String object.
**
** @cc Bio::EnsEMBL::Translation:display_id
** @param [r] translation [const EnsPTranslation] Ensembl Translation
** @param [wP] Pidentifier [AjPStr*] Display identifier address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationFetchDisplayIdentifier(const EnsPTranslation translation,
                                            AjPStr *Pidentifier)
{
    if(!translation)
        return ajFalse;

    if(!Pidentifier)
        return ajFalse;

    if(translation->StableIdentifier &&
       ajStrGetLen(translation->StableIdentifier))
        *Pidentifier = ajStrNewS(translation->StableIdentifier);
    else if(translation->Identifier)
        *Pidentifier = ajFmtStr("%u", translation->Identifier);
    else
        *Pidentifier = ajFmtStr("%p", translation);

    return ajTrue;
}




/* @func ensTranslationFetchSequenceStr ***************************************
**
** Fetch the sequence of an Ensembl Translation as AJAX String.
**
** The sequence is based on ensTranscriptFetchTranslatedSequence.
**
** The caller is responsible for deleting the AJAX String.
**
** @cc Bio::EnsEMBL::Translation::seq
** @cc Bio::EnsEMBL::Translation::modify_translation
** @param [u] translation [EnsPTranslation] Ensembl Translation
** @param [r] Psequence [AjPStr*] Sequence address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
** NOTE: In this implementation, the Ensembl Translation object does not cache
** the translated sequence, since Sequence Edits applied to Transcript objects
** invalidate the sequence. Because Transcripts hold only weak references for
** Translation objects, it would be difficult to propagate changes from
** Transcripts to Translations. Nevertheless, Ensembl Exon sequences are
** cached as are Transcript and Translation Attributes so that the sequence can
** be recalculated quickly.
******************************************************************************/

AjBool ensTranslationFetchSequenceStr(EnsPTranslation translation,
                                      AjPStr *Psequence)
{
    if(!translation)
        return ajFalse;

    if(!Psequence)
        return ajFalse;

    if(*Psequence)
        ajStrAssignClear(Psequence);
    else
        *Psequence = ajStrNew();

    if(translation->Sequence)
        ajStrAssignS(Psequence, translation->Sequence);

    if(!translation->Transcript)
    {
        ajDebug("ensTranslationFetchSequenceStr requires a Translation with a "
                "Transcript.\n");

        return ajFalse;
    }

    ensTranscriptFetchTranslatedSequence(translation->Transcript, Psequence);

    return ajTrue;
}




/* @func ensTranslationFetchSequenceSeq ***************************************
**
** Fetch the sequence of an Ensembl Translation as AJAX Sequence.
** The caller is responsible for deleting the AJAX Sequence.
**
** @cc Bio::EnsEMBL::Translation:seq
** @param [u] translation [EnsPTranslation] Ensembl Translation
** @param [wP] Psequence [AjPSeq*] AJAX Sequence address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationFetchSequenceSeq(EnsPTranslation translation,
                                      AjPSeq *Psequence)
{
    AjPStr name = NULL;
    AjPStr sequence = NULL;

    if(!translation)
        return ajFalse;

    if(!Psequence)
        return ajFalse;

    ensTranslationFetchSequenceStr(translation, &sequence);

    ensTranslationFetchDisplayIdentifier(translation, &name);

    *Psequence = ajSeqNewNameS(sequence, name);

    ajStrDel(&name);
    ajStrDel(&sequence);

    return ajTrue;
}




/* @datasection [EnsPTranslationadaptor] Translation Adaptor ******************
**
** Functions for manipulating Ensembl Translation Adaptor objects
**
** @cc Bio::EnsEMBL::DBSQL::Translationadaptor CVS Revision: 1.35
**
** @nam2rule Translationadaptor
**
******************************************************************************/

static const char *translationAdaptorTables[] =
{
    "translation",
    "translation_stable_id",
    NULL
};

static const char *translationAdaptorColumns[] =
{
    "translation.translation_id",
    "translation.transcript_id",
    "translation.seq_start",
    "translation.start_exon_id",
    "translation.seq_end",
    "translation.end_exon_id",
    "translation_stable_id.stable_id",
    "translation_stable_id.version",
    "translation_stable_id.created_date",
    "translation_stable_id.modified_date",
    NULL
};

static EnsOBaseadaptorLeftJoin translationAdaptorLeftJoin[] =
{
    {
        "translation_stable_id",
        "translation_stable_id.translation_id = translation.translation_id"
    },
    {NULL, NULL}
};

static const char *translationAdaptorDefaultCondition = NULL;

static const char *translationAdaptorFinalCondition = NULL;




/* @funcstatic translationAdaptorFetchAllBySQL ********************************
**
** Fetch all Ensembl Translation objects via an SQL statement.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [uN] am [EnsPAssemblymapper] Ensembl Assembly Mapper
** @param [uN] slice [EnsPSlice] Ensembl Slice
** @param [u] translations [AjPList] AJAX List of Ensembl Translations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool translationAdaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                              const AjPStr statement,
                                              EnsPAssemblymapper am,
                                              EnsPSlice slice,
                                              AjPList translations)
{
    ajuint identifier   = 0;
    ajuint transcriptid = 0;
    ajuint startexonid  = 0;
    ajuint start        = 0;
    ajuint endexonid    = 0;
    ajuint end          = 0;
    ajuint version      = 0;

    AjIList iter = NULL;
    const AjPList exons = NULL;

    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;

    AjPStr stableid = NULL;
    AjPStr cdate    = NULL;
    AjPStr mdate    = NULL;

    EnsPExon exon      = NULL;
    EnsPExon startexon = NULL;
    EnsPExon endexon   = NULL;

    EnsPTranscript transcript = NULL;
    EnsPTranscriptadaptor tra = NULL;

    EnsPTranslation translation = NULL;
    EnsPTranslationadaptor tla = NULL;

    if(ajDebugTest("translationAdaptorFetchAllBySQL"))
        ajDebug("translationAdaptorFetchAllBySQL\n"
                "  dba %p\n"
                "  statement %p\n"
                "  am %p\n"
                "  slice %p\n"
                "  translations %p\n",
                dba,
                statement,
                am,
                slice,
                translations);

    if(!dba)
        return ajFalse;

    if(!statement)
        return ajFalse;

    if(!translations)
        return ajFalse;

    tla = ensRegistryGetTranslationadaptor(dba);

    tra = ensRegistryGetTranscriptadaptor(dba);

    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);

    sqli = ajSqlrowiterNew(sqls);

    while(!ajSqlrowiterDone(sqli))
    {
        identifier   = 0;
        transcriptid = 0;
        start        = 0;
        startexonid  = 0;
        end          = 0;
        endexonid    = 0;
        stableid     = ajStrNew();
        version      = 0;
        cdate        = ajStrNew();
        mdate        = ajStrNew();

        sqlr = ajSqlrowiterGet(sqli);

        ajSqlcolumnToUint(sqlr, &identifier);
        ajSqlcolumnToUint(sqlr, &transcriptid);
        ajSqlcolumnToUint(sqlr, &start);
        ajSqlcolumnToUint(sqlr, &startexonid);
        ajSqlcolumnToUint(sqlr, &end);
        ajSqlcolumnToUint(sqlr, &endexonid);
        ajSqlcolumnToStr(sqlr, &stableid);
        ajSqlcolumnToUint(sqlr, &version);
        ajSqlcolumnToStr(sqlr, &cdate);
        ajSqlcolumnToStr(sqlr, &mdate);

        ensTranscriptadaptorFetchByIdentifier(tra, transcriptid, &transcript);

        if(!transcript)
            ajFatal("translationAdaptorFetchAllBySQL could not get "
                    "Ensembl Transcript for identifier %u.\n", transcriptid);

        exons = ensTranscriptGetExons(transcript);

        iter = ajListIterNewread(exons);

        while(!ajListIterDone(iter))
        {
            exon = (EnsPExon) ajListIterGet(iter);

            if(ensExonGetIdentifier(exon) == startexonid)
                startexon = exon;

            if(ensExonGetIdentifier(exon) == endexonid)
                endexon = exon;
        }

        ajListIterDel(&iter);

        if(!startexon)
            ajFatal("translationAdaptorFetchAllBySQL could not get "
                    "start Exon for transcript_id %u",
                    ensTranscriptGetIdentifier(transcript));

        if(!endexon)
            ajFatal("translationAdaptorFetchAllBySQL could not get "
                    "end Exon for transcript_id %u",
                    ensTranscriptGetIdentifier(transcript));

        translation = ensTranslationNew(tla,
                                        identifier,
                                        transcript,
                                        startexon,
                                        endexon,
                                        start,
                                        end,
                                        (AjPStr) NULL,
                                        stableid,
                                        version,
                                        cdate,
                                        mdate);

        ajListPushAppend(translations, (void *) translation);	

        ensTranscriptDel(&transcript);

        ajStrDel(&stableid);

        ajStrDel(&cdate);

        ajStrDel(&mdate);	
    }

    ajSqlrowiterDel(&sqli);

    ajSqlstatementDel(&sqls);

    return ajTrue;
}




#if AJFALSE

/* @funcstatic translationAdaptorCacheReference *******************************
**
** Wrapper function to reference an Ensembl Translation
** from an Ensembl Cache.
**
** @param [r] value [void *] Ensembl Translation
**
** @return [void *] Ensembl Translation or NULL
** @@
******************************************************************************/

static void *translationAdaptorCacheReference(void *value)
{
    if(!value)
        return NULL;

    return (void *) ensTranslationNewRef((EnsPTranslation) value);
}




/* @funcstatic translationAdaptorCacheDelete **********************************
**
** Wrapper function to delete an Ensembl Translation
** from an Ensembl Cache.
**
** @param [r] value [void**] Ensembl Translation address
**
** @return [void]
** @@
******************************************************************************/

static void translationAdaptorCacheDelete(void **value)
{
    if(!value)
        return;

    ensTranslationDel((EnsPTranslation *) value);

    return;
}




/* @funcstatic translationAdaptorCacheSize ************************************
**
** Wrapper function to determine the memory size of an Ensembl Translation
** from an Ensembl Cache.
**
** @param [r] value [const void*] Ensembl Translation
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

static ajuint translationAdaptorCacheSize(const void *value)
{
    if(!value)
        return 0;

    return ensTranslationGetMemSize((EnsPTranslation) value);
}

#endif




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Translation Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** Translation Adaptor. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPTranslationadaptor]
** @fnote None
**
** @nam3rule New Constructor
**
** @argrule New dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @valrule * [EnsPTranslationadaptor] Ensembl Translation Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensTranslationadaptorNew *********************************************
**
** Default Ensembl Translation Adaptor constructor.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPTranslationadaptor] Ensembl Translation Adaptor or NULL
** @@
******************************************************************************/

EnsPTranslationadaptor ensTranslationadaptorNew(EnsPDatabaseadaptor dba)
{
    EnsPTranslationadaptor tla = NULL;

    if(!dba)
        return NULL;

    AJNEW0(tla);

    tla->Adaptor = ensBaseadaptorNew(
        dba,
        translationAdaptorTables,
        translationAdaptorColumns,
        translationAdaptorLeftJoin,
        translationAdaptorDefaultCondition,
        translationAdaptorFinalCondition,
        translationAdaptorFetchAllBySQL);

    return tla;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Translation Adaptor.
**
** @fdata [EnsPTranslationadaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Translation Adaptor object
**
** @argrule * Ptla [EnsPTranslationadaptor*] Ensembl Translation Adaptor
**                                           object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensTranslationadaptorDel *********************************************
**
** Default destructor for an Ensembl Translation Adaptor.
**
** @param [d] Ptla [EnsPTranslationadaptor*] Ensembl Translation Adaptor
**                                           address
**
** @return [void]
** @@
******************************************************************************/

void ensTranslationadaptorDel(EnsPTranslationadaptor *Ptla)
{
    EnsPTranslationadaptor pthis = NULL;

    if(!Ptla)
        return;

    if(!*Ptla)
        return;

    pthis = *Ptla;

    ensBaseadaptorDel(&pthis->Adaptor);

    AJFREE(pthis);

    *Ptla = NULL;

    return;
}




/* @func ensTranslationadaptorGetDatabaseadaptor ******************************
**
** Get the Ensembl Database Adaptor element of an Ensembl Translation Adaptor.
**
** @param [r] tla [const EnsPTranslationadaptor] Ensembl Translation Adaptor
**
** @return [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @@
******************************************************************************/

EnsPDatabaseadaptor ensTranslationadaptorGetDatabaseadaptor(
    const EnsPTranslationadaptor tla)
{
    if(!tla)
        return NULL;

    return ensBaseadaptorGetDatabaseadaptor(tla->Adaptor);
}




/* @func ensTranslationadaptorFetchByIdentifier *******************************
**
** Fetch an Ensembl Translation via its SQL database-internal identifier.
** The caller is responsible for deleting the Ensembl Translation.
**
** @param [u] tla [EnsPTranslationadaptor] Ensembl Translation Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @param [wP] Ptranslation [EnsPTranslation*] Ensembl Translation address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationadaptorFetchByIdentifier(EnsPTranslationadaptor tla,
                                              ajuint identifier,
                                              EnsPTranslation *Ptranslation)
{
    if(!tla)
        return ajFalse;

    if(!identifier)
        return ajFalse;

    if(!Ptranslation)
        return ajFalse;

    *Ptranslation = (EnsPTranslation)
        ensBaseadaptorFetchByIdentifier(tla->Adaptor, identifier);

    return ajTrue;
}




/* @func ensTranslationadaptorFetchByStableIdentifier *************************
**
** Fetch an Ensembl Translation via its stable identifier and version.
** In case a version is not specified, the current Ensembl Translation
** will be returned.
** The caller is responsible for deleting the Ensembl Translation.
**
** @param [r] tla [EnsPTranslationadaptor] Ensembl Translation Adaptor
** @param [r] stableid [const AjPStr] Stable identifier
** @param [r] version [ajuint] Version
** @param [wP] Ptranslation [EnsPTranslation*] Ensembl Translation address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationadaptorFetchByStableIdentifier(
    EnsPTranslationadaptor tla,
    const AjPStr stableid,
    ajuint version,
    EnsPTranslation *Ptranslation)
{
    char *txtstableid = NULL;

    AjPList translations = NULL;

    AjPStr constraint = NULL;

    EnsPTranslation translation = NULL;

    if(!tla)
        return ajFalse;

    if(!stableid)
        return ajFalse;

    if(!Ptranslation)
        return ajFalse;

    ensBaseadaptorEscapeC(tla->Adaptor, &txtstableid, stableid);

    if(version)
        constraint = ajFmtStr(
            "translation_stable_id.stable_id = '%s' "
            "AND "
            "translation_stable_id.version = %u",
            txtstableid,
            version);
    else
        constraint = ajFmtStr(
            "translation_stable_id.stable_id = '%s'",
            txtstableid);

    ajCharDel(&txtstableid);

    translations = ajListNew();

    ensBaseadaptorGenericFetch(tla->Adaptor,
                               constraint,
                               (EnsPAssemblymapper) NULL,
                               (EnsPSlice) NULL,
                               translations);

    if(ajListGetLength(translations) > 1)
        ajDebug("ensTranslationadaptorFetchByStableIdentifier got more than "
                "one Translation for stable identifier '%S' and version %u.\n",
                stableid, version);

    ajListPop(translations, (void **) Ptranslation);

    while(ajListPop(translations, (void **) &translation))
        ensTranslationDel(&translation);

    ajListFree(&translations);

    ajStrDel(&constraint);

    return ajTrue;
}




/* @func ensTranslationadaptorFetchByTranscript *******************************
**
** Fetch an Ensembl Translation via an Ensembl Transcript.
** The caller is responsible for deleting the Ensembl Translation.
**
** @param [r] tla [EnsPTranslationadaptor] Ensembl Translation Adaptor
** @param [r] transcript [EnsPTranscript] Ensembl Transcript
** @param [wP] Ptranslation [EnsPTranslation*] Ensembl Translation address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationadaptorFetchByTranscript(EnsPTranslationadaptor tla,
                                              EnsPTranscript transcript,
                                              EnsPTranslation *Ptranslation)
{
    ajuint identifier   = 0;
    ajuint transcriptid = 0;
    ajuint startexonid  = 0;
    ajuint start        = 0;
    ajuint endexonid    = 0;
    ajuint end          = 0;
    ajuint version      = 0;

    ajulong rows = 0;

    AjIList iter        = NULL;
    const AjPList exons = NULL;

    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;

    AjPStr stableid = NULL;
    AjPStr cdate    = NULL;
    AjPStr mdate    = NULL;
    AjPStr statement = NULL;

    EnsPDatabaseadaptor dba = NULL;

    EnsPExon exon      = NULL;
    EnsPExon startexon = NULL;
    EnsPExon endexon   = NULL;

    ajDebug("ensTranslationadaptorFetchByTranscript\n"
            "  tla %p\n"
            "  transcript %p\n"
            "  Ptranslation %p\n",
            tla,
            transcript,
            Ptranslation);

    if(!tla)
        return ajFalse;

    if(!transcript)
        return ajFalse;

    if(!Ptranslation)
        return ajFalse;

    dba = ensBaseadaptorGetDatabaseadaptor(tla->Adaptor);

    statement = ajFmtStr(
        "SELECT "
        "translation.translation_id, "
        "translation.transcript_id, "
        "translation.seq_start, "
        "translation.start_exon_id, "
        "translation.seq_end, "
        "translation.end_exon_id, "
        "translation_stable_id.stable_id, "
        "translation_stable_id.version, "
        "translation_stable_id.created_date, "
        "translation_stable_id.modified_date "
        "FROM "
        "(translation) "
        "LEFT JOIN "
        "translation_stable_id "
        "ON "
        "translation_stable_id.translation_id = "
        "translation.translation_id "
        "WHERE "
        "translation.transcript_id = %u",
        ensTranscriptGetIdentifier(transcript));

    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);

    /* Check first, how many rows were returned. */

    rows = ajSqlstatementGetSelectedrows(sqls);

    if(rows == 0)
    {
        ajDebug("ensTranslationadaptorFetchByTranscript could not get "
                "an Ensembl Translation for Ensembl Transcript %u.\n",
                ensTranscriptGetIdentifier(transcript));

        ensTranscriptTrace(transcript, 1);
    }
    else if(rows > 1)
    {
        ajDebug("ensTranslationadaptorFetchByTranscript got more than one "
                "Ensembl Translation for Ensembl Transcript %u.\n",
                ensTranscriptGetIdentifier(transcript));

        ensTranscriptTrace(transcript, 1);

        ajSqlstatementDel(&sqls);

        ajStrDel(&statement);

        return ajFalse;
    }

    sqli = ajSqlrowiterNew(sqls);

    while(!ajSqlrowiterDone(sqli))
    {
        identifier = 0;
        transcriptid = 0;
        start = 0;
        startexonid = 0;
        end = 0;
        endexonid = 0;
        stableid = ajStrNew();
        version = 0;
        cdate = ajStrNew();
        mdate = ajStrNew();

        sqlr = ajSqlrowiterGet(sqli);

        ajSqlcolumnToUint(sqlr, &identifier);
        ajSqlcolumnToUint(sqlr, &transcriptid);
        ajSqlcolumnToUint(sqlr, &start);
        ajSqlcolumnToUint(sqlr, &startexonid);
        ajSqlcolumnToUint(sqlr, &end);
        ajSqlcolumnToUint(sqlr, &endexonid);
        ajSqlcolumnToStr(sqlr, &stableid);
        ajSqlcolumnToUint(sqlr, &version);
        ajSqlcolumnToStr(sqlr, &cdate);
        ajSqlcolumnToStr(sqlr, &mdate);

        exons = ensTranscriptGetExons(transcript);

        iter = ajListIterNewread(exons);

        while(!ajListIterDone(iter))
        {
            exon = (EnsPExon) ajListIterGet(iter);

            if(ensExonGetIdentifier(exon) == startexonid)
                startexon = exon;

            if(ensExonGetIdentifier(exon) == endexonid)
                endexon = exon;
        }

        ajListIterDel(&iter);

        if(!startexon)
            ajFatal("ensTranslationadaptorFetchByTranscript could not get "
                    "start Exon for transcript_id %u.",
                    ensTranscriptGetIdentifier(transcript));

        if(!endexon)
            ajFatal("ensTranslationadaptorFetchByTranscript could not get "
                    "end Exon for transcript_id %u.",
                    ensTranscriptGetIdentifier(transcript));

        *Ptranslation = ensTranslationNew(tla,
                                          identifier,
                                          transcript,
                                          startexon,
                                          endexon,
                                          start,
                                          end,
                                          (AjPStr) NULL,
                                          stableid,
                                          version,
                                          cdate,
                                          mdate);

        ajStrDel(&stableid);
        ajStrDel(&cdate);
        ajStrDel(&mdate);	
    }

    ajSqlrowiterDel(&sqli);

    ajSqlstatementDel(&sqls);

    ajStrDel(&statement);

    return ajTrue;
}




#if AJFALSE

/*
** FIXME: Relations between Transcripts and Translations are still
** problematic.
**
** Since the 'translation' SQL table has a 'translation.transcript_id' field,
** the Translation object retains its corresponding Transcript object, while
** the Transcript has only a weak reference (back) to its Translation object.
**
** To get a Transcript for a Translation, the Transcript is immediately known,
** as the Translation holds already a strong reference to it, while getting
** a Translation for a Transcript requires fetching it via the Translation
** Adaptor.
**
** When fetching a Translation from the database, the Translation Adaptor
** automatically fetches the Transcript via its identifier. In case the
** Transcript is already known (i.e. ensTranslationadaptorFetchByTranscript),
** the Translation Adaptor would fetch another instance of the Transcript.
** To avoid this, the Translation Adaptor cannot go through the Base Adaptor
** function in those cases, as there is no way to propagate the Transcript
** through it, which would be needed for retaining it.
**
** Another variant would be implementing a cache in the Transcript Adaptor, but
** Transcripts can be quite big, especially if they have sequence attached.
** Additionally, since the Transcript class is a sub class of Feature, it has
** a Slice attached. Since Transcripts need to be handled by the cache
** generically, the Transcripts should only have its original Sequence Region
** Slice attached. Transcripts, which need placing on other Slices need
** transfering onto the new Slice (ensTranscriptTransfer). This needed
** implementing in all functions that return cached Transcripts or Features
** in general.
**
** If a similar model is adopted for the relationship between Transcripts
** and Genes, then the Transcript should retain the Gene object. The Gene
** could be immediately reached by a Transcript, while the Transcripts
** require fetching by the Gene via the Transcript Adaptor. This would also
** require a specialised ens TranscriptadaptorFetchAllByGene method, which
** propagates the Gene into the Transcript.
*/

AjBool ensTranslationadaptorFetchByTranscript(EnsPTranslationadaptor tla,
                                              EnsPTranscript transcript,
                                              EnsPTranslation *Ptranslation)
{
    AjPList translations = NULL;

    AjPStr constraint = NULL;

    EnsPTranslation translation = NULL;

    if(!tla)
        return ajFalse;

    if(!transcript)
        return ajFalse;

    if(!Ptranslation)
        return ajFalse;

    constraint = ajFmtStr("translation.transcript_id = %u",
                          ensTranscriptGetIdentifier(transcript));

    translations = ajListNew();

    ensBaseadaptorGenericFetch(tla->Adaptor,
                               constraint,
                               (EnsPAssemblymapper) NULL,
                               (EnsPSlice) NULL,
                               translations);

    if(ajListGetLength(translations) == 0)
    {
        ajDebug("ensTranslationadaptorFetchByTranscript could not get "
                "an Ensembl Translation for Ensembl Transcript %u.\n",
                ensTranscriptGetIdentifier(transcript));

        ensTranscriptTrace(transcript, 1);
    }
    else if(ajListGetLength(translations) == 1)
        ajListPop(translations, (void **) Ptranslation);
    else if(ajListGetLength(translations) > 1)
    {
        ajDebug("ensTranslationadaptorFetchByTranscript got more than one "
                "Ensembl Translation for Ensembl Transcript %u.\n",
                ensTranscriptGetIdentifier(transcript));

        ensTranscriptTrace(transcript, 1);
    }

    while(ajListPop(translations, (void **) &translation))
        ensTranslationDel(&translation);

    ajListFree(&translations);

    ajStrDel(&constraint);

    return ajTrue;
}

#endif




/* @func ensTranslationadaptorFetchAllByExternalName **************************
**
** Fetch all Ensembl Translations via an Ensembl Database Entry name and
** Ensembl External Database name.
** The caller is responsible for deleting the Ensembl Translations before
** deleting the AJAX List.
**
** @param [r] tla [EnsPTranslationadaptor] Ensembl Translation Adaptor
** @param [r] externalname [AjPStr] Ensembl Database Entry name
** @param [r] externaldbname [AjPStr] Ensembl External Database name
** @param [u] translations [AjPList] AJAX List of Ensembl Translations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationadaptorFetchAllByExternalName(
    EnsPTranslationadaptor tla,
    AjPStr externalname,
    AjPStr externaldbname,
    AjPList translations)
{
    ajuint *Pidentifier = NULL;

    AjPList idlist = NULL;

    EnsPDatabaseadaptor dba = NULL;

    EnsPDatabaseentryadaptor dbea = NULL;

    EnsPTranslation translation = NULL;

    if(!tla)
        return ajFalse;

    if(!externalname)
        return ajFalse;

    if(!externaldbname)
        return ajFalse;

    if(!translations)
        return ajFalse;

    dba = ensTranslationadaptorGetDatabaseadaptor(tla);

    dbea = ensRegistryGetDatabaseentryadaptor(dba);

    idlist = ajListNew();

    ensDatabaseentryadaptorFetchAllTranslationIdentifiersByExternalName(
        dbea,
        externalname,
        externaldbname,
        idlist);

    while(ajListPop(idlist, (void **) &Pidentifier))
    {
        ensTranslationadaptorFetchByIdentifier(tla,
                                               *Pidentifier,
                                               &translation);

        ajListPushAppend(translations, (void *) translation);

        AJFREE(Pidentifier);
    }

    ajListFree(&idlist);

    return ajTrue;
}




/* @funcstatic translationAdaptorFetchAllByIdentifiers ************************
**
** Helper function for the generic function to fetch Ensembl Translations by an
** AJAX List of Ensembl Transcripts.
**
** @param [r] tla [const EnsPTranslationadaptor] Ensembl Translation Adaptor
** @param [r] identifiers [AjPStr] Comma-separated list of SQL database-
**                                 internal identifiers used in an IN
**                                 comparison function in a SQL SELECT
**                                 statement.
** @param [r] translations [AjPList] AJAX List of Ensembl Translations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool translationAdaptorFetchAllByIdentifiers(
    const EnsPTranslationadaptor tla,
    AjPStr identifiers,
    AjPList translations)
{
    AjPStr constraint = NULL;

    if(!tla)
        return ajFalse;

    if(!identifiers)
        return ajFalse;

    if(!translations)
        return ajFalse;

    constraint = ajFmtStr("translation.transcript_id IN (%S)", identifiers);

    ensBaseadaptorGenericFetch(tla->Adaptor,
                               constraint,
                               (EnsPAssemblymapper) NULL,
                               (EnsPSlice) NULL,
                               translations);

    ajStrDel(&constraint);

    return ajTrue;
}




/* @func ensTranslationadaptorFetchAllByTranscriptList ************************
**
** Fetch all Ensembl Translations via an AJAX List of Ensembl Transcripts.
** The caller is responsible for deleting the Ensembl Translations before
** deleting the AJAX List.
**
** @param [r] tla [EnsPTranslationadaptor] Ensembl Translation Adaptor
** @param [r] transcripts [AjPList] AJAX List of Ensembl Transcripts
** @param [u] translations [AjPList] AJAX List of Ensembl Translations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
** This function is modelled after ensBaseadaptorFetchAllByIdentifiers, but
** since this function cannot use the primary key, it has to be copied here.
******************************************************************************/

AjBool ensTranslationadaptorFetchAllByTranscriptList(
    EnsPTranslationadaptor tla,
    AjPList transcripts,
    AjPList translations)
{
    register ajuint i = 0;

    AjIList iter = NULL;

    AjPStr identifiers = NULL;

    EnsPTranscript transcript = NULL;

    if(!tla)
        return ajFalse;

    if(!transcripts)
        return ajFalse;

    if(!translations)
        return ajFalse;

    iter = ajListIterNew(transcripts);

    identifiers = ajStrNew();

    while(!ajListIterDone(iter))
    {
        transcript = (EnsPTranscript) ajListIterGet(iter);

        if(!transcript)
            continue;

        ajFmtPrintAppS(&identifiers,
                       "%u, ",
                       ensTranscriptGetIdentifier(transcript));

        i++;

        /* Run the statement if the maximum chunk size is exceed. */

        if(i >= 200)
        {
            /* Remove the last comma and space. */

            ajStrCutEnd(&identifiers, 2);

            translationAdaptorFetchAllByIdentifiers(tla,
                                                    identifiers,
                                                    translations);

            ajStrAssignClear(&identifiers);

            i = 0;
        }
    }

    ajListIterDel(&iter);

    /* Run the final statement. */

    /* Remove the last comma and space. */

    ajStrCutEnd(&identifiers, 2);

    translationAdaptorFetchAllByIdentifiers(tla,
                                            identifiers,
                                            translations);

    ajStrDel(&identifiers);

    return ajTrue;
}




/* @func ensTranslationadaptorFetchAllIdentifiers *****************************
**
** Fetch all SQL database-internal identifiers of Ensembl Translations.
**
** The caller is responsible for deleting the AJAX unsigned integers before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::DBSQL::TranslationAdaptor::list_dbIDs
** @param [u] tla [const EnsPTranslationadaptor] Ensembl Translation Adaptor
** @param [u] identifiers [AjPList] AJAX List of AJAX unsigned integers
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationadaptorFetchAllIdentifiers(
    const EnsPTranslationadaptor tla,
    AjPList identifiers)
{
    AjBool value = AJFALSE;

    AjPStr table = NULL;

    if(!tla)
        return ajFalse;

    if(!identifiers)
        return ajFalse;

    table = ajStrNewC("translation");

    value = ensBaseadaptorFetchAllIdentifiers(tla->Adaptor,
                                              table,
                                              (AjPStr) NULL,
                                              identifiers);

    ajStrDel(&table);

    return value;
}




/* @func ensTranslationadaptorFetchAllStableIdentifiers ***********************
**
** Fetch all stable identifiers of Ensembl Translations.
**
** The caller is responsible for deleting the AJAX Strings before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::DBSQL::TranslationAdaptor::list_stable_ids
** @param [u] tla [EnsPTranslationadaptor] Ensembl Translation Adaptor
** @param [u] identifiers [AjPList] AJAX List of AJAX Strings
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTranslationadaptorFetchAllStableIdentifiers(
    const EnsPTranslationadaptor tla,
    AjPList identifiers)
{
    AjBool value = AJFALSE;

    AjPStr table   = NULL;
    AjPStr primary = NULL;

    if(!tla)
        return ajFalse;

    if(!identifiers)
        return ajFalse;

    table   = ajStrNewC("translation_stable_id");
    primary = ajStrNewC("stable_id");

    value = ensBaseadaptorFetchAllStrings(tla->Adaptor,
                                          table,
                                          primary,
                                          identifiers);

    ajStrDel(&table);
    ajStrDel(&primary);

    return value;
}
