/******************************************************************************
** @source Ensembl Quality Check functions
**
** @author Copyright (C) 1999 Ensembl Developers
** @author Copyright (C) 2006 Michael K. Schuster
** @modified 2009 by Alan Bleasby for incorporation into EMBOSS core
** @version $Revision: 1.14 $
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

#include "ensqc.h"




/* ==================================================================== */
/* ============================ constants ============================= */
/* ==================================================================== */




/* ==================================================================== */
/* ======================== global variables ========================== */
/* ==================================================================== */




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */

/* qcdasfeatureCategory *******************************************************
**
** The Ensembl Quality Check DAS Feature category element is enumerated in
** both, the SQL table definition and the data structure. The following strings
** are used for conversion in database operations and correspond to
** EnsEQcdasfeatureCategory.
**
******************************************************************************/

static const char *qcdasfeatureCategory[] =
{
    (const char *) NULL,
    "unknown",
    "transcript:perfect",
    "transcript:tolerance",
    "transcript:partial'",
    "transcript:missing",
    "transcript",
    "translation:perfect",
    "translation:tolerance",
    "translation:partial",
    "translation:missing",
    "translation",
    (const char *) NULL
};




/* qcdasfeatureType ***********************************************************
**
** The Ensembl Quality Check DAS Feature type element is enumerated in
** both, the SQL table definition and the data structure. The following strings
** are used for conversion in database operations and correspond to
** EnsEQcdasfeatureType.
**
******************************************************************************/

static const char *qcdasfeatureType[] =
{
    (const char *) NULL,
    "unknown",
    "exon:perfect",
    "exon:partial",
    "exon:missing",
    "exon:frameshift",
    "exon:gap",
    "exon",
    (const char *) NULL
};




/* qcvariationClass ***********************************************************
**
** The Ensembl Quality Check Variation class element is enumerated in
** both, the SQL table definition and the data structure. The following strings
** are used for conversion in database operations and correspond to
** EnsEQcvariationClass.
**
******************************************************************************/

static const char *qcvariationClass[] =
{
    (const char *) NULL,
    "none",
    "simple",
    "splice",
    "exon",
    (const char *) NULL
};




/* qcvariationType ************************************************************
**
** The Ensembl Quality Check Variation type element is enumerated in
** both, the SQL table definition and the data structure. The following strings
** are used for conversion in database operations and correspond to
** EnsEQcvariationType.
**
******************************************************************************/

static const char *qcvariationType[] =
{
    (const char *) NULL,
    "none",
    "single",
    "multi",
    (const char *) NULL
};




/* qcvariationState ***********************************************************
**
** The Ensembl Quality Check Variation state element is enumerated in
** both, the SQL table definition and the data structure. The following strings
** are used for conversion in database operations and correspond to
** EnsEQcvariationState.
**
******************************************************************************/

static const char *qcvariationState[] =
{
    (const char *) NULL,
    "none",
    "match",
    "gap",
    "frameshift",
    "5'ss",
    "3'ss",
    "split",
    (const char *) NULL
};




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

static AjBool qcdasfeatureadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                               const AjPStr statement,
                                               EnsPAssemblymapper am,
                                               EnsPSlice slice,
                                               AjPList qcdasfs);

static AjBool qcvariationadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                              const AjPStr statement,
                                              EnsPAssemblymapper am,
                                              EnsPSlice slice,
                                              AjPList qcvs);

static AjBool qcsubmissionadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                               const AjPStr statement,
                                               EnsPAssemblymapper am,
                                               EnsPSlice slice,
                                               AjPList qcss);




/* ==================================================================== */
/* ===================== All functions by section ===================== */
/* ==================================================================== */

/* @filesection ensqc *********************************************************
**
** @nam1rule ens Function belongs to the Ensembl library
**
******************************************************************************/




/* @datasection [EnsPQcdasfeature] QC DAS Feature *****************************
**
** Functions for manipulating Ensembl QC DAS Feature objects
**
** Bio::EnsEMBL::QC::DASFeature CVS Revision:
**
** @nam2rule Qcdasfeature
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl QC DAS Feature by pointer.
** It is the responsibility of the user to first destroy any previous
** QC DAS Feature. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPQcdasfeature]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPQcdasfeature] Ensembl QC DAS Feature
** @argrule Ref object [EnsPQcdasfeature] Ensembl QC DAS Feature
**
** @valrule * [EnsPQcdasfeature] Ensembl QC DAS Feature
**
** @fcategory new
******************************************************************************/




/* @func ensQcdasfeatureNew ***************************************************
**
** Default constructor for an Ensembl QC DAS Feature.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [u] qcdasfa [EnsPQcdasfeatureadaptor] Ensembl QC DAS Feature Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::QC::DASFeature::new
** @param [u] qca [EnsPQcalignment] Ensembl QC Alignment
** @param [u] analysis [EnsPAnalysis] Ensembl Analysis
** @param [u] segment [EnsPQcsequence] Segment Ensembl QC Sequence
** @param [r] segstart [ajuint] Segment start
** @param [r] segend [ajuint] Segment end
** @param [r] segstrand [ajint] Segment strand
** @param [u] feature [EnsPQcsequence] Feature Ensembl QC Sequence
** @param [r] fstart [ajuint] Feature start
** @param [r] fend [ajuint] Feature end
** @param [r] phase [ajint] Phase
** @param [u] category [EnsEQcdasfeatureCategory] Category
** @param [u] type [EnsEQcdasfeatureType] Type
**
** @return [EnsPQcdasfeature] Ensembl QC DAS Feature or NULL
** @@
******************************************************************************/

EnsPQcdasfeature ensQcdasfeatureNew(EnsPQcdasfeatureadaptor qcdasfa,
                                    ajuint identifier,
                                    EnsPQcalignment qca,
                                    EnsPAnalysis analysis,
                                    EnsPQcsequence segment,
                                    ajuint segstart,
                                    ajuint segend,
                                    ajint segstrand,
                                    EnsPQcsequence feature,
                                    ajuint fstart,
                                    ajuint fend,
                                    ajint phase,
                                    EnsEQcdasfeatureCategory category,
                                    EnsEQcdasfeatureType type)
{
    EnsPQcdasfeature qcdasf = NULL;

    if(!qca)
        return NULL;

    if(!analysis)
        return NULL;

    if(!segment)
        return NULL;

    if(!feature)
        return NULL;

    AJNEW0(qcdasf);

    qcdasf->Use = 1;

    qcdasf->Identifier = identifier;

    qcdasf->Adaptor = qcdasfa;

    qcdasf->Qcalignment = ensQcalignmentNewRef(qca);

    qcdasf->Analysis = ensAnalysisNewRef(analysis);

    qcdasf->SegmentSequence = ensQcsequenceNewRef(segment);

    qcdasf->SegmentStart = segstart;

    qcdasf->SegmentEnd = segend;

    qcdasf->SegmentStrand = segstrand;

    qcdasf->FeatureSequence = ensQcsequenceNewRef(feature);

    qcdasf->FeatureStart = fstart;

    qcdasf->FeatureEnd = fend;

    qcdasf->Phase = phase;

    qcdasf->Category = category;

    qcdasf->Type = type;

    return qcdasf;
}




/* @func ensQcdasfeatureNewObj ************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPQcdasfeature] Ensembl QC DAS Feature
**
** @return [EnsPQcdasfeature] Ensembl QC DAS Feature or NULL
** @@
******************************************************************************/

EnsPQcdasfeature ensQcdasfeatureNewObj(const EnsPQcdasfeature object)
{
    EnsPQcdasfeature qcdasf = NULL;

    if(!object)
        return NULL;

    AJNEW0(qcdasf);

    qcdasf->Use = 1;

    qcdasf->Identifier = object->Identifier;

    qcdasf->Adaptor = object->Adaptor;

    if(object->Qcalignment)
        qcdasf->Qcalignment = ensQcalignmentNewRef(object->Qcalignment);

    if(object->Analysis)
        qcdasf->Analysis = ensAnalysisNewRef(object->Analysis);

    if(object->SegmentSequence)
        qcdasf->SegmentSequence = ensQcsequenceNewRef(object->SegmentSequence);

    if(object->FeatureSequence)
        qcdasf->FeatureSequence = ensQcsequenceNewRef(object->FeatureSequence);

    qcdasf->SegmentStart = object->SegmentStart;

    qcdasf->SegmentEnd = object->SegmentEnd;

    qcdasf->SegmentStrand = object->SegmentStrand;

    qcdasf->FeatureStart = object->FeatureStart;

    qcdasf->FeatureEnd = object->FeatureEnd;

    qcdasf->Phase = object->Phase;

    qcdasf->Category = object->Category;

    qcdasf->Type = object->Type;

    return qcdasf;
}




/* @func ensQcdasfeatureNewRef ************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] qcdasf [EnsPQcdasfeature] Ensembl DAS Feature
**
** @return [EnsPQcdasfeature] Ensembl QC DAS Feature or NULL
** @@
******************************************************************************/

EnsPQcdasfeature ensQcdasfeatureNewRef(EnsPQcdasfeature qcdasf)
{
    if(!qcdasf)
        return NULL;

    qcdasf->Use++;

    return qcdasf;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl QC DAS Feature.
**
** @fdata [EnsPQcdasfeature]
** @fnote None
**
** @nam3rule Del Destroy (free) a QC DAS Feature object
**
** @argrule * Pqcdasf [EnsPQcdasfeature*] QC DAS Feature object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensQcdasfeatureDel ***************************************************
**
** Default destructor for an Ensembl QC DAS Feature.
**
** @param [d] Pqcdasf [EnsPQcdasfeature*] Ensembl QC DAS Feature address
**
** @return [void]
** @@
******************************************************************************/

void ensQcdasfeatureDel(EnsPQcdasfeature *Pqcdasf)
{
    EnsPQcdasfeature pthis = NULL;

    if(!Pqcdasf)
        return;

    if(!*Pqcdasf)
        return;

    if(ajDebugTest("ensQcdasfeatureDel"))
    {
        ajDebug("ensQcdasfeatureDel\n"
                "  *Pqcdasf %p\n",
                *Pqcdasf);

        ensQcdasfeatureTrace(*Pqcdasf, 1);
    }

    pthis = *Pqcdasf;

    pthis->Use--;

    if(pthis->Use)
    {
        *Pqcdasf = NULL;

        return;
    }

    ensQcalignmentDel(&pthis->Qcalignment);

    ensAnalysisDel(&pthis->Analysis);

    ensQcsequenceDel(&pthis->SegmentSequence);

    ensQcsequenceDel(&pthis->FeatureSequence);

    AJFREE(pthis);

    *Pqcdasf = NULL;

    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl QC DASFeature object.
**
** @fdata [EnsPQcdasfeature]
** @fnote None
**
** @nam3rule Get Return QC DAS Feature attribute(s)
** @nam4rule GetAdaptor Return the Ensembl DAS Feature Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetQcalignment Return the Ensembl QC Alignment
** @nam4rule GetAnalysis Return the Ensembl Analysis
** @nam4rule GetSegmentSequence Return the segment Ensembl QC Sequence
** @nam4rule GetFeatureSequence Return the feature Ensembl QC Sequence
** @nam4rule GetSegmentStart Return the segment start
** @nam4rule GetSegmentEnd Return the segment end
** @nam4rule GetSegmentStrand Return the segment strand
** @nam4rule GetFeatureStart Return the feature start
** @nam4rule GetFeatureEnd Return the feature end
** @nam4rule GetPhase Return the phase
** @nam4rule GetCategory Return the category
** @nam4rule GetType Return the type
**
** @argrule * qcdasf [const EnsPQcdasfeature] QC DAS Feature
**
** @valrule Adaptor [EnsPQcdasfeatureadaptor] Ensembl QC DAS Feature Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Qcalignment [EnsPQcalignment] Ensembl QC Alignment
** @valrule Analysis [EnsPAnalysis] Ensembl Analysis
** @valrule SegmentSequence [EnsPQcsequence] Segment Ensembl QC Sequence
** @valrule FeatureSequence [EnsPQcsequence] Feature Ensembl QC Sequence
** @valrule SegmentStart [ajuint] Segment start
** @valrule SegmentEnd [ajuint] Segment end
** @valrule SegmentStrand [ajint] Segment strand
** @valrule FeatureStart [ajuint] Feature start
** @valrule FeatureEnd [ajuint] Feature end
** @valrule Phase [ajint] Phase
** @valrule Category [EnsEQcdasfeatureCategory] Category
** @valrule Type [EnsEQcdasfeatureType] Type
**
** @fcategory use
******************************************************************************/




/* @func ensQcdasfeatureGetAdaptor ********************************************
**
** Get the Ensembl DAS Feature Adaptor element of an Ensembl DAS Feature.
**
** @param [r] qcdasf [const EnsPQcdasfeature] Ensembl QC DAS Feature
**
** @return [EnsPQcdasfeatureadaptor] Ensembl QC DAS Feature Adaptor
** @@
******************************************************************************/

EnsPQcdasfeatureadaptor ensQcdasfeatureGetAdaptor(
    const EnsPQcdasfeature qcdasf)
{
    if(!qcdasf)
        return NULL;

    return qcdasf->Adaptor;
}




/* @func ensQcdasfeatureGetIdentifier *****************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQcdasfeature] Ensembl QC DAS Feature
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensQcdasfeatureGetIdentifier(
    const EnsPQcdasfeature qcdasf)
{
    if(!qcdasf)
        return 0;

    return qcdasf->Identifier;
}




/* @func ensQcdasfeatureGetQcalignment ****************************************
**
** Get the Ensembl QC Alignment element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQcdasfeature] Ensembl QC DAS Feature
**
** @return [EnsPQcalignment] Ensembl QC Alignment
** @@
******************************************************************************/

EnsPQcalignment ensQcdasfeatureGetQcalignment(
    const EnsPQcdasfeature qcdasf)
{
    if(!qcdasf)
        return NULL;

    return qcdasf->Qcalignment;
}




/* @func ensQcdasfeatureGetAnalysis *******************************************
**
** Get the Ensembl Analysis element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQcdasfeature] Ensembl QC DAS Feature
**
** @return [EnsPAnalysis] Ensembl Analysis
** @@
******************************************************************************/

EnsPAnalysis ensQcdasfeatureGetAnalysis(
    const EnsPQcdasfeature qcdasf)
{
    if(!qcdasf)
        return NULL;

    return qcdasf->Analysis;
}




/* @func ensQcdasfeatureGetSegmentSequence ************************************
**
** Get the segment Ensembl QC Sequence element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQcdasfeature] Ensembl QC DAS Feature
**
** @return [EnsPQcsequence] Ensembl QC Sequence
** @@
******************************************************************************/

EnsPQcsequence ensQcdasfeatureGetSegmentSequence(
    const EnsPQcdasfeature qcdasf)
{
    if(!qcdasf)
        return NULL;

    return qcdasf->SegmentSequence;
}




/* @func ensQcdasfeatureGetFeatureSequence ************************************
**
** Get the feature Ensembl QC Sequence element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQcdasfeature] Ensembl QC DAS Feature
**
** @return [EnsPQcsequence] Ensembl QC Sequence
** @@
******************************************************************************/

EnsPQcsequence ensQcdasfeatureGetFeatureSequence(
    const EnsPQcdasfeature qcdasf)
{
    if(!qcdasf)
        return NULL;

    return qcdasf->FeatureSequence;
}




/* @func ensQcdasfeatureGetSegmentStart ***************************************
**
** Get the segment start element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQcdasfeature] Ensembl QC DAS Feature
**
** @return [ajuint] Segment start
** @@
******************************************************************************/

ajuint ensQcdasfeatureGetSegmentStart(
    const EnsPQcdasfeature qcdasf)
{
    if(!qcdasf)
        return 0;

    return qcdasf->SegmentStart;
}




/* @func ensQcdasfeatureGetSegmentEnd *****************************************
**
** Get the segment end element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQcdasfeature] Ensembl QC DAS Feature
**
** @return [ajuint] Segment end
** @@
******************************************************************************/

ajuint ensQcdasfeatureGetSegmentEnd(
    const EnsPQcdasfeature qcdasf)
{
    if(!qcdasf)
        return 0;

    return qcdasf->SegmentEnd;
}




/* @func ensQcdasfeatureGetSegmentStrand **************************************
**
** Get the segment strand element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQcdasfeature] Ensembl QC DAS Feature
**
** @return [ajint] Segment strand
** @@
******************************************************************************/

ajint ensQcdasfeatureGetSegmentStrand(
    const EnsPQcdasfeature qcdasf)
{
    if(!qcdasf)
        return 0;

    return qcdasf->SegmentStrand;
}




/* @func ensQcdasfeatureGetFeatureStart ***************************************
**
** Get the feature start element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQcdasfeature] Ensembl QC DAS Feature
**
** @return [ajuint] Feature start
** @@
******************************************************************************/

ajuint ensQcdasfeatureGetFeatureStart(
    const EnsPQcdasfeature qcdasf)
{
    if(!qcdasf)
        return 0;

    return qcdasf->FeatureStart;
}




/* @func ensQcdasfeatureGetFeatureEnd ***************************************
**
** Get the feature end element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQcdasfeature] Ensembl QC DAS Feature
**
** @return [ajuint] Feature end
** @@
******************************************************************************/

ajuint ensQcdasfeatureGetFeatureEnd(
    const EnsPQcdasfeature qcdasf)
{
    if(!qcdasf)
        return 0;

    return qcdasf->FeatureEnd;
}




/* @func ensQcdasfeatureGetPhase **********************************************
**
** Get the phase element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQcdasfeature] Ensembl QC DAS Feature
**
** @return [ajint] Phase
** @@
******************************************************************************/

ajint ensQcdasfeatureGetPhase(
    const EnsPQcdasfeature qcdasf)
{
    if(!qcdasf)
        return 0;

    return qcdasf->Phase;
}




/* @func ensQcdasfeatureGetCategory *******************************************
**
** Get the category element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQcdasfeature] Ensembl QC DAS Feature
**
** @return [EnsEQcdasfeatureCategory] Category or ensEQcdasfeatureCategoryNULL
** @@
******************************************************************************/

EnsEQcdasfeatureCategory ensQcdasfeatureGetCategory(
    const EnsPQcdasfeature qcdasf)
{
    if(!qcdasf)
        return ensEQcdasfeatureCategoryNULL;

    return qcdasf->Category;
}




/* @func ensQcdasfeatureGetType ***********************************************
**
** Get the type element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQcdasfeature] Ensembl QC DAS Feature
**
** @return [EnsEQcdasfeatureType] Type or ensEQcdasfeatureTypeNULL
** @@
******************************************************************************/

EnsEQcdasfeatureType ensQcdasfeatureGetType(
    const EnsPQcdasfeature qcdasf)
{
    if(!qcdasf)
        return ensEQcdasfeatureTypeNULL;

    return qcdasf->Type;
}




/* @section modifiers *********************************************************
**
** Functions for assigning elements of an Ensembl QC DAS Feature object.
**
** @fdata [EnsPQcdasfeature]
** @fnote None
**
** @nam3rule Set Set one element of a QC DAS Feature
** @nam4rule SetAdaptor Set the Ensembl QC DAS Feature Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetQcalignment Set the Ensembl QC Alignment
** @nam4rule SetAnalysis Set the Ensembl Analysis
** @nam4rule SetSegmentSequence Set the segment Ensembl QC Sequence
** @nam4rule SetFeatureSequence Set the feature Ensembl QC Sequence
** @nam4rule SetSegmentStart Set the segment start
** @nam4rule SetSegmentEnd Set the segment end
** @nam4rule SetSegmentStrand Set the segment strand
** @nam4rule SetFeatureStart Set the feature start
** @nam4rule SetFeatureEnd Set the feature end
** @nam4rule SetPhase Set the phase
** @nam4rule SetCategory Set the category
** @nam4rule SetType Set the type
**
** @argrule * qcdasf [EnsPQcdasfeature] Ensembl QC DAS Feature
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensQcdasfeatureSetAdaptor ********************************************
**
** Set the Ensembl QC DAS Feature Adaptor element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQcdasfeature] Ensembl QC DAS Feature
** @param [r] qcdasfa [EnsPQcdasfeatureadaptor] Ensembl QC DAS Feature Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureSetAdaptor(EnsPQcdasfeature qcdasf,
                                 EnsPQcdasfeatureadaptor qcdasfa)
{
    if(!qcdasf)
        return ajFalse;

    qcdasf->Adaptor = qcdasfa;

    return ajTrue;
}




/* @func ensQcdasfeatureSetIdentifier *****************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQcdasfeature] Ensembl QC DAS Feature
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureSetIdentifier(EnsPQcdasfeature qcdasf,
                                    ajuint identifier)
{
    if(!qcdasf)
        return ajFalse;

    qcdasf->Identifier = identifier;

    return ajTrue;
}




/* @func ensQcdasfeatureSetQcalignment ****************************************
**
** Set the Ensembl QC Alignment element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQcdasfeature] Ensembl QC DAS Feature
** @param [u] qca [EnsPQcalignment] Ensembl QC Alignment
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureSetQcalignment(EnsPQcdasfeature qcdasf,
                                     EnsPQcalignment qca)
{
    if(!qcdasf)
        return ajFalse;

    ensQcalignmentDel(&qcdasf->Qcalignment);

    qcdasf->Qcalignment = ensQcalignmentNewRef(qca);

    return ajTrue;
}




/* @func ensQcdasfeatureSetAnalysis *******************************************
**
** Set the Ensembl Analysis element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQcdasfeature] Ensembl QC DAS Feature
** @param [u] analysis [EnsPAnalysis] Ensembl Analysis
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureSetAnalysis(EnsPQcdasfeature qcdasf,
                                  EnsPAnalysis analysis)
{
    if(!qcdasf)
        return ajFalse;

    ensAnalysisDel(&qcdasf->Analysis);

    qcdasf->Analysis = ensAnalysisNewRef(analysis);

    return ajTrue;
}




/* @func ensQcdasfeatureSetSegmentSequence ************************************
**
** Set the segment Ensembl QC Sequence element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQcdasfeature] Ensembl QC DAS Feature
** @param [u] qcs [EnsPQcsequence] Ensembl QC Sequence
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureSetSegmentSequence(EnsPQcdasfeature qcdasf,
                                         EnsPQcsequence qcs)
{
    if(!qcdasf)
        return ajFalse;

    ensQcsequenceDel(&qcdasf->SegmentSequence);

    qcdasf->SegmentSequence = ensQcsequenceNewRef(qcs);

    return ajTrue;
}




/* @func ensQcdasfeatureSetFeatureSequence ************************************
**
** Set the feature Ensembl QC Sequence element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQcdasfeature] Ensembl QC DAS Feature
** @param [u] qcs [EnsPQcsequence] Ensembl QC Sequence
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureSetFeatureSequence(EnsPQcdasfeature qcdasf,
                                         EnsPQcsequence qcs)
{
    if(!qcdasf)
        return ajFalse;

    ensQcsequenceDel(&qcdasf->FeatureSequence);

    qcdasf->FeatureSequence = ensQcsequenceNewRef(qcs);

    return ajTrue;
}




/* @func ensQcdasfeatureSetSegmentStart ***************************************
**
** Set the segment start element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQcdasfeature] Ensembl QC DAS Feature
** @param [r] start [ajuint] Segment start
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureSetSegmentStart(EnsPQcdasfeature qcdasf,
                                      ajuint start)
{
    if(!qcdasf)
        return ajFalse;

    qcdasf->SegmentStart = start;

    return ajTrue;
}




/* @func ensQcdasfeatureSetSegmentEnd *****************************************
**
** Set the segment end element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQcdasfeature] Ensembl QC DAS Feature
** @param [r] end [ajuint] Segment end
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureSetSegmentEnd(EnsPQcdasfeature qcdasf,
                                    ajuint end)
{
    if(!qcdasf)
        return ajFalse;

    qcdasf->SegmentEnd = end;

    return ajTrue;
}




/* @func ensQcdasfeatureSetSegmentStrand **************************************
**
** Set the segment strand element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQcdasfeature] Ensembl QC DAS Feature
** @param [r] strand [ajint] Segment strand
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureSetSegmentStrand(EnsPQcdasfeature qcdasf,
                                       ajint strand)
{
    if(!qcdasf)
        return ajFalse;

    qcdasf->SegmentStrand = strand;

    return ajTrue;
}




/* @func ensQcdasfeatureSetFeatureStart ***************************************
**
** Set the feature start element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQcdasfeature] Ensembl QC DAS Feature
** @param [r] start [ajuint] Feature start
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureSetFeatureStart(EnsPQcdasfeature qcdasf,
                                      ajuint start)
{
    if(!qcdasf)
        return ajFalse;

    qcdasf->FeatureStart = start;

    return ajTrue;
}




/* @func ensQcdasfeatureSetFeatureEnd *****************************************
**
** Set the feature end element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQcdasfeature] Ensembl QC DAS Feature
** @param [r] end [ajuint] Feature end
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureSetFeatureEnd(EnsPQcdasfeature qcdasf,
                                    ajuint end)
{
    if(!qcdasf)
        return ajFalse;

    qcdasf->FeatureEnd = end;

    return ajTrue;
}




/* @func ensQcdasfeatureSetPhase **********************************************
**
** Set the phase element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQcdasfeature] Ensembl QC DAS Feature
** @param [r] phase [ajint] Phase
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureSetPhase(EnsPQcdasfeature qcdasf,
                               ajint phase)
{
    if(!qcdasf)
        return ajFalse;

    qcdasf->Phase = phase;

    return ajTrue;
}




/* @func ensQcdasfeatureSetCategory *******************************************
**
** Set the category element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQcdasfeature] Ensembl QC DAS Feature
** @param [r] category [EnsEQcdasfeatureCategory] Category
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureSetCategory(EnsPQcdasfeature qcdasf,
                                  EnsEQcdasfeatureCategory category)
{
    if(!qcdasf)
        return ajFalse;

    qcdasf->Category = category;

    return ajTrue;
}




/* @func ensQcdasfeatureSetType ***********************************************
**
** Set the type element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQcdasfeature] Ensembl QC DAS Feature
** @param [r] type [EnsEQcdasfeatureType] Type
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureSetType(EnsPQcdasfeature qcdasf,
                              EnsEQcdasfeatureType type)
{
    if(!qcdasf)
        return ajFalse;

    qcdasf->Type = type;

    return ajTrue;
}




/* @func ensQcdasfeatureGetMemsize ********************************************
**
** Get the memory size in bytes of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQcdasfeature] Ensembl QC DAS Feature
**
** @return [ajulong] Memory size
** @@
******************************************************************************/

ajulong ensQcdasfeatureGetMemsize(const EnsPQcdasfeature qcdasf)
{
    ajulong size = 0;

    if(!qcdasf)
        return 0;

    size += sizeof (EnsOQcdasfeature);

    size += ensAnalysisGetMemsize(qcdasf->Analysis);

    size += ensQcalignmentGetMemsize(qcdasf->Qcalignment);

    size += ensQcsequenceGetMemsize(qcdasf->SegmentSequence);

    size += ensQcsequenceGetMemsize(qcdasf->FeatureSequence);

    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl QC DAS Feature object.
**
** @fdata [EnsPQcdasfeature]
** @nam3rule Trace Report Ensembl QC DAS Feature elements to debug file
**
** @argrule Trace qcdasf [const EnsPQcdasfeature] Ensembl QC DAS Feature
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensQcdasfeatureTrace *************************************************
**
** Trace an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQcdasfeature] Ensembl QC DAS Feature
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureTrace(const EnsPQcdasfeature qcdasf, ajuint level)
{
    AjPStr indent = NULL;

    if(!qcdasf)
        return ajFalse;

    indent = ajStrNew();

    ajStrAppendCountK(&indent, ' ', level * 2);

    ajDebug("%SensQcdasfeatureTrace %p\n"
            "%S  Use %u\n"
            "%S  Identifier %u\n"
            "%S  Adaptor %p\n"
            "%S  Qcalignment %p\n"
            "%S  Analysis %p\n"
            "%S  SegmentSequence %p\n"
            "%S  FeatureSequence %p\n"
            "%S  SegmentStart %u\n"
            "%S  SegmentEnd %u\n"
            "%S  SegmentStrand %d\n"
            "%S  FeatureStart %u\n"
            "%S  FeatureEnd %u\n"
            "%S  Phase %d\n"
            "%S  Category '%s'\n"
            "%S  Type '%s'\n",
            indent, qcdasf,
            indent, qcdasf->Use,
            indent, qcdasf->Identifier,
            indent, qcdasf->Adaptor,
            indent, qcdasf->Qcalignment,
            indent, qcdasf->Analysis,
            indent, qcdasf->SegmentSequence,
            indent, qcdasf->FeatureSequence,
            indent, qcdasf->SegmentStart,
            indent, qcdasf->SegmentEnd,
            indent, qcdasf->SegmentStrand,
            indent, qcdasf->FeatureStart,
            indent, qcdasf->FeatureEnd,
            indent, qcdasf->Phase,
            indent, ensQcdasfeatureCategoryToChar(qcdasf->Category),
            indent, ensQcdasfeatureTypeToChar(qcdasf->Type));

    ensAnalysisTrace(qcdasf->Analysis, 1);

    ensQcalignmentTrace(qcdasf->Qcalignment, 1);

    ensQcsequenceTrace(qcdasf->SegmentSequence, 1);

    ensQcsequenceTrace(qcdasf->FeatureSequence, 1);

    ajStrDel(&indent);

    return ajTrue;
}




/* @func ensQcdasfeatureCategoryFromStr ***************************************
**
** Convert an AJAX String into an Ensembl QC DAS Feature category element.
**
** @param [r] category [const AjPStr] Category string
**
** @return [EnsEQcdasfeatureCategory] Ensembl QC DAS Feature category or
**                                    ensEQcdasfeatureCategoryNULL
** @@
******************************************************************************/

EnsEQcdasfeatureCategory ensQcdasfeatureCategoryFromStr(const AjPStr category)
{
    register EnsEQcdasfeatureCategory i = ensEQcdasfeatureCategoryNULL;

    EnsEQcdasfeatureCategory ecategory = ensEQcdasfeatureCategoryNULL;

    for(i = ensEQcdasfeatureCategoryUnknown; qcdasfeatureCategory[i]; i++)
        if(ajStrMatchCaseC(category, qcdasfeatureCategory[i]))
            ecategory = i;

    if(!ecategory)
        ajDebug("ensQcdasfeatureCategoryFromStr encountered "
                "unexpected string '%S'.\n", category);

    return ecategory;
}




/* @func ensQcdasfeatureTypeFromStr *******************************************
**
** Convert an AJAX String into an Ensembl QC DAS Feature type element.
**
** @param [r] type [const AjPStr] Type string
**
** @return [EnsEQcdasfeatureType] Ensembl QC DAS Feature type or
**                                ensEQcdasfeatureTypeNULL
** @@
******************************************************************************/

EnsEQcdasfeatureType ensQcdasfeatureTypeFromStr(const AjPStr type)
{
    register EnsEQcdasfeatureType i = ensEQcdasfeatureTypeNULL;

    EnsEQcdasfeatureType etype = ensEQcdasfeatureTypeNULL;

    for(i = ensEQcdasfeatureTypeUnknown; qcdasfeatureType[i]; i++)
        if(ajStrMatchCaseC(type, qcdasfeatureType[i]))
            etype = i;

    if(!etype)
        ajDebug("ensQcdasfeatureTypeFromStr encountered "
                "unexpected string '%S'.\n", type);

    return etype;
}




/* @func ensQcdasfeatureCategoryToChar ****************************************
**
** Convert an Ensembl QC DAS Feature category element into a
** C-type (char*) string.
**
** @param [r] category [EnsEQcdasfeatureCategory] Ensembl QC DAS Feature
**                                                category
**
** @return [const char*] Ensembl QC DAS Feature category
**                       C-type (char*) string
** @@
******************************************************************************/

const char* ensQcdasfeatureCategoryToChar(EnsEQcdasfeatureCategory category)
{
    register EnsEQcdasfeatureCategory i = ensEQcdasfeatureCategoryNULL;

    if(!category)
        return NULL;

    for(i = ensEQcdasfeatureCategoryUnknown;
        qcdasfeatureCategory[i] && (i < category);
        i++);

    if(!qcdasfeatureCategory[i])
        ajDebug("ensQcdasfeatureCategoryToChar encountered an "
                "out of boundary error on group %d.\n", category);

    return qcdasfeatureCategory[i];
}




/* @func ensQcdasfeatureTypeToChar ********************************************
**
** Convert an Ensembl QC DAS Feature type element into a
** C-type (char*) string.
**
** @param [r] type [EnsEQcdasfeatureType] Ensembl QC DAS Feature type
**
** @return [const char*] Ensembl QC DAS Feature type
**                       C-type (char*) string
** @@
******************************************************************************/

const char* ensQcdasfeatureTypeToChar(EnsEQcdasfeatureType type)
{
    register EnsEQcdasfeatureType i = ensEQcdasfeatureTypeNULL;

    if(!type)
        return NULL;

    for(i = ensEQcdasfeatureTypeUnknown;
        qcdasfeatureType[i] && (i < type);
        i++);

    if(!qcdasfeatureType[i])
        ajDebug("ensQcdasfeatureTypeToChar encountered an "
                "out of boundary error on group %d.\n", type);

    return qcdasfeatureType[i];
}




/* @datasection [EnsPQcdasfeatureadaptor] QC DAS Feature Adaptor **************
**
** Functions for manipulating Ensembl QC DAS Feature Adaptor objects
**
** Bio::EnsEMBL::QC::DBSQL::DASFeatureadaptor CVS Revision:
**
** @nam2rule Qcdasfeatureadaptor
**
******************************************************************************/

static const char *qcdasfeatureadaptorTables[] =
{
    "das_feature",
    (const char *) NULL
};




static const char *qcdasfeatureadaptorColumns[] =
{
    "das_feature.das_feature_id",
    "das_feature.analysis_id",
    "das_feature.alignment_id",
    "das_feature.feature_id",
    "das_feature.feature_start",
    "das_feature.feature_end",
    "das_feature.segment_id",
    "das_feature.segment_start",
    "das_feature.segment_end",
    "das_feature.segment_strand",
    "das_feature.phase",
    "das_feature.category",
    "das_feature.type",
    (const char *) NULL
};




static EnsOBaseadaptorLeftJoin qcdasfeatureadaptorLeftJoin[] =
{
    {(const char*) NULL, (const char*) NULL}
};




static const char *qcdasfeatureadaptorDefaultCondition =
    (const char*) NULL;




static const char *qcdasfeatureadaptorFinalCondition =
    (const char *) NULL;




/* @funcstatic qcdasfeatureadaptorFetchAllBySQL *******************************
**
** Run a SQL statement against an Ensembl Database Adaptor and consolidate the
** results into an AJAX List of Ensembl QC DAS Feature objects.
**
** @param [u] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [uN] am [EnsPAssemblymapper] Ensembl Assembly Mapper
** @param [uN] slice [EnsPSlice] Ensembl Slice
** @param [u] qcdasfs [AjPList] AJAX List of Ensembl QC DAS Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool qcdasfeatureadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                               const AjPStr statement,
                                               EnsPAssemblymapper am,
                                               EnsPSlice slice,
                                               AjPList qcdasfs)
{
    ajint tstrand = 0;
    ajint phase   = 0;

    ajuint identifier  = 0;
    ajuint analysisid  = 0;
    ajuint alignmentid = 0;
    ajuint qsid   = 0;
    ajuint qstart = 0;
    ajuint qend   = 0;
    ajuint tsid   = 0;
    ajuint tstart = 0;
    ajuint tend   = 0;

    EnsEQcdasfeatureCategory ecategory = ensEQcdasfeatureCategoryNULL;
    EnsEQcdasfeatureType etype         = ensEQcdasfeatureTypeNULL;

    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;

    AjPStr category = NULL;
    AjPStr type     = NULL;

    EnsPAnalysis analysis  = NULL;
    EnsPAnalysisadaptor aa = NULL;

    EnsPQcalignment qca         = NULL;
    EnsPQcalignmentadaptor qcaa = NULL;

    EnsPQcdasfeature qcdasf         = NULL;
    EnsPQcdasfeatureadaptor qcdasfa = NULL;

    EnsPQcsequence qsequence   = NULL;
    EnsPQcsequence tsequence   = NULL;
    EnsPQcsequenceadaptor qcsa = NULL;

    if(ajDebugTest("qcdasfeatureadaptorFetchAllBySQL"))
        ajDebug("qcdasfeatureadaptorFetchAllBySQL\n"
                "  dba %p\n"
                "  statement %p\n"
                "  am %p\n"
                "  slice %p\n"
                "  qcdasfs %p\n",
                dba,
                statement,
                am,
                slice,
                qcdasfs);

    if(!dba)
        return ajFalse;

    if(!statement)
        return ajFalse;

    if(!qcdasfs)
        return ajFalse;

    aa = ensRegistryGetAnalysisadaptor(dba);

    qcaa = ensRegistryGetQcalignmentadaptor(dba);

    qcdasfa = ensRegistryGetQcdasfeatureadaptor(dba);

    qcsa = ensRegistryGetQcsequenceadaptor(dba);

    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);

    sqli = ajSqlrowiterNew(sqls);

    while(!ajSqlrowiterDone(sqli))
    {
        identifier  = 0;
        analysisid  = 0;
        alignmentid = 0;
        qsid        = 0;
        qstart      = 0;
        qend        = 0;
        tsid        = 0;
        tstart      = 0;
        tend        = 0;
        tstrand     = 0;
        phase       = 0;
        category    = ajStrNew();
        type        = ajStrNew();
        ecategory   = ensEQcdasfeatureCategoryNULL;
        etype       = ensEQcdasfeatureTypeNULL;

        sqlr = ajSqlrowiterGet(sqli);

        ajSqlcolumnToUint(sqlr, &identifier);
        ajSqlcolumnToUint(sqlr, &analysisid);
        ajSqlcolumnToUint(sqlr, &alignmentid);
        ajSqlcolumnToUint(sqlr, &qsid);
        ajSqlcolumnToUint(sqlr, &qstart);
        ajSqlcolumnToUint(sqlr, &qend);
        ajSqlcolumnToUint(sqlr, &tsid);
        ajSqlcolumnToUint(sqlr, &tstart);
        ajSqlcolumnToUint(sqlr, &tend);
        ajSqlcolumnToInt(sqlr, &tstrand);
        ajSqlcolumnToInt(sqlr, &phase);
        ajSqlcolumnToStr(sqlr, &category);
        ajSqlcolumnToStr(sqlr, &type);

        ensAnalysisadaptorFetchByIdentifier(aa, analysisid, &analysis);

        ensQcalignmentadaptorFetchByIdentifier(qcaa, alignmentid, &qca);

        ensQcsequenceadaptorFetchByIdentifier(qcsa, qsid, &qsequence);

        ensQcsequenceadaptorFetchByIdentifier(qcsa, tsid, &tsequence);

        ecategory = ensQcdasfeatureCategoryFromStr(category);

        etype = ensQcdasfeatureTypeFromStr(type);

        qcdasf = ensQcdasfeatureNew(qcdasfa,
                                    identifier,
                                    qca,
                                    analysis,
                                    qsequence,
                                    qstart,
                                    qend,
                                    tstrand,
                                    tsequence,
                                    tstart,
                                    tend,
                                    phase,
                                    ecategory,
                                    etype);

        ajListPushAppend(qcdasfs, (void *) qcdasf);

        ensQcsequenceDel(&qsequence);
        ensQcsequenceDel(&tsequence);

        ensAnalysisDel(&analysis);

        ensQcalignmentDel(&qca);

        ajStrDel(&category);
        ajStrDel(&type);
    }

    ajSqlrowiterDel(&sqli);

    ensDatabaseadaptorSqlstatementDel(dba, &sqls);

    return ajTrue;
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl QC DAS Feature Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** QC DAS Feature Adaptor. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPQcdasfeatureadaptor]
** @fnote None
**
** @nam3rule New Constructor
**
** @argrule New dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @valrule * [EnsPQcdasfeatureadaptor] Ensembl QC DAS Feature Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensQcdasfeatureadaptorNew ********************************************
**
** Default constructor for an Ensembl QC DAS Feature Adaptor.
**
** Ensembl Object Adaptors are singleton objects in the sense that a single
** instance of an Ensembl Object Adaptor connected to a particular database is
** sufficient to instantiate any number of Ensembl Objects from the database.
** Each Ensembl Object will have a weak reference to the Object Adaptor that
** instantiated it. Therefore, Ensembl Object Adaptors should not be
** instantiated directly, but rather obtained from the Ensembl Registry,
** which will in turn call this function if neccessary.
**
** @see ensRegistryGetDatabaseadaptor
** @see ensRegistryGetQcdasfeatureadaptor
**
** @param [u] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPQcdasfeatureadaptor] Ensembl QC DAS Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPQcdasfeatureadaptor ensQcdasfeatureadaptorNew(
    EnsPDatabaseadaptor dba)
{
    if(!dba)
        return NULL;

    return ensBaseadaptorNew(
        dba,
        qcdasfeatureadaptorTables,
        qcdasfeatureadaptorColumns,
        qcdasfeatureadaptorLeftJoin,
        qcdasfeatureadaptorDefaultCondition,
        qcdasfeatureadaptorFinalCondition,
        qcdasfeatureadaptorFetchAllBySQL);
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl QC DAS Feature Adaptor.
**
** @fdata [EnsPQcdasfeatureadaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) a QC DAS Feature Adaptor object
**
** @argrule * Pqcdasfa [EnsPQcdasfeatureadaptor*] QC DAS Feature Adaptor
**                                                object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensQcdasfeatureadaptorDel ********************************************
**
** Default destructor for an Ensembl QC DAS Feature Adaptor.
**
** Ensembl Object Adaptors are singleton objects that are registered in the
** Ensembl Registry and weakly referenced by Ensembl Objects that have been
** instantiated by it. Therefore, Ensembl Object Adaptors should never be
** destroyed directly. Upon exit, the Ensembl Registry will call this function
** if required.
**
** @param [d] Pqcdasfa [EnsPQcdasfeatureadaptor*] Ensembl QC DAS Feature
**                                                Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensQcdasfeatureadaptorDel(EnsPQcdasfeatureadaptor *Pqcdasfa)
{
    if(!Pqcdasfa)
        return;

    if(!*Pqcdasfa)
        return;

    if(ajDebugTest("ensQcdasfeatureadaptorDel"))
        ajDebug("ensQcdasfeatureadaptorDel\n"
                "  *Pqcdasfa %p\n",
                *Pqcdasfa);

    ensBaseadaptorDel(Pqcdasfa);

    return;
}




/* @func ensQcdasfeatureadaptorFetchByIdentifier ******************************
**
** Fetch an Ensembl QC DAS Feature via its SQL database-internal identifier.
** The caller is responsible for deleting the Ensembl QC DAS Feature.
**
** @param [u] qcdasfa [EnsPQcdasfeatureadaptor] Ensembl QC DAS Feature Adaptor
** @param [r] identifier [ajuint] SQL database-internal QC DAS Feature
**                                identifier
** @param [wP] Pqcdasf [EnsPQcdasfeature*] Ensembl QC DAS Feature address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureadaptorFetchByIdentifier(
    EnsPQcdasfeatureadaptor qcdasfa,
    ajuint identifier,
    EnsPQcdasfeature *Pqcdasf)
{
    if(!qcdasfa)
        return ajFalse;

    if(!identifier)
        return ajFalse;

    if(!Pqcdasf)
        return ajFalse;

    *Pqcdasf = (EnsPQcdasfeature)
        ensBaseadaptorFetchByIdentifier(qcdasfa, identifier);

    return ajTrue;
}




/* @func ensQcdasfeatureadaptorFetchAllByQcalignment **************************
**
** Fetch all Ensembl QC DAS Features by an Ensembl QC Alignment.
** The caller is responsible for deleting the Ensembl QC DAS Features
** before deleting the AJAX List.
**
** @param [u] qcdasfa [EnsPQcdasfeatureadaptor] Ensembl QC DAS Feature Adaptor
** @param [r] qca [const EnsPQcalignment] Ensembl QC Alignment
** @param [u] qcdasfs [AjPList] AJAX List of Ensembl QC DAS Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureadaptorFetchAllByQcalignment(
    EnsPQcdasfeatureadaptor qcdasfa,
    const EnsPQcalignment qca,
    AjPList qcdasfs)
{
    AjPStr constraint = NULL;

    if(!qcdasfa)
        return ajFalse;

    if(!qca)
        return ajFalse;

    if(!qcdasfs)
        return ajFalse;

    constraint = ajFmtStr("das_feature.alignment_id = %u",
                          ensQcalignmentGetIdentifier(qca));

    ensBaseadaptorGenericFetch(qcdasfa,
                               constraint,
                               (EnsPAssemblymapper) NULL,
                               (EnsPSlice) NULL,
                               qcdasfs);

    ajStrDel(&constraint);

    return ajTrue;
}




/* @func ensQcdasfeatureadaptorFetchAllByFeature ******************************
**
** Fetch all Ensembl QC DAS Features by an Ensembl QC Sequence representing an
** Ensembl QC DAS Feature feature.
** The caller is responsible for deleting the Ensembl QC DAS Features
** before deleting the AJAX List.
**
** @param [u] qcdasfa [EnsPQcdasfeatureadaptor] Ensembl QC DAS Feature Adaptor
** @param [rN] analysis [const EnsPAnalysis] Ensembl Analysis
** @param [r] feature [const EnsPQcsequence] Feature Ensembl QC Sequence
** @param [u] qcdasfs [AjPList] AJAX List of Ensembl QC DAS Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureadaptorFetchAllByFeature(
    EnsPQcdasfeatureadaptor qcdasfa,
    const EnsPAnalysis analysis,
    const EnsPQcsequence feature,
    AjPList qcdasfs)
{
    AjPStr constraint = NULL;

    if(!qcdasfa)
        return ajFalse;

    if(!feature)
        return ajFalse;

    if(!qcdasfs)
        return ajFalse;

    constraint = ajFmtStr("das_feature.feature_id = %u",
                          ensQcsequenceGetIdentifier(feature));

    if(analysis)
        ajFmtPrintAppS(&constraint,
                       " AND das_feature.analysis_id = %u",
                       ensAnalysisGetIdentifier(analysis));

    ensBaseadaptorGenericFetch(qcdasfa,
                               constraint,
                               (EnsPAssemblymapper) NULL,
                               (EnsPSlice) NULL,
                               qcdasfs);

    ajStrDel(&constraint);

    return ajTrue;
}




/* @func ensQcdasfeatureadaptorFetchAllBySegment ******************************
**
** Fetch all Ensembl QC DAS Features by an Ensembl QC Sequence representing an
** Ensembl QC DAS Feature segment.
** The caller is responsible for deleting the Ensembl QC DAS Features
** before deleting the AJAX List.
**
** @param [u] qcdasfa [EnsPQcdasfeatureadaptor] Ensembl QC DAS Feature Adaptor
** @param [rN] analysis [const EnsPAnalysis] Ensembl Analysis
** @param [r] segment [const EnsPQcsequence] Segment Ensembl QC Sequence
** @param [u] qcdasfs [AjPList] AJAX List of Ensembl QC DAS Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureadaptorFetchAllBySegment(
    EnsPQcdasfeatureadaptor qcdasfa,
    const EnsPAnalysis analysis,
    const EnsPQcsequence segment,
    AjPList qcdasfs)
{
    AjPStr constraint = NULL;

    if(!qcdasfa)
        return ajFalse;

    if(!segment)
        return ajFalse;

    if(!qcdasfs)
        return ajFalse;

    constraint = ajFmtStr("das_feature.segment_id = %u",
                          ensQcsequenceGetIdentifier(segment));

    if(analysis)
        ajFmtPrintAppS(&constraint,
                       " AND das_feature.analysis_id = %u",
                       ensAnalysisGetIdentifier(analysis));

    ensBaseadaptorGenericFetch(qcdasfa,
                               constraint,
                               (EnsPAssemblymapper) NULL,
                               (EnsPSlice) NULL,
                               qcdasfs);

    ajStrDel(&constraint);

    return ajTrue;
}




/* @func ensQcdasfeatureadaptorFetchAllByAFS **********************************
**
** Fetch all Ensembl QC DAS Features by an Ensembl Analysis and
** Ensembl QC Sequences representing Ensembl QC DAS Feature feature and
** segment.
** The caller is responsible for deleting the Ensembl QC DAS Features
** before deleting the AJAX List.
**
** @param [u] qcdasfa [EnsPQcdasfeatureadaptor] Ensembl QC DAS Feature Adaptor
** @param [r] analysis [const EnsPAnalysis] Ensembl Analysis
** @param [r] feature [const EnsPQcsequence] Feature Ensembl QC Sequence
** @param [r] segment [const EnsPQcsequence] Segment Ensembl QC Sequence
** @param [u] qcdasfs [AjPList] AJAX List of Ensembl QC DAS Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureadaptorFetchAllByAFS(
    EnsPQcdasfeatureadaptor qcdasfa,
    const EnsPAnalysis analysis,
    const EnsPQcsequence feature,
    const EnsPQcsequence segment,
    AjPList qcdasfs)
{
    AjPStr constraint = NULL;

    if(!qcdasfa)
        return ajFalse;

    if(!analysis)
        return ajFalse;

    if(!feature)
        return ajFalse;

    if(!segment)
        return ajFalse;

    constraint = ajFmtStr("das_feature.analysis_id = %u "
                          "AND "
                          "das_feature.feature_id = %u "
                          "AND "
                          "das_feature.segment_id = %u ",
                          ensAnalysisGetIdentifier(analysis),
                          ensQcsequenceGetIdentifier(feature),
                          ensQcsequenceGetIdentifier(segment));

    ensBaseadaptorGenericFetch(qcdasfa,
                               constraint,
                               (EnsPAssemblymapper) NULL,
                               (EnsPSlice) NULL,
                               qcdasfs);

    ajStrDel(&constraint);

    return ajTrue;
}




/* @func ensQcdasfeatureadaptorFetchAllByRegion *******************************
**
** Fetch all Ensembl QC DAS Features that fall into a region on the segment.
** The caller is responsible for deleting the Ensembl QC DAS Features
** before deleting the AJAX List.
**
** @param [u] qcdasfa [EnsPQcdasfeatureadaptor] Ensembl QC DAS Feature Adaptor
** @param [r] analysis [const EnsPAnalysis] Ensembl Analysis
** @param [r] segment [const EnsPQcsequence] Segment Ensembl QC Sequence
** @param [r] start [ajuint] Start
** @param [r] end [ajuint] End
** @param [u] qcdasfs [AjPList] AJAX List of Ensembl QC DAS Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureadaptorFetchAllByRegion(
    EnsPQcdasfeatureadaptor qcdasfa,
    const EnsPAnalysis analysis,
    const EnsPQcsequence segment,
    ajuint start,
    ajuint end,
    AjPList qcdasfs)
{
    AjPStr constraint = NULL;

    if(!qcdasfa)
        return ajFalse;

    if(!analysis)
        return ajFalse;

    if(!segment)
        return ajFalse;

    if(!qcdasfs)
        return ajFalse;

    constraint = ajFmtStr(
        "das.analysis_id = %u "
        "AND "
        "das.segment_id = %u "
        "AND "
        "("
        "(das.segment_start >= %u AND das.segment_start <= %u) "
        "OR "
        "(das.segment_end >= %u AND das.segment_end <= %u) "
        "OR "
        "(das.segment_start <= %u AND das.segment_end >= %u)"
        ")",
        ensAnalysisGetIdentifier(analysis),
        ensQcsequenceGetIdentifier(segment),
        start,
        end,
        start,
        end,
        start,
        end);

    ensBaseadaptorGenericFetch(qcdasfa,
                               constraint,
                               (EnsPAssemblymapper) NULL,
                               (EnsPSlice) NULL,
                               qcdasfs);

    ajStrDel(&constraint);

    return ajTrue;
}




/* @func ensQcdasfeatureadaptorStore ******************************************
**
** Store an Ensembl QC DAS Feature.
**
** @param [u] qcdasfa [EnsPQcdasfeatureadaptor] Ensembl QC DAS Feature Adaptor
** @param [u] qcdasf [EnsPQcdasfeature] Ensembl QC DAS Feature
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureadaptorStore(EnsPQcdasfeatureadaptor qcdasfa,
                                   EnsPQcdasfeature qcdasf)
{
    AjBool value = AJFALSE;

    AjPSqlstatement sqls = NULL;

    AjPStr statement = NULL;

    EnsPDatabaseadaptor dba = NULL;

    if(!qcdasfa)
        return ajFalse;

    if(!qcdasf)
        return ajFalse;

    if(ensQcdasfeatureGetAdaptor(qcdasf) &&
       ensQcdasfeatureGetIdentifier(qcdasf))
        return ajFalse;

    dba = ensBaseadaptorGetDatabaseadaptor(qcdasfa);

    statement = ajFmtStr(
        "INSERT IGNORE INTO "
        "das_feature "
        "SET "
        "das_feature.alignment_id = %u, "
        "das_feature.analysis_id = %u, "
        "das_feature.feature_id = %u, "
        "das_feature.feature_start = %u, "
        "das_feature.feature_end = %u, "
        "das_feature.segment_id = %u, "
        "das_feature.segment_start = %u, "
        "das_feature.segment_end = %u, "
        "das_feature.segment_strand = %d, "
        "das_feature.phase = %d, "
        "das_feature.category = '%s', "
        "das_feature.type = '%s'",
        ensQcalignmentGetIdentity(qcdasf->Qcalignment),
        ensAnalysisGetIdentifier(qcdasf->Analysis),
        ensQcsequenceGetIdentifier(qcdasf->FeatureSequence),
        qcdasf->FeatureStart,
        qcdasf->FeatureEnd,
        ensQcsequenceGetIdentifier(qcdasf->SegmentSequence),
        qcdasf->SegmentStart,
        qcdasf->SegmentEnd,
        qcdasf->SegmentStrand,
        qcdasf->Phase,
        ensQcdasfeatureCategoryToChar(qcdasf->Category),
        ensQcdasfeatureTypeToChar(qcdasf->Type));

    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);

    if(ajSqlstatementGetAffectedrows(sqls))
    {
        ensQcdasfeatureSetIdentifier(qcdasf,
                                     ajSqlstatementGetIdentifier(sqls));

        ensQcdasfeatureSetAdaptor(qcdasf, qcdasfa);

        value = ajTrue;
    }

    ensDatabaseadaptorSqlstatementDel(dba, &sqls);

    ajStrDel(&statement);

    return value;
}




/* @func ensQcdasfeatureadaptorUpdate *****************************************
**
** Update an Ensembl QC DAS Feature.
**
** @param [u] qcdasfa [EnsPQcdasfeatureadaptor] Ensembl QC DAS Feature Adaptor
** @param [r] qcdasf [const EnsPQcdasfeature] Ensembl QC DAS Feature
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureadaptorUpdate(EnsPQcdasfeatureadaptor qcdasfa,
                                    const EnsPQcdasfeature qcdasf)
{
    AjBool value = AJFALSE;

    AjPSqlstatement sqls = NULL;

    AjPStr statement = NULL;

    EnsPDatabaseadaptor dba = NULL;

    if(!qcdasfa)
        return ajFalse;

    if(!qcdasf)
        return ajFalse;

    if(!ensQcdasfeatureGetIdentifier(qcdasf))
        return ajFalse;

    dba = ensBaseadaptorGetDatabaseadaptor(qcdasfa);

    statement = ajFmtStr(
        "UPDATE IGNORE "
        "das_feature "
        "SET "
        "das_feature.alignment_id = %u, "
        "das_feature.analysis_id = %u, "
        "das_feature.feature_id = %u, "
        "das_feature.feature_start = %u, "
        "das_feature.feature_end = %u, "
        "das_feature.segment_id = %u, "
        "das_feature.segment_start = %u, "
        "das_feature.segment_end = %u, "
        "das_feature.segment_strand = %d, "
        "das_feature.phase = %d, "
        "das_feature.category = '%s', "
        "das_feature.type = '%s' "
        "WHERE "
        "das_feature.das_feature_id = %u",
        ensQcalignmentGetIdentity(qcdasf->Qcalignment),
        ensAnalysisGetIdentifier(qcdasf->Analysis),
        ensQcsequenceGetIdentifier(qcdasf->FeatureSequence),
        qcdasf->FeatureStart,
        qcdasf->FeatureEnd,
        ensQcsequenceGetIdentifier(qcdasf->SegmentSequence),
        qcdasf->SegmentStart,
        qcdasf->SegmentEnd,
        qcdasf->SegmentStrand,
        qcdasf->Phase,
        ensQcdasfeatureCategoryToChar(qcdasf->Category),
        ensQcdasfeatureTypeToChar(qcdasf->Type),
        ensQcdasfeatureGetIdentifier(qcdasf));

    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);

    if(ajSqlstatementGetAffectedrows(sqls))
        value = ajTrue;

    ensDatabaseadaptorSqlstatementDel(dba, &sqls);

    ajStrDel(&statement);

    return value;
}




/* @func ensQcdasfeatureadaptorDelete *****************************************
**
** Delete an Ensembl QC DAS Feature.
**
** @param [u] qcdasfa [EnsPQcdasfeatureadaptor] Ensembl QC DAS Feature Adaptor
** @param [u] qcdasf [EnsPQcdasfeature] Ensembl QC DAS Feature
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcdasfeatureadaptorDelete(EnsPQcdasfeatureadaptor qcdasfa,
                                    EnsPQcdasfeature qcdasf)
{
    AjBool value = AJFALSE;

    AjPSqlstatement sqls = NULL;

    AjPStr statement = NULL;

    EnsPDatabaseadaptor dba = NULL;

    if(!qcdasfa)
        return ajFalse;

    if(!qcdasf)
        return ajFalse;

    if(!ensQcdasfeatureGetIdentifier(qcdasf))
        return ajFalse;

    dba = ensBaseadaptorGetDatabaseadaptor(qcdasfa);

    statement = ajFmtStr(
        "DELETE FROM "
        "das_feature "
        "WHERE "
        "das_feature.das_feature_id = %u",
        qcdasf->Identifier);

    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);

    if(ajSqlstatementGetAffectedrows(sqls))
    {
        qcdasf->Adaptor    = (EnsPQcdasfeatureadaptor) NULL;
        qcdasf->Identifier = 0;

        value = ajTrue;
    }

    ensDatabaseadaptorSqlstatementDel(dba, &sqls);

    ajStrDel(&statement);

    return value;
}




/* @datasection [EnsPQcvariation] QC Variation ********************************
**
** Functions for manipulating Ensembl QC Variation objects
**
** Bio::EnsEMBL::QC::Variation CVS Revision:
**
** @nam2rule Qcvariation
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl QC Variation by pointer.
** It is the responsibility of the user to first destroy any previous
** QC Variation. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPQcvariation]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPQcvariation] Ensembl QC Variation
** @argrule Ref qcv [EnsPQcvariation] Ensembl QC Variation
**
** @valrule * [EnsPQcvariation] Ensembl QC Variation
**
** @fcategory new
******************************************************************************/




/* @func ensQcvariationNew ****************************************************
**
** Default constructor for an Ensembl QC Variation.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [u] qcva [EnsPQcvariationadaptor] Ensembl QC Variation Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::QC::Variation::new
** @param [u] qca [EnsPQcalignment] Ensembl QC Alignment
** @param [u] analysis [EnsPAnalysis] Ensembl Analysis
** @param [u] qsequence [EnsPQcsequence] Query Ensembl QC Sequence
** @param [r] qstart [ajuint] Query start
** @param [r] qend [ajuint] Query end
** @param [r] qstring [AjPStr] Query string
** @param [u] tsequence [EnsPQcsequence] Target Ensembl QC Sequence
** @param [r] tstart [ajuint] Target start
** @param [r] tend [ajuint] Target end
** @param [r] tstring [AjPStr] Target string
** @param [r] class [EnsEQcvariationClass] Class
** @param [r] type [EnsEQcvariationType] Type
** @param [r] state [EnsEQcvariationState] State
**
** @return [EnsPQcvariation] Ensembl QC Variation or NULL
** @@
******************************************************************************/

EnsPQcvariation ensQcvariationNew(EnsPQcvariationadaptor qcva,
                                  ajuint identifier,
                                  EnsPQcalignment qca,
                                  EnsPAnalysis analysis,
                                  EnsPQcsequence qsequence,
                                  ajuint qstart,
                                  ajuint qend,
                                  AjPStr qstring,
                                  EnsPQcsequence tsequence,
                                  ajuint tstart,
                                  ajuint tend,
                                  AjPStr tstring,
                                  EnsEQcvariationClass class,
                                  EnsEQcvariationType type,
                                  EnsEQcvariationState state)
{
    EnsPQcvariation qcv = NULL;

    if(!qca)
        return NULL;

    if(!analysis)
        return NULL;

    if(!qsequence)
        return NULL;

    if(!tsequence)
        return NULL;

    AJNEW0(qcv);

    qcv->Use = 1;

    qcv->Identifier = identifier;

    qcv->Adaptor = qcva;

    qcv->Qcalignment = ensQcalignmentNewRef(qca);

    qcv->Analysis = ensAnalysisNewRef(analysis);

    qcv->QuerySequence = ensQcsequenceNewRef(qsequence);
    qcv->QueryStart    = qstart;
    qcv->QueryEnd      = qend;

    if(qstring)
        qcv->QueryString = ajStrNewRef(qstring);

    qcv->TargetSequence = ensQcsequenceNewRef(tsequence);
    qcv->TargetStart    = tstart;
    qcv->TargetEnd      = tend;

    if(tstring)
        qcv->TargetString = ajStrNewRef(tstring);

    qcv->Class = class;
    qcv->Type  = type;
    qcv->State = state;

    return qcv;
}




/* @func ensQcvariationNewObj *************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPQcvariation] Ensembl QC Variation
**
** @return [EnsPQcvariation] Ensembl QC Variation or NULL
** @@
******************************************************************************/

EnsPQcvariation ensQcvariationNewObj(const EnsPQcvariation object)
{
    EnsPQcvariation qcv= NULL;

    if(!object)
        return NULL;

    AJNEW0(qcv);

    qcv->Use = 1;

    qcv->Identifier = object->Identifier;

    qcv->Adaptor = object->Adaptor;

    qcv->Qcalignment = ensQcalignmentNewRef(object->Qcalignment);

    qcv->Analysis = ensAnalysisNewRef(object->Analysis);

    qcv->QuerySequence = ensQcsequenceNewRef(object->QuerySequence);
    qcv->QueryStart    = object->QueryStart;
    qcv->QueryEnd      = object->QueryEnd;

    if(object->QueryString)
        qcv->QueryString = ajStrNewRef(object->QueryString);

    qcv->TargetSequence = ensQcsequenceNewRef(object->TargetSequence);
    qcv->TargetStart    = object->TargetStart;
    qcv->TargetEnd      = object->TargetEnd;

    if(object->TargetString)
        qcv->TargetString = ajStrNewRef(object->TargetString);

    qcv->Class = object->Class;
    qcv->Type  = object->Type;
    qcv->State = object->State;

    return qcv;
}




/* @func ensQcvariationNewRef *************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] qcv [EnsPQcvariation] Ensembl Variation
**
** @return [EnsPQcvariation] Ensembl QC Variation
** @@
******************************************************************************/

EnsPQcvariation ensQcvariationNewRef(EnsPQcvariation qcv)
{
    if(!qcv)
        return NULL;

    qcv->Use++;

    return qcv;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl QC Variation.
**
** @fdata [EnsPQcvariation]
** @fnote None
**
** @nam3rule Del Destroy (free) a QC Variation object
**
** @argrule * Pqcv [EnsPQcvariation*] QC Variation object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensQcvariationDel ****************************************************
**
** Default destructor for an Ensembl QC Variation.
**
** @param [d] Pqcv [EnsPQcvariation*] Ensembl QC Variation address
**
** @return [void]
** @@
******************************************************************************/

void ensQcvariationDel(EnsPQcvariation *Pqcv)
{
    EnsPQcvariation pthis = NULL;

    if(!Pqcv)
        return;

    if(!*Pqcv)
        return;

    if(ajDebugTest("ensQcvariationDel"))
    {
        ajDebug("ensQcvariationDel\n"
                "  *Pqcv %p\n",
                *Pqcv);

        ensQcvariationTrace(*Pqcv, 1);
    }

    pthis = *Pqcv;

    pthis->Use--;

    if(pthis->Use)
    {
        *Pqcv = NULL;

        return;
    }

    ensQcalignmentDel(&pthis->Qcalignment);

    ensAnalysisDel(&pthis->Analysis);

    ensQcsequenceDel(&pthis->QuerySequence);
    ensQcsequenceDel(&pthis->TargetSequence);

    ajStrDel(&pthis->QueryString);
    ajStrDel(&pthis->TargetString);

    AJFREE(pthis);

    *Pqcv = NULL;

    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl QC Variation object.
**
** @fdata [EnsPQcvariation]
** @fnote None
**
** @nam3rule Get Return QC Variation attribute(s)
** @nam4rule GetAdaptor Return the Ensembl QC Variation Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetQcalignment Return the Ensembl QC Alignment
** @nam4rule GetAnalysis Return the Ensembl Analysis
** @nam4rule GetQuerySequence Return the query Ensembl QC Sequence
** @nam4rule GetQueryStart Return the query start
** @nam4rule GetQueryEnd Return the query end
** @nam4rule GetQueryString Return the query string
** @nam4rule GetTargetSequence Return the target Ensembl QC Sequence
** @nam4rule GetTargetStart Return the target start
** @nam4rule GetTargetEnd Return the target end
** @nam4rule GetTargetString Return the target string
** @nam4rule GetClass Return the class
** @nam4rule GetType Return the type
** @nam4rule GetState Return the state
**
** @argrule * qcv [const EnsPQcvariation] QC Variation
**
** @valrule Adaptor [EnsPQcvariationadaptor] Ensembl QC Variation Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Qcalignment [EnsPQcalignment] Ensembl QC Alignment
** @valrule Analysis [EnsPAnalysis] Ensembl Analysis
** @valrule QuerySequence [AjPStr] Query Ensembl QC Sequence
** @valrule QueryStart [ajuint] Query start
** @valrule QueryEnd [ajuint] Query end
** @valrule QueryString [AjPStr] Query string
** @valrule TargetSequence [AjPStr] Target Ensembl QC Sequence
** @valrule TargetStart [ajuint] Target start
** @valrule TargetEnd [ajuint] Target end
** @valrule TargetString [AjPStr] Target string
** @valrule Class [EnsEQcvariationClass] Class
** @valrule Type [EnsEQcvariationType] Type
** @valrule State [EnsEQcvariationState] State
**
** @fcategory use
******************************************************************************/




/* @func ensQcvariationGetAdaptor *********************************************
**
** Get the Ensembl QC Variation Adaptor element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQcvariation] Ensembl QC Variation
**
** @return [EnsPQcvariationadaptor] Ensembl QC Variation Adaptor
** @@
******************************************************************************/

EnsPQcvariationadaptor ensQcvariationGetAdaptor(const EnsPQcvariation qcv)
{
    if(!qcv)
        return NULL;

    return qcv->Adaptor;
}




/* @func ensQcvariationGetIdentifier ******************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQcvariation] Ensembl QC Variation
**
** @return [ajuint] SQL database-internal identifier
** @@
******************************************************************************/

ajuint ensQcvariationGetIdentifier(const EnsPQcvariation qcv)
{
    if(!qcv)
        return 0;

    return qcv->Identifier;
}




/* @func ensQcvariationGetQcalignment *****************************************
**
** Get the Ensembl QC Alignment element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQcvariation] Ensembl QC Variation
**
** @return [EnsPQcalignment] Ensembl QC Alignment
** @@
******************************************************************************/

EnsPQcalignment ensQcvariationGetQcalignment(const EnsPQcvariation qcv)
{
    if(!qcv)
        return NULL;

    return qcv->Qcalignment;
}




/* @func ensQcvariationGetAnalysis ********************************************
**
** Get the Ensembl Analysis element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQcvariation] Ensembl QC Variation
**
** @return [EnsPAnalysis] Ensembl Analysis
** @@
******************************************************************************/

EnsPAnalysis ensQcvariationGetAnalysis(const EnsPQcvariation qcv)
{
    if(!qcv)
        return NULL;

    return qcv->Analysis;
}




/* @func ensQcvariationGetQuerySequence ***************************************
**
** Get the query Ensembl QC Sequence element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQcvariation] Ensembl QC Variation
**
** @return [EnsPQcsequence] Query Ensembl QC Sequence
** @@
******************************************************************************/

EnsPQcsequence ensQcvariationGetQuerySequence(const EnsPQcvariation qcv)
{
    if(!qcv)
        return NULL;

    return qcv->QuerySequence;
}




/* @func ensQcvariationGetQueryStart ******************************************
**
** Get the query start element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQcvariation] Ensembl QC Variation
**
** @return [ajuint] Query start
** @@
******************************************************************************/

ajuint ensQcvariationGetQueryStart(const EnsPQcvariation qcv)
{
    if(!qcv)
        return 0;

    return qcv->QueryStart;
}




/* @func ensQcvariationGetQueryEnd ********************************************
**
** Get the query end element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQcvariation] Ensembl QC Variation
**
** @return [ajuint] Query end
** @@
******************************************************************************/

ajuint ensQcvariationGetQueryEnd(const EnsPQcvariation qcv)
{
    if(!qcv)
        return 0;

    return qcv->QueryEnd;
}




/* @func ensQcvariationGetQueryString *****************************************
**
** Get the query string element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQcvariation] Ensembl QC Variation
**
** @return [AjPStr] Query string
** @@
******************************************************************************/

AjPStr ensQcvariationGetQueryString(const EnsPQcvariation qcv)
{
    if(!qcv)
        return NULL;

    return qcv->QueryString;
}




/* @func ensQcvariationGetTargetSequence **************************************
**
** Get the target Ensembl QC Sequence element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQcvariation] Ensembl QC Variation
**
** @return [EnsPQcsequence] Target Ensembl QC Sequence
** @@
******************************************************************************/

EnsPQcsequence ensQcvariationGetTargetSequence(const EnsPQcvariation qcv)
{
    if(!qcv)
        return NULL;

    return qcv->TargetSequence;
}




/* @func ensQcvariationGetTargetStart *****************************************
**
** Get the target start element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQcvariation] Ensembl QC Variation
**
** @return [ajuint] Target start
** @@
******************************************************************************/

ajuint ensQcvariationGetTargetStart(const EnsPQcvariation qcv)
{
    if(!qcv)
        return 0;

    return qcv->TargetStart;
}




/* @func ensQcvariationGetTargetEnd *******************************************
**
** Get the target end element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQcvariation] Ensembl QC Variation
**
** @return [ajuint] Target end
** @@
******************************************************************************/

ajuint ensQcvariationGetTargetEnd(const EnsPQcvariation qcv)
{
    if(!qcv)
        return 0;

    return qcv->TargetEnd;
}




/* @func ensQcvariationGetTargetString ****************************************
**
** Get the target string element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQcvariation] Ensembl QC Variation
**
** @return [AjPStr] Target string
** @@
******************************************************************************/

AjPStr ensQcvariationGetTargetString(const EnsPQcvariation qcv)
{
    if(!qcv)
        return NULL;

    return qcv->TargetString;
}




/* @func ensQcvariationGetClass ***********************************************
**
** Get the class element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQcvariation] Ensembl QC Variation
**
** @return [EnsEQcvariationClass] Class or ensEQcvariationClassNULL
** @@
******************************************************************************/

EnsEQcvariationClass ensQcvariationGetClass(const EnsPQcvariation qcv)
{
    if(!qcv)
        return ensEQcvariationClassNULL;

    return qcv->Class;
}




/* @func ensQcvariationGetType ************************************************
**
** Get the type element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQcvariation] Ensembl QC Variation
**
** @return [EnsEQcvariationType] Type or ensEQcvariationTypeNULL
** @@
******************************************************************************/

EnsEQcvariationType ensQcvariationGetType(const EnsPQcvariation qcv)
{
    if(!qcv)
        return ensEQcvariationTypeNULL;

    return qcv->Type;
}




/* @func ensQcvariationGetState ***********************************************
**
** Get the state element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQcvariation] Ensembl QC Variation
**
** @return [EnsEQcvariationState] State or ensEQcvariationStateNULL
** @@
******************************************************************************/

EnsEQcvariationState ensQcvariationGetState(const EnsPQcvariation qcv)
{
    if(!qcv)
        return ensEQcvariationStateNULL;

    return qcv->State;
}




/* @section modifiers *********************************************************
**
** Functions for assigning elements of an Ensembl QC Variation object.
**
** @fdata [EnsPQcvariation]
** @fnote None
**
** @nam3rule Set Set one element of a QC Variation
** @nam4rule SetAdaptor Set the Ensembl QC Variation Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetQcalignment Set the Ensembl QC Alignment
** @nam4rule SetAnalysis Set the Ensembl Analysis
** @nam4rule SetQuerySequence Set the query Ensembl QC Sequence
** @nam4rule SetQueryStart Set the query start
** @nam4rule SetQueryEnd Set the query end
** @nam4rule SetQueryString Set the query string
** @nam4rule SetTargetSequence Set the target Ensembl QC Sequence
** @nam4rule SetTargetStart Set the target start
** @nam4rule SetTargetEnd Set the target end
** @nam4rule SetTargetString Set the target string
** @nam4rule SetClass Set the class
** @nam4rule SetType Set the type
** @nam4rule SetState Set the state
**
** @argrule * qcv [EnsPQcvariation] Ensembl QC Variation
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensQcvariationSetAdaptor *********************************************
**
** Set the Ensembl Database Adaptor element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQcvariation] Ensembl QC Variation
** @param [u] qcva [EnsPQcvariationadaptor] Ensembl QC Variation Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationSetAdaptor(EnsPQcvariation qcv,
                                EnsPQcvariationadaptor qcva)
{
    if(!qcv)
        return ajFalse;

    qcv->Adaptor = qcva;

    return ajTrue;
}




/* @func ensQcvariationSetIdentifier ******************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl QC Variation.
**
** @param [u] qcv [EnsPQcvariation] Ensembl QC Variation
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationSetIdentifier(EnsPQcvariation qcv,
                                   ajuint identifier)
{
    if(!qcv)
        return ajFalse;

    qcv->Identifier = identifier;

    return ajTrue;
}




/* @func ensQcvariationSetAnalysis ********************************************
**
** Set the Ensembl Analysis element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQcvariation] Ensembl QC Variation
** @param [u] analysis [EnsPAnalysis] Ensembl Analysis
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationSetAnalysis(EnsPQcvariation qcv,
                                 EnsPAnalysis analysis)
{
    if(!qcv)
        return ajFalse;

    ensAnalysisDel(&qcv->Analysis);

    qcv->Analysis = ensAnalysisNewRef(analysis);

    return ajTrue;
}




/* @func ensQcvariationSetQcalignment *****************************************
**
** Set the Ensembl QC Alignment element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQcvariation] Ensembl QC Variation
** @param [u] qca [EnsPQcalignment] Ensembl QC Alignment
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationSetQcalignment(EnsPQcvariation qcv,
                                    EnsPQcalignment qca)
{
    if(!qcv)
        return ajFalse;

    ensQcalignmentDel(&qcv->Qcalignment);

    qcv->Qcalignment = ensQcalignmentNewRef(qca);

    return ajTrue;
}




/* @func ensQcvariationSetQuerySequence ***************************************
**
** Set the query Ensembl QC Sequence element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQcvariation] Ensembl QC Variation
** @param [u] qsequence [EnsPQcsequence] Ensembl QC Sequence
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationSetQuerySequence(EnsPQcvariation qcv,
                                      EnsPQcsequence qsequence)
{
    if(!qcv)
        return ajFalse;

    ensQcsequenceDel(&qcv->QuerySequence);

    qcv->QuerySequence = ensQcsequenceNewRef(qsequence);

    return ajTrue;
}




/* @func ensQcvariationSetQueryStart ******************************************
**
** Set the query start element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQcvariation] Ensembl QC Variation
** @param [r] qstart [ajuint] Query start
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationSetQueryStart(EnsPQcvariation qcv,
                                   ajuint qstart)
{
    if(!qcv)
        return ajFalse;

    qcv->QueryStart = qstart;

    return ajTrue;
}




/* @func ensQcvariationSetQueryEnd ********************************************
**
** Set the query end element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQcvariation] Ensembl QC Variation
** @param [r] qend [ajuint] Query end
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationSetQueryEnd(EnsPQcvariation qcv,
                                 ajuint qend)
{
    if(!qcv)
        return ajFalse;

    qcv->QueryEnd = qend;

    return ajTrue;
}




/* @func ensQcvariationSetQueryString *****************************************
**
** Set the query string element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQcvariation] Ensembl QC Variation
** @param [u] qstring [AjPStr] Query string
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationSetQueryString(EnsPQcvariation qcv,
                                    AjPStr qstring)
{
    if(!qcv)
        return ajFalse;

    ajStrDel(&qcv->QueryString);

    if(qcv->QueryString)
        qcv->QueryString = ajStrNewRef(qstring);

    return ajTrue;
}




/* @func ensQcvariationSetTargetSequence **************************************
**
** Set the target Ensembl QC Sequence element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQcvariation] Ensembl QC Variation
** @param [u] tsequence [EnsPQcsequence] Ensembl QC Sequence
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationSetTargetSequence(EnsPQcvariation qcv,
                                       EnsPQcsequence tsequence)
{
    if(!qcv)
        return ajFalse;

    ensQcsequenceDel(&qcv->TargetSequence);

    qcv->TargetSequence = ensQcsequenceNewRef(tsequence);

    return ajTrue;
}




/* @func ensQcvariationSetTargetStart *****************************************
**
** Set the target start element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQcvariation] Ensembl QC Variation
** @param [r] tstart [ajuint] Target start
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationSetTargetStart(EnsPQcvariation qcv,
                                    ajuint tstart)
{
    if(!qcv)
        return ajFalse;

    qcv->TargetStart = tstart;

    return ajTrue;
}




/* @func ensQcvariationSetTargetEnd *******************************************
**
** Set the target end element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQcvariation] Ensembl QC Variation
** @param [r] tend [ajuint] Target end
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationSetTargetEnd(EnsPQcvariation qcv,
                                  ajuint tend)
{
    if(!qcv)
        return ajFalse;

    qcv->TargetEnd = tend;

    return ajTrue;
}




/* @func ensQcvariationSetTargetString ****************************************
**
** Set the target string element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQcvariation] Ensembl QC Variation
** @param [u] tstring [AjPStr] Target string
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationSetTargetString(EnsPQcvariation qcv,
                                     AjPStr tstring)
{
    if(!qcv)
        return ajFalse;

    ajStrDel(&qcv->TargetString);

    if(tstring)
        qcv->TargetString = ajStrNewRef(tstring);

    return ajTrue;
}




/* @func ensQcvariationSetClass ***********************************************
**
** Set the class element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQcvariation] Ensembl QC Variation
** @param [r] class [EnsEQcvariationClass] Class
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationSetClass(EnsPQcvariation qcv,
                              EnsEQcvariationClass class)
{
    if(!qcv)
        return ajFalse;

    qcv->Class = class;

    return ajTrue;
}




/* @func ensQcvariationSetType ************************************************
**
** Set the type element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQcvariation] Ensembl QC Variation
** @param [r] type [EnsEQcvariationType] Type
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationSetType(EnsPQcvariation qcv,
                             EnsEQcvariationType type)
{
    if(!qcv)
        return ajFalse;

    qcv->Type = type;

    return ajTrue;
}




/* @func ensQcvariationSetState ***********************************************
**
** Set the state element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQcvariation] Ensembl QC Variation
** @param [r] state [EnsEQcvariationState] State
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationSetState(EnsPQcvariation qcv,
                              EnsEQcvariationState state)
{
    if(!qcv)
        return ajFalse;

    qcv->State = state;

    return ajTrue;
}




/* @func ensQcvariationGetMemsize *********************************************
**
** Get the memory size in bytes of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQcvariation] Ensembl QC Variation
**
** @return [ajulong] Memory size
** @@
******************************************************************************/

ajulong ensQcvariationGetMemsize(const EnsPQcvariation qcv)
{
    ajulong size = 0;

    if(!qcv)
        return 0;

    size += sizeof (EnsOQcvariation);

    size += ensQcalignmentGetMemsize(qcv->Qcalignment);

    size += ensAnalysisGetMemsize(qcv->Analysis);

    size += ensQcsequenceGetMemsize(qcv->QuerySequence);
    size += ensQcsequenceGetMemsize(qcv->TargetSequence);

    if(qcv->QueryString)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(qcv->QueryString);
    }

    if(qcv->TargetString)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(qcv->TargetString);
    }

    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl QC Variation object.
**
** @fdata [EnsPQcvariation]
** @nam3rule Trace Report QC Variation elements to debug file
**
** @argrule Trace qcv [const EnsPQcvariation] Ensembl QC Variation
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensQcvariationTrace **************************************************
**
** Trace an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQcvariation] Ensembl QC Variation
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationTrace(const EnsPQcvariation qcv, ajuint level)
{
    AjPStr indent = NULL;

    if(!qcv)
        return ajFalse;

    indent = ajStrNew();

    ajStrAppendCountK(&indent, ' ', level * 2);

    ajDebug("%SensQcvariationTrace %p\n"
            "%S  Use %u\n"
            "%S  Identifier %u\n"
            "%S  Adaptor %p\n"
            "%S  Qcalignment %p\n"
            "%S  Analysis %p\n"
            "%S  QuerySequence %p\n"
            "%S  QueryStart %u\n"
            "%S  QueryEnd %u\n"
            "%S  QueryString '%S'\n"
            "%S  TargetSequence %p\n"
            "%S  TargetStart %u\n"
            "%S  TargetEnd %u\n"
            "%S  TargetString '%S'\n"
            "%S  Class '%s'\n"
            "%S  Type '%s'\n"
            "%S  State '%s'\n",
            indent, qcv,
            indent, qcv->Use,
            indent, qcv->Identifier,
            indent, qcv->Adaptor,
            indent, qcv->Qcalignment,
            indent, qcv->Analysis,
            indent, qcv->QuerySequence,
            indent, qcv->QueryStart,
            indent, qcv->QueryEnd,
            indent, qcv->QueryString,
            indent, qcv->TargetSequence,
            indent, qcv->TargetStart,
            indent, qcv->TargetEnd,
            indent, qcv->TargetString,
            indent, ensQcvariationClassToChar(qcv->Class),
            indent, ensQcvariationTypeToChar(qcv->Type),
            indent, ensQcvariationStateToChar(qcv->State));

    ensQcalignmentTrace(qcv->Qcalignment, level + 1);

    ensAnalysisTrace(qcv->Analysis, level + 1);

    ensQcsequenceTrace(qcv->QuerySequence, level + 1);
    ensQcsequenceTrace(qcv->TargetSequence, level + 1);

    ajStrDel(&indent);

    return ajTrue;
}




/* @func ensQcvariationClassFromStr *******************************************
**
** Convert an AJAX String into an Ensembl QC Variation class element.
**
** @param [r] vclass [const AjPStr] Class string
**
** @return [EnsEQcvariationClass] Ensembl QC Variation class or
**                                ensEQcvariationClassNULL
** @@
******************************************************************************/

EnsEQcvariationClass ensQcvariationClassFromStr(const AjPStr vclass)
{
    register EnsEQcvariationClass i = ensEQcvariationClassNULL;

    EnsEQcvariationClass eclass = ensEQcvariationClassNULL;

    for(i = ensEQcvariationClassNone; qcvariationClass[i]; i++)
        if(ajStrMatchCaseC(vclass, qcvariationClass[i]))
            eclass = i;

    if(!eclass)
        ajDebug("ensQcvariationClassFromStr encountered "
                "unexpected string '%S'.\n", vclass);

    return eclass;
}




/* @func ensQcvariationTypeFromStr ********************************************
**
** Convert an AJAX String into an Ensembl QC Variation type element.
**
** @param [r] type [const AjPStr] Type string
**
** @return [EnsEQcvariationType] Ensembl QC Variation type or
**                               ensEQcvariationTypeNULL
** @@
******************************************************************************/

EnsEQcvariationType ensQcvariationTypeFromStr(const AjPStr type)
{
    register EnsEQcvariationType i = ensEQcvariationTypeNULL;

    EnsEQcvariationType etype = ensEQcvariationTypeNULL;

    for(i = ensEQcvariationTypeNone; qcvariationType[i]; i++)
        if(ajStrMatchCaseC(type, qcvariationType[i]))
            etype = i;

    if(!etype)
        ajDebug("ensQcvariationTypeFromStr encountered "
                "unexpected string '%S'.\n", type);

    return etype;
}




/* @func ensQcvariationStateFromStr *******************************************
**
** Convert an AJAX String into an Ensembl QC Variation state element.
**
** @param [r] state [const AjPStr] State string
**
** @return [EnsEQcvariationState] Ensembl QC Variation state or
**                                ensEQcvariationStateNULL
** @@
******************************************************************************/

EnsEQcvariationState ensQcvariationStateFromStr(const AjPStr state)
{
    register EnsEQcvariationState i = ensEQcvariationStateNULL;

    EnsEQcvariationState estate = ensEQcvariationStateNULL;

    for(i = ensEQcvariationStateNone; qcvariationState[i]; i++)
        if(ajStrMatchCaseC(state, qcvariationState[i]))
            estate = i;

    if(!estate)
        ajDebug("ensQcvariationStateFromStr encountered "
                "unexpected string '%S'.\n", state);

    return estate;
}




/* @func ensQcvariationClassToChar ********************************************
**
** Convert an Ensembl QC Variation class element into a C-type (char*) string.
**
** @param [r] vclass [EnsEQcvariationClass] Ensembl QC Variation class
**
** @return [const char*] Ensembl QC Variation class C-type (char*) string
** @@
******************************************************************************/

const char* ensQcvariationClassToChar(EnsEQcvariationClass vclass)
{
    register EnsEQcvariationClass i = ensEQcvariationClassNULL;

    if(!vclass)
        return NULL;

    for(i = ensEQcvariationClassNone;
        qcvariationClass[i] && (i < vclass);
        i++);

    if(!qcvariationClass[i])
        ajDebug("ensQcvariationClassToChar encountered an "
                "out of boundary error on group %d.\n", vclass);

    return qcvariationClass[i];
}




/* @func ensQcvariationTypeToChar *********************************************
**
** Convert an Ensembl QC Variation type element into a C-type (char*) string.
**
** @param [r] type [EnsEQcvariationType] Ensembl QC Variation type
**
** @return [const char*] Ensembl QC Variation type C-type (char*) string
** @@
******************************************************************************/

const char* ensQcvariationTypeToChar(EnsEQcvariationType type)
{
    register EnsEQcvariationType i = ensEQcvariationTypeNULL;

    if(!type)
        return NULL;

    for(i = ensEQcvariationTypeNone;
        qcvariationType[i] && (i < type);
        i++);

    if(!qcvariationType[i])
        ajDebug("ensQcvariationTypeToChar encountered an "
                "out of boundary error on group %d.\n", type);

    return qcvariationType[i];
}




/* @func ensQcvariationStateToChar ********************************************
**
** Convert an Ensembl QC Variation state element into a C-type (char*) string.
**
** @param [r] state [EnsEQcvariationState] Ensembl QC Variation state
**
** @return [const char*] Ensembl QC Variation state C-type (char*) string
** @@
******************************************************************************/

const char* ensQcvariationStateToChar(EnsEQcvariationState state)
{
    register EnsEQcvariationState i = ensEQcvariationStateNULL;

    if(!state)
        return NULL;

    for(i = ensEQcvariationStateNone;
        qcvariationState[i] && (i < state);
        i++);

    if(!qcvariationState[i])
        ajDebug("ensQcvariationStateToChar encountered an "
                "out of boundary error on group %d.\n", state);

    return qcvariationState[i];
}




/* @datasection [EnsPQcvariationadaptor] QC Variation Adaptor *****************
**
** Functions for manipulating Ensembl QC Variation Adaptor objects
**
** Bio::EnsEMBL::QC::DBSQL::Variationadaptor CVS Revision:
**
** @nam2rule Qcvariationadaptor
**
******************************************************************************/

static const char *qcvariationadaptorTables[] =
{
    "variation",
    (const char *) NULL
};




static const char *qcvariationadaptorColumns[] =
{
    "variation.variation_id",
    "variation.analysis_id",
    "variation.alignment_id",
    "variation.query_db_id",
    "variation.query_id"
    "variation.query_start",
    "variation.query_end",
    "variation.query_seq",
    "variation.target_db_id"
    "variation.target_id",
    "variation.target_start",
    "variation.target_end",
    "variation.target_seq",
    "variation.class",
    "variation.type",
    "variation.state",
    (const char *) NULL
};




static EnsOBaseadaptorLeftJoin qcvariationadaptorLeftJoin[] =
{
    {(const char*) NULL, (const char*) NULL}
};




static const char *qcvariationadaptorDefaultCondition =
    (const char*) NULL;




static const char *qcvariationadaptorFinalCondition =
    (const char *) NULL;




/* @funcstatic qcvariationadaptorFetchAllBySQL ********************************
**
** Run a SQL statement against an Ensembl Database Adaptor and consolidate the
** results into an AJAX List of Ensembl QC Variation objects.
**
** @param [u] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [uN] am [EnsPAssemblymapper] Ensembl Assembly Mapper
** @param [uN] slice [EnsPSlice] Ensembl Slice
** @param [u] qcvs [AjPList] AJAX List of Ensembl QC Variations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool qcvariationadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                              const AjPStr statement,
                                              EnsPAssemblymapper am,
                                              EnsPSlice slice,
                                              AjPList qcvs)
{
    ajuint identifier  = 0;
    ajuint analysisid  = 0;
    ajuint alignmentid = 0;

    ajuint qdbid  = 0;
    ajuint qsid   = 0;
    ajuint qstart = 0;
    ajuint qend   = 0;
    ajuint tdbid  = 0;
    ajuint tsid   = 0;
    ajuint tstart = 0;
    ajuint tend   = 0;

    EnsEQcvariationClass eclass = ensEQcvariationClassNULL;
    EnsEQcvariationType etype   = ensEQcvariationTypeNULL;
    EnsEQcvariationState estate = ensEQcvariationStateNULL;

    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;

    AjPStr qstring = NULL;
    AjPStr tstring = NULL;
    AjPStr class   = NULL;
    AjPStr type    = NULL;
    AjPStr state   = NULL;

    EnsPAnalysis analysis  = NULL;
    EnsPAnalysisadaptor aa = NULL;

    EnsPQcalignment qca         = NULL;
    EnsPQcalignmentadaptor qcaa = NULL;

    EnsPQcsequence qsequence   = NULL;
    EnsPQcsequence tsequence   = NULL;
    EnsPQcsequenceadaptor qcsa = NULL;

    EnsPQcvariation qcv         = NULL;
    EnsPQcvariationadaptor qcva = NULL;

    if(ajDebugTest("qcvariationadaptorFetchAllBySQL"))
        ajDebug("qcvariationadaptorFetchAllBySQL\n"
                "  dba %p\n"
                "  statement %p\n"
                "  am %p\n"
                "  slice %p\n"
                "  qcvs %p\n",
                dba,
                statement,
                am,
                slice,
                qcvs);

    if(!dba)
        return ajFalse;

    if(!statement)
        return ajFalse;

    if(!qcvs)
        return ajFalse;

    aa = ensRegistryGetAnalysisadaptor(dba);

    qcaa = ensRegistryGetQcalignmentadaptor(dba);

    qcva = ensRegistryGetQcvariationadaptor(dba);

    qcsa = ensRegistryGetQcsequenceadaptor(dba);

    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);

    sqli = ajSqlrowiterNew(sqls);

    while(!ajSqlrowiterDone(sqli))
    {
        identifier  = 0;
        analysisid  = 0;
        alignmentid = 0;
        qdbid       = 0;
        qsid        = 0;
        qstart      = 0;
        qend        = 0;
        qstring     = ajStrNew();
        tdbid       = 0;
        tsid        = 0;
        tstart      = 0;
        tend        = 0;
        tstring     = ajStrNew();
        class       = ajStrNew();
        type        = ajStrNew();
        state       = ajStrNew();

        sqlr = ajSqlrowiterGet(sqli);

        ajSqlcolumnToUint(sqlr, &identifier);
        ajSqlcolumnToUint(sqlr, &analysisid);
        ajSqlcolumnToUint(sqlr, &alignmentid);
        ajSqlcolumnToUint(sqlr, &qdbid);
        ajSqlcolumnToUint(sqlr, &qsid);
        ajSqlcolumnToUint(sqlr, &qstart);
        ajSqlcolumnToUint(sqlr, &qend);
        ajSqlcolumnToStr(sqlr, &qstring);
        ajSqlcolumnToUint(sqlr, &tdbid);
        ajSqlcolumnToUint(sqlr, &tsid);
        ajSqlcolumnToUint(sqlr, &tstart);
        ajSqlcolumnToUint(sqlr, &tend);
        ajSqlcolumnToStr(sqlr, &tstring);
        ajSqlcolumnToStr(sqlr, &class);
        ajSqlcolumnToStr(sqlr, &type);
        ajSqlcolumnToStr(sqlr, &state);

        ensQcalignmentadaptorFetchByIdentifier(qcaa, alignmentid, &qca);

        ensAnalysisadaptorFetchByIdentifier(aa, analysisid, &analysis);

        ensQcsequenceadaptorFetchByIdentifier(qcsa, qsid, &qsequence);

        ensQcsequenceadaptorFetchByIdentifier(qcsa, tsid, &tsequence);

        eclass = ensQcvariationClassFromStr(class);

        etype = ensQcvariationTypeFromStr(type);

        estate = ensQcvariationStateFromStr(state);

        qcv = ensQcvariationNew(qcva,
                                identifier,
                                qca,
                                analysis,
                                qsequence,
                                qstart,
                                qend,
                                qstring,
                                tsequence,
                                tstart,
                                tend,
                                tstring,
                                eclass,
                                etype,
                                estate);

        ajListPushAppend(qcvs, (void *) qcv);

        ensQcsequenceDel(&qsequence);
        ensQcsequenceDel(&tsequence);

        ensAnalysisDel(&analysis);

        ensQcalignmentDel(&qca);

        ajStrDel(&qstring);
        ajStrDel(&tstring);
        ajStrDel(&class);
        ajStrDel(&type);
        ajStrDel(&state);
    }

    ajSqlrowiterDel(&sqli);

    ensDatabaseadaptorSqlstatementDel(dba, &sqls);

    return ajTrue;
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl QC Variation Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** QC Variation Adaptor. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPQcvariationadaptor]
** @fnote None
**
** @nam3rule New Constructor
**
** @argrule New dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @valrule * [EnsPQcvariationadaptor] Ensembl QC Variation Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensQcvariationadaptorNew *********************************************
**
** Default constructor for an Ensembl QC Variation Adaptor.
**
** Ensembl Object Adaptors are singleton objects in the sense that a single
** instance of an Ensembl Object Adaptor connected to a particular database is
** sufficient to instantiate any number of Ensembl Objects from the database.
** Each Ensembl Object will have a weak reference to the Object Adaptor that
** instantiated it. Therefore, Ensembl Object Adaptors should not be
** instantiated directly, but rather obtained from the Ensembl Registry,
** which will in turn call this function if neccessary.
**
** @see ensRegistryGetDatabaseadaptor
** @see ensRegistryGetQcvariationadaptor
**
** @param [u] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPQcvariationadaptor] Ensembl QC Variation Adaptor or NULL
** @@
******************************************************************************/

EnsPQcvariationadaptor ensQcvariationadaptorNew(
    EnsPDatabaseadaptor dba)
{
    if(!dba)
        return NULL;

    return ensBaseadaptorNew(
        dba,
        qcvariationadaptorTables,
        qcvariationadaptorColumns,
        qcvariationadaptorLeftJoin,
        qcvariationadaptorDefaultCondition,
        qcvariationadaptorFinalCondition,
        qcvariationadaptorFetchAllBySQL);
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl QC Variation Adaptor.
**
** @fdata [EnsPQcvariationadaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) a QC Variation Adaptor object
**
** @argrule * Pqcva [EnsPQcvariationadaptor*] QC Variation Adaptor
**                                            object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensQcvariationadaptorDel *********************************************
**
** Default destructor for an Ensembl QC Variation Adaptor.
**
** Ensembl Object Adaptors are singleton objects that are registered in the
** Ensembl Registry and weakly referenced by Ensembl Objects that have been
** instantiated by it. Therefore, Ensembl Object Adaptors should never be
** destroyed directly. Upon exit, the Ensembl Registry will call this function
** if required.
**
** @param [d] Pqcva [EnsPQcvariationadaptor*] Ensembl QC Variation Adaptor
**                                            address
**
** @return [void]
** @@
******************************************************************************/

void ensQcvariationadaptorDel(EnsPQcvariationadaptor* Pqcva)
{
    if(!Pqcva)
        return;

    if(!*Pqcva)
        return;

    if(ajDebugTest("ensQcvariationadaptorDel"))
        ajDebug("ensQcvariationadaptorDel\n"
                "  *Pqcva %p\n",
                *Pqcva);

    ensBaseadaptorDel(Pqcva);

    return;
}




/* @func ensQcvariationadaptorFetchByIdentifier *******************************
**
** Fetch an Ensembl QC Variation via its SQL database-internal identifier.
** The caller is responsible for deleting the Ensembl QC Variation.
**
** @param [u] qcva [EnsPQcvariationadaptor] Ensembl QC Variation Adaptor
** @param [r] identifier [ajuint] SQL database-internal QC Variation identifier
** @param [wP] Pqcv [EnsPQcvariation*] Ensembl QC Variation address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationadaptorFetchByIdentifier(
    EnsPQcvariationadaptor qcva,
    ajuint identifier,
    EnsPQcvariation *Pqcv)
{
    if(!qcva)
        return ajFalse;

    if(!identifier)
        return ajFalse;

    if(!Pqcv)
        return ajFalse;

    *Pqcv = (EnsPQcvariation)
        ensBaseadaptorFetchByIdentifier(qcva, identifier);

    return ajTrue;
}




/* @func ensQcvariationadaptorFetchAllByQcalignment ***************************
**
** Fetch all Ensembl QC Variations by an Ensembl QC Alignment.
** The caller is responsible for deleting the Ensembl QC Variations
** before deleting the AJAX List.
**
** @param [u] qcva [EnsPQcvariationadaptor] Ensembl QC Variation Adaptor
** @param [r] qca [const EnsPQcalignment] Ensembl QC Alignment
** @param [u] qcvs [AjPList] AJAX List of Ensembl QC Variations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationadaptorFetchAllByQcalignment(
    EnsPQcvariationadaptor qcva,
    const EnsPQcalignment qca,
    AjPList qcvs)
{
    AjPStr constraint = NULL;

    if(!qcva)
        return ajFalse;

    if(!qca)
        return ajFalse;

    if(!qcvs)
        return ajFalse;

    constraint = ajFmtStr("variation.alignment_id = %u",
                          ensQcalignmentGetIdentifier(qca));

    ensBaseadaptorGenericFetch(qcva,
                               constraint,
                               (EnsPAssemblymapper) NULL,
                               (EnsPSlice) NULL,
                               qcvs);

    ajStrDel(&constraint);

    return ajTrue;
}




/* @func ensQcvariationadaptorFetchAllByAnalysisQueryTarget *******************
**
** Fetch all Ensembl QC Variations by Ensembl an Analysis, as well as
** Query and Target Ensembl Sequences.
** The caller is responsible for deleting the Ensembl QC Variations
** before deleting the AJAX List.
**
** @param [u] qcva [EnsPQcvariationadaptor] Ensembl QC Variation Adaptor
** @param [r] analysis [const EnsPAnalysis] Ensembl Analysis
** @param [r] qdb [const EnsPQcdatabase] Query Ensembl QC Database
** @param [r] tdb [const EnsPQcdatabase] Target Ensembl QC Database
** @param [u] qcvs [AjPList] AJAX List of Ensembl QC Variations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationadaptorFetchAllByAnalysisQueryTarget(
    EnsPQcvariationadaptor qcva,
    const EnsPAnalysis analysis,
    const EnsPQcdatabase qdb,
    const EnsPQcdatabase tdb,
    AjPList qcvs)
{
    AjPStr constraint = NULL;

    if(!qcva)
        return ajFalse;

    if(!analysis)
        return ajFalse;

    if(!qdb)
        return ajFalse;

    if(!tdb)
        return ajFalse;

    if(!qcvs)
        return ajFalse;

    constraint = ajFmtStr("variation.analysis_id = %u "
                          "AND "
                          "variation.query_db_id = %u "
                          "AND "
                          "variation.target_db_id = %u",
                          ensAnalysisGetIdentifier(analysis),
                          ensQcdatabaseGetIdentifier(qdb),
                          ensQcdatabaseGetIdentifier(tdb));

    ensBaseadaptorGenericFetch(qcva,
                               constraint,
                               (EnsPAssemblymapper) NULL,
                               (EnsPSlice) NULL,
                               qcvs);

    ajStrDel(&constraint);

    return ajTrue;
}




/* @func ensQcvariationadaptorFetchAllByQuery *********************************
**
** Fetch all Ensembl QC Variations by a Query Ensembl QC Sequence.
** The caller is responsible for deleting the Ensembl QC Variations
** before deleting the AJAX List.
**
** @param [u] qcva [EnsPQcvariationadaptor] Ensembl QC Variation Adaptor
** @param [rN] analysis [const EnsPAnalysis] Ensembl Analysis
** @param [r] qdb [const EnsPQcdatabase] Query Ensembl QC Database
** @param [u] qcvs [AjPList] AJAX List of Ensembl QC Variations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationadaptorFetchAllByQuery(
    EnsPQcvariationadaptor qcva,
    const EnsPAnalysis analysis,
    const EnsPQcdatabase qdb,
    AjPList qcvs)
{
    AjPStr constraint = NULL;

    if(!qcva)
        return ajFalse;

    if(!qdb)
        return ajFalse;

    if(!qcvs)
        return ajFalse;

    constraint = ajFmtStr("variation.query_db_id = %u",
                          ensQcdatabaseGetIdentifier(qdb));

    if(analysis)
        ajFmtPrintAppS(&constraint,
                       " AND variation.analysis_id = %u",
                       ensAnalysisGetIdentifier(analysis));

    ensBaseadaptorGenericFetch(qcva,
                               constraint,
                               (EnsPAssemblymapper) NULL,
                               (EnsPSlice) NULL,
                               qcvs);

    ajStrDel(&constraint);

    return ajTrue;
}




/* @func ensQcvariationadaptorFetchAllByTarget ********************************
**
** Fetch all Ensembl QC Variations by a Target Ensembl QC Sequence.
** The caller is responsible for deleting the Ensembl QC Variations
** before deleting the AJAX List.
**
** @param [u] qcva [EnsPQcvariationadaptor] Ensembl QC Variation Adaptor
** @param [rN] analysis [const EnsPAnalysis] Ensembl Analysis
** @param [r] tdb [const EnsPQcdatabase] Target Ensembl QC Database
** @param [u] qcvs [AjPList] AJAX List of Ensembl QC Variations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationadaptorFetchAllByTarget(
    EnsPQcvariationadaptor qcva,
    const EnsPAnalysis analysis,
    const EnsPQcdatabase tdb,
    AjPList qcvs)
{
    AjPStr constraint = NULL;

    if(!qcva)
        return ajFalse;

    if(!tdb)
        return ajFalse;

    if(!qcvs)
        return ajFalse;

    constraint = ajFmtStr("variation.target_db_id = %u",
                          ensQcdatabaseGetIdentifier(tdb));

    if(analysis)
        ajFmtPrintAppS(&constraint,
                       " AND variation.analysis_id = %u",
                       ensAnalysisGetIdentifier(analysis));

    ensBaseadaptorGenericFetch(qcva,
                               constraint,
                               (EnsPAssemblymapper) NULL,
                               (EnsPSlice) NULL,
                               qcvs);

    ajStrDel(&constraint);

    return ajTrue;
}




/* @func ensQcvariationadaptorStore *******************************************
**
** Store an Ensembl QC Variation.
**
** @param [u] qcva [EnsPQcvariationadaptor] Ensembl QC Variation Adaptor
** @param [u] qcv [EnsPQcvariation] Ensembl QC Variation
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationadaptorStore(EnsPQcvariationadaptor qcva,
                                  EnsPQcvariation qcv)
{
    char *txtqstr = NULL;
    char *txttstr = NULL;

    AjBool value = AJFALSE;

    AjPSqlstatement sqls = NULL;

    AjPStr statement = NULL;

    EnsPDatabaseadaptor dba = NULL;

    if(!qcva)
        return ajFalse;

    if(!qcv)
        return ajFalse;

    if(ensQcvariationGetAdaptor(qcv) && ensQcvariationGetIdentifier(qcv))
        return ajFalse;

    dba = ensBaseadaptorGetDatabaseadaptor(qcva);

    ensDatabaseadaptorEscapeC(dba, &txtqstr, qcv->QueryString);

    ensDatabaseadaptorEscapeC(dba, &txttstr, qcv->TargetString);

    statement = ajFmtStr(
        "INSERT IGNORE INTO "
        "variation "
        "SET "
        "variation.analysis_id = %u, "
        "variation.alignment_id = %u, "
        "variation.query_db_id = %u, "
        "variation.query_id = %u, "
        "variation.query_start = %u, "
        "variation.query_end = %u, "
        "variation.query_seq = '%s', "
        "variation.target_db_id = %u, "
        "variation.target_id = %u, "
        "variation.target_start = %u, "
        "variation.target_end = %u, "
        "variation.target_seq = '%s', "
        "variation.class = '%s', "
        "variation.type = '%s', "
        "variation.state = '%s'",
        ensAnalysisGetIdentifier(qcv->Analysis),
        ensQcalignmentGetIdentity(qcv->Qcalignment),
        ensQcsequenceGetQcdatabaseIdentifier(qcv->QuerySequence),
        ensQcsequenceGetIdentifier(qcv->QuerySequence),
        qcv->QueryStart,
        qcv->QueryEnd,
        txtqstr,
        ensQcsequenceGetQcdatabaseIdentifier(qcv->TargetSequence),
        ensQcsequenceGetIdentifier(qcv->TargetSequence),
        qcv->TargetStart,
        qcv->TargetEnd,
        txttstr,
        ensQcvariationClassToChar(qcv->Class),
        ensQcvariationTypeToChar(qcv->Type),
        ensQcvariationStateToChar(qcv->State));

    ajCharDel(&txtqstr);
    ajCharDel(&txttstr);

    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);

    if(ajSqlstatementGetAffectedrows(sqls))
    {
        ensQcvariationSetIdentifier(qcv, ajSqlstatementGetIdentifier(sqls));

        ensQcvariationSetAdaptor(qcv, qcva);

        value = ajTrue;
    }

    ensDatabaseadaptorSqlstatementDel(dba, &sqls);

    ajStrDel(&statement);

    return value;
}




/* @func ensQcvariationadaptorUpdate ******************************************
**
** Update an Ensembl QC Variation.
**
** @param [u] qcva [EnsPQcvariationadaptor] Ensembl QC Variation Adaptor
** @param [r] qcv [const EnsPQcvariation] Ensembl QC Variation
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationadaptorUpdate(EnsPQcvariationadaptor qcva,
                                   const EnsPQcvariation qcv)
{
    char *txtqstr = NULL;
    char *txttstr = NULL;

    AjBool value = AJFALSE;

    AjPSqlstatement sqls = NULL;

    AjPStr statement = NULL;

    EnsPDatabaseadaptor dba = NULL;

    if(!qcva)
        return ajFalse;

    if(!qcv)
        return ajFalse;

    if(!ensQcvariationGetIdentifier(qcv))
        return ajFalse;

    dba = ensBaseadaptorGetDatabaseadaptor(qcva);

    ensDatabaseadaptorEscapeC(dba, &txtqstr, qcv->QueryString);

    ensDatabaseadaptorEscapeC(dba, &txttstr, qcv->TargetString);

    statement = ajFmtStr(
        "UPDATE IGNORE "
        "variation "
        "SET "
        "variation.analysis_id = %u, "
        "variation.alignment_id = %u, "
        "variation.query_db_id = %u, "
        "variation.query_id = %u, "
        "variation.query_start = %u, "
        "variation.query_end = %u, "
        "variation.query_seq = '%s', "
        "variation.target_db_id = %u, "
        "variation.target_id = %u, "
        "variation.target_start = %u, "
        "variation.target_end = %u, "
        "variation.target_seq = '%s', "
        "variation.class = '%s', "
        "variation.type = '%s', "
        "variation.state = '%s' "
        "WHERE "
        "variation.variation_id = %u",
        ensAnalysisGetIdentifier(qcv->Analysis),
        ensQcalignmentGetIdentity(qcv->Qcalignment),
        ensQcsequenceGetQcdatabaseIdentifier(qcv->QuerySequence),
        ensQcsequenceGetIdentifier(qcv->QuerySequence),
        qcv->QueryStart,
        qcv->QueryEnd,
        txtqstr,
        ensQcsequenceGetQcdatabaseIdentifier(qcv->TargetSequence),
        ensQcsequenceGetIdentifier(qcv->TargetSequence),
        qcv->TargetStart,
        qcv->TargetEnd,
        txttstr,
        ensQcvariationClassToChar(qcv->Class),
        ensQcvariationTypeToChar(qcv->Type),
        ensQcvariationStateToChar(qcv->State),
        qcv->Identifier);

    ajCharDel(&txtqstr);
    ajCharDel(&txttstr);

    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);

    if(ajSqlstatementGetAffectedrows(sqls))
        value = ajTrue;

    ensDatabaseadaptorSqlstatementDel(dba, &sqls);

    ajStrDel(&statement);

    return value;
}




/* @func ensQcvariationadaptorDelete ******************************************
**
** Delete an Ensembl QC Variation.
**
** @param [u] qcva [EnsPQcvariationadaptor] Ensembl QC Variation Adaptor
** @param [u] qcv [EnsPQcvariation] Ensembl QC Variation
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcvariationadaptorDelete(EnsPQcvariationadaptor qcva,
                                   EnsPQcvariation qcv)
{
    AjBool value = AJFALSE;

    AjPSqlstatement sqls = NULL;

    AjPStr statement = NULL;

    EnsPDatabaseadaptor dba = NULL;

    if(!qcva)
        return ajFalse;

    if(!qcv)
        return ajFalse;

    if(!ensQcvariationGetIdentifier(qcv))
        return ajFalse;

    dba = ensBaseadaptorGetDatabaseadaptor(qcva);

    statement = ajFmtStr(
        "DELETE FROM "
        "variation "
        "WHERE "
        "variation.variation_id = %u",
        qcv->Identifier);

    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);

    if(ajSqlstatementGetAffectedrows(sqls))
    {
        qcv->Adaptor    = (EnsPQcvariationadaptor) NULL;
        qcv->Identifier = 0;

        value = ajTrue;
    }

    ensDatabaseadaptorSqlstatementDel(dba, &sqls);

    ajStrDel(&statement);

    return value;
}




/* @datasection [EnsPQcsubmission] QC Submission ******************************
**
** Functions for manipulating Ensembl QC Submission objects
**
** Bio::EnsEMBL::QC::Submission CVS Revision:
**
** @nam2rule Qcsubmission
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl QC Submission by pointer.
** It is the responsibility of the user to first destroy any previous
** QC Submission. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPQcsubmission]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPQcsubmission] Ensembl QC Submission
** @argrule Ref object [EnsPQcsubmission] Ensembl QC Submission
**
** @valrule * [EnsPQcsubmission] Ensembl QC Submission
**
** @fcategory new
******************************************************************************/




/* @func ensQcsubmissionNew ***************************************************
**
** Default constructor for an Ensembl QC Submission.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [u] qcsba [EnsPQcsubmissionadaptor] Ensembl QC Submission Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::QC::Submission::new
** @param [u] analysis [EnsPAnalysis] Ensembl Analysis
** @param [u] qsequence [EnsPQcsequence] Query Ensembl QC Sequence
** @param [u] tsequence [EnsPQcsequence] Target Ensembl QC Sequence
** @param [r] tstart [ajuint] Target start
** @param [r] tend [ajuint] Target end
** @param [r] tstrand [ajint] Target strand
** @param [r] analysisjobid [ajuint] Ensembl Hive Analysis Job identifier
**
** @return [EnsPQcsubmission] Ensembl QC Submission or NULL
** @@
******************************************************************************/

EnsPQcsubmission ensQcsubmissionNew(EnsPQcsubmissionadaptor qcsba,
                                    ajuint identifier,
                                    EnsPAnalysis analysis,
                                    EnsPQcsequence qsequence,
                                    EnsPQcsequence tsequence,
                                    ajuint tstart,
                                    ajuint tend,
                                    ajint tstrand,
                                    ajuint analysisjobid)
{
    EnsPQcsubmission qcsb = NULL;

    if(!analysis)
        return NULL;

    if(!qsequence)
        return NULL;

    if(!tsequence)
        return NULL;

    AJNEW0(qcsb);

    qcsb->Use = 1;

    qcsb->Identifier = identifier;

    qcsb->Adaptor = qcsba;

    qcsb->Analysis = ensAnalysisNewRef(analysis);

    qcsb->QuerySequence  = ensQcsequenceNewRef(qsequence);
    qcsb->TargetSequence = ensQcsequenceNewRef(tsequence);

    qcsb->TargetStart = tstart;
    qcsb->TargetEnd   = tend;

    qcsb->TargetStrand = tstrand;

    qcsb->AnalysisJobIdentifier = analysisjobid;

    return qcsb;
}




/* @func ensQcsubmissionNewObj ************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPQcsubmission] Ensembl QC Submission
**
** @return [EnsPQcsubmission] Ensembl QC Submission or NULL
** @@
******************************************************************************/

EnsPQcsubmission ensQcsubmissionNewObj(const EnsPQcsubmission object)
{
    EnsPQcsubmission qcsb = NULL;

    if(!object)
        return NULL;

    AJNEW0(qcsb);

    qcsb->Use = 1;

    qcsb->Identifier = object->Identifier;

    qcsb->Adaptor = object->Adaptor;

    if(object->Analysis)
        qcsb->Analysis = ensAnalysisNewRef(object->Analysis);

    qcsb->QuerySequence = ensQcsequenceNewRef(object->QuerySequence);

    qcsb->TargetSequence = ensQcsequenceNewRef(object->TargetSequence);

    qcsb->TargetStart = object->TargetStart;

    qcsb->TargetEnd = object->TargetEnd;

    qcsb->TargetStrand = object->TargetStrand;

    qcsb->AnalysisJobIdentifier = object->AnalysisJobIdentifier;

    return qcsb;
}




/* @func ensQcsubmissionNewRef ************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] qcsb [EnsPQcsubmission] Ensembl Submission
**
** @return [EnsPQcsubmission] Ensembl QC Submission
** @@
******************************************************************************/

EnsPQcsubmission ensQcsubmissionNewRef(EnsPQcsubmission qcsb)
{
    if(!qcsb)
        return NULL;

    qcsb->Use++;

    return qcsb;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl QC Submission.
**
** @fdata [EnsPQcsubmission]
** @fnote None
**
** @nam3rule Del Destroy (free) a QC Submission object
**
** @argrule * Pqcsb [EnsPQcsubmission*] QC Submission object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensQcsubmissionDel ***************************************************
**
** Default destructor for an Ensembl QC Submission.
**
** Ensembl Object Adaptors are singleton objects that are registered in the
** Ensembl Registry and weakly referenced by Ensembl Objects that have been
** instantiated by it. Therefore, Ensembl Object Adaptors should never be
** destroyed directly. Upon exit, the Ensembl Registry will call this function
** if required.
**
** @param [d] Pqcsb [EnsPQcsubmission*] Ensembl QC Submission address
**
** @return [void]
** @@
******************************************************************************/

void ensQcsubmissionDel(EnsPQcsubmission *Pqcsb)
{
    EnsPQcsubmission pthis = NULL;

    if(!Pqcsb)
        return;

    if(!*Pqcsb)
        return;

    if(ajDebugTest("ensQcsubmissionDel"))
    {
        ajDebug("ensQcsubmissionDel\n"
                "  *Pqcsb %p\n",
                *Pqcsb);

        ensQcsubmissionTrace(*Pqcsb, 1);
    }

    pthis = *Pqcsb;

    pthis->Use--;

    if(pthis->Use)
    {
        *Pqcsb = NULL;

        return;
    }

    ensAnalysisDel(&pthis->Analysis);

    ensQcsequenceDel(&pthis->QuerySequence);

    ensQcsequenceDel(&pthis->TargetSequence);

    AJFREE(pthis);

    *Pqcsb = NULL;

    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl QC Submission object.
**
** @fdata [EnsPQcsubmission]
** @fnote None
**
** @nam3rule Get Return QC Submission attribute(s)
** @nam4rule GetAdaptor Return the Ensembl QC Submission Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetAnalysis Return the Ensembl Analysis
** @nam4rule GetQuerySequence Return the query Ensembl QC Sequence
** @nam4rule GetQueryStart Return the query start
** @nam4rule GetQueryEnd Return the query end
** @nam4rule GetQueryStrand Return the query strand
** @nam4rule GetTargetSequence Return the target Ensembl QC Sequence
** @nam4rule GetTargetStart Return the target start
** @nam4rule GetTargetEnd Return the target end
** @nam4rule GetTargetStrand Return the target strand
** @nam4rule GetAnalysisJobIdentifier Return the Hive Analysis Job Identifier
**
** @argrule * qcsb [const EnsPQcsubmission] QC Submission
**
** @valrule Adaptor [EnsPQcsubmissionadaptor] Ensembl QC Submission Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Analysis [EnsPAnalysis] Ensembl Analysis
** @valrule QuerySequence [AjPStr] Query Ensembl QC Sequence
** @valrule QueryStart [ajuint] Query start
** @valrule QueryEnd [ajuint] Query end
** @valrule QueryStrand [ajint] Query strand
** @valrule TargetSequence [AjPStr] Target Ensembl QC Sequence
** @valrule TargetStart [ajuint] Target start
** @valrule TargetEnd [ajuint] Target end
** @valrule TargetStrand [ajint] Target strand
** @valrule AnalysisJobIdentifier [ajuint] Hive Analysis Job Identifier
**
** @fcategory use
******************************************************************************/




/* @func ensQcsubmissionGetAdaptor ********************************************
**
** Get the Ensembl QC Submission Adaptor element of an Ensembl QC Submission.
**
** @param [r] qcsb [const EnsPQcsubmission] Ensembl QC Submission
**
** @return [EnsPQcsubmissionadaptor] Ensembl QC Submission Adaptor
** @@
******************************************************************************/

EnsPQcsubmissionadaptor ensQcsubmissionGetAdaptor(const EnsPQcsubmission qcsb)
{
    if(!qcsb)
        return NULL;

    return qcsb->Adaptor;
}




/* @func ensQcsubmissionGetIdentifier *****************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl QC Submission.
**
** @param [r] qcsb [const EnsPQcsubmission] Ensembl QC Submission
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensQcsubmissionGetIdentifier(const EnsPQcsubmission qcsb)
{
    if(!qcsb)
        return 0;

    return qcsb->Identifier;
}




/* @func ensQcsubmissionGetAnalysis *******************************************
**
** Get the Ensembl Analysis element of an Ensembl QC Submission.
**
** @param [r] qcsb [const EnsPQcsubmission] Ensembl QC Submission
**
** @return [EnsPAnalysis] Ensembl Analysis
** @@
******************************************************************************/

EnsPAnalysis ensQcsubmissionGetAnalysis(const EnsPQcsubmission qcsb)
{
    if(!qcsb)
        return NULL;

    return qcsb->Analysis;
}




/* @func ensQcsubmissionGetQuerySequence **************************************
**
** Get the query Ensembl QC Sequence element of an Ensembl QC Submission.
**
** @param [r] qcsb [const EnsPQcsubmission] Ensembl QC Submission
**
** @return [EnsPQcsequence] Query Ensembl QC Sequence
** @@
******************************************************************************/

EnsPQcsequence ensQcsubmissionGetQuerySequence(const EnsPQcsubmission qcsb)
{
    if(!qcsb)
        return NULL;

    return qcsb->QuerySequence;
}




/* @func ensQcsubmissionGetQueryStart *****************************************
**
** Get the query start element of an Ensembl QC Submission.
**
** @param [r] qcsb [const EnsPQcsubmission] Ensembl QC Submission
**
** @return [ajuint] Query start
** @@
******************************************************************************/

ajuint ensQcsubmissionGetQueryStart(const EnsPQcsubmission qcsb)
{
    if(!qcsb)
        return 0;

    return qcsb->QueryStart;
}




/* @func ensQcsubmissionGetQueryEnd *******************************************
**
** Get the query end element of an Ensembl QC Submission.
**
** @param [r] qcsb [const EnsPQcsubmission] Ensembl QC Submission
**
** @return [ajuint] Query end
** @@
******************************************************************************/

ajuint ensQcsubmissionGetQueryEnd(const EnsPQcsubmission qcsb)
{
    if(!qcsb)
        return 0;

    return qcsb->QueryEnd;
}




/* @func ensQcsubmissionGetQueryStrand ****************************************
**
** Get the query strand element of an Ensembl QC Submission.
**
** @param [r] qcsb [const EnsPQcsubmission] Ensembl QC Submission
**
** @return [ajint] Query strand
** @@
******************************************************************************/

ajint ensQcsubmissionGetQueryStrand(const EnsPQcsubmission qcsb)
{
    if(!qcsb)
        return 0;

    return qcsb->QueryStrand;
}




/* @func ensQcsubmissionGetTargetSequence *************************************
**
** Get the target Ensembl QC Sequence element of an Ensembl QC Submission.
**
** @param [r] qcsb [const EnsPQcsubmission] Ensembl QC Submission
**
** @return [EnsPQcsequence] Target Ensembl QC Sequence
** @@
******************************************************************************/

EnsPQcsequence ensQcsubmissionGetTargetSequence(const EnsPQcsubmission qcsb)
{
    if(!qcsb)
        return NULL;

    return qcsb->TargetSequence;
}




/* @func ensQcsubmissionGetTargetStart ****************************************
**
** Get the target start element of an Ensembl QC Submission.
**
** @param [r] qcsb [const EnsPQcsubmission] Ensembl QC Submission
**
** @return [ajuint] Target start
** @@
******************************************************************************/

ajuint ensQcsubmissionGetTargetStart(const EnsPQcsubmission qcsb)
{
    if(!qcsb)
        return 0;

    return qcsb->TargetStart;
}




/* @func ensQcsubmissionGetTargetEnd ******************************************
**
** Get the target end element of an Ensembl QC Submission.
**
** @param [r] qcsb [const EnsPQcsubmission] Ensembl QC Submission
**
** @return [ajuint] Target end
** @@
******************************************************************************/

ajuint ensQcsubmissionGetTargetEnd(const EnsPQcsubmission qcsb)
{
    if(!qcsb)
        return 0;

    return qcsb->TargetEnd;
}




/* @func ensQcsubmissionGetTargetStrand ***************************************
**
** Get the target strand element of an Ensembl QC Submission.
**
** @param [r] qcsb [const EnsPQcsubmission] Ensembl QC Submission
**
** @return [ajint] Target strand
** @@
******************************************************************************/

ajint ensQcsubmissionGetTargetStrand(const EnsPQcsubmission qcsb)
{
    if(!qcsb)
        return 0;

    return qcsb->TargetStrand;
}




/* @func ensQcsubmissionGetAnalysisJobIdentifier ******************************
**
** Get the Hive Analysis Job identifier element of an Ensembl QC Variation.
**
** @param [r] qcsb [const EnsPQcsubmission] Ensembl QC Variation
**
** @return [ajuint] Hive Analysis Job identifier
** @@
******************************************************************************/

ajuint ensQcsubmissionGetAnalysisJobIdentifier(const EnsPQcsubmission qcsb)
{
    if(!qcsb)
        return 0;

    return qcsb->AnalysisJobIdentifier;
}




/* @section modifiers *********************************************************
**
** Functions for assigning elements of an Ensembl QC Submission object.
**
** @fdata [EnsPQcsubmission]
** @fnote None
**
** @nam3rule Set Set one element of a QC Submission
** @nam4rule SetAdaptor Set the Ensembl QC Submission Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetQuerySequence Set the query Ensembl QC Sequence
** @nam4rule SetQueryStart Set the query start
** @nam4rule SetQueryEnd Set the query end
** @nam4rule SetQueryStrand Set the query strand
** @nam4rule SetTargetSequence Set the target Ensembl QC Sequence
** @nam4rule SetTargetStart Set the target start
** @nam4rule SetTargetEnd Set the target end
** @nam4rule SetTargetStrand Set the target strand
** @nam4rule SetAnalysisJobIdentifier Set the Analysis Job Identifier
**
** @argrule * qcsb [EnsPQcsubmission] Ensembl QC Submission
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensQcsubmissionSetAdaptor ********************************************
**
** Set the Ensembl Database Adaptor element of an QC Submission.
**
** @param [u] qcsb [EnsPQcsubmission] Ensembl QC Submission
** @param [uN] qcsba [EnsPQcsubmissionadaptor] Ensembl QC Submission Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcsubmissionSetAdaptor(EnsPQcsubmission qcsb,
                                 EnsPQcsubmissionadaptor qcsba)
{
    if(!qcsb)
        return ajFalse;

    qcsb->Adaptor = qcsba;

    return ajTrue;
}




/* @func ensQcsubmissionSetIdentifier *****************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl QC Submission.
**
** @param [u] qcsb [EnsPQcsubmission] Ensembl QC Submission
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcsubmissionSetIdentifier(EnsPQcsubmission qcsb,
                                    ajuint identifier)
{
    if(!qcsb)
        return ajFalse;

    qcsb->Identifier = identifier;

    return ajTrue;
}




/* @func ensQcsubmissionSetAnalysis *******************************************
**
** Set the Ensembl Analysis element of an Ensembl QC Submission.
**
** @param [u] qcsb [EnsPQcsubmission] Ensembl QC Submission
** @param [uN] analysis [EnsPAnalysis] Ensembl Analysis
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcsubmissionSetAnalysis(EnsPQcsubmission qcsb,
                                  EnsPAnalysis analysis)
{
    if(!qcsb)
        return ajFalse;

    ensAnalysisDel(&qcsb->Analysis);

    qcsb->Analysis = ensAnalysisNewRef(analysis);

    return ajTrue;
}




/* @func ensQcsubmissionSetQuerySequence **************************************
**
** Set the query Ensembl QC Sequence element of an Ensembl QC Submission.
**
** @param [u] qcsb [EnsPQcsubmission] Ensembl QC Submission
** @param [uN] qsequence [EnsPQcsequence] Ensembl QC Sequence
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcsubmissionSetQuerySequence(EnsPQcsubmission qcsb,
                                       EnsPQcsequence qsequence)
{
    if(!qcsb)
        return ajFalse;

    ensQcsequenceDel(&qcsb->QuerySequence);

    qcsb->QuerySequence = ensQcsequenceNewRef(qsequence);

    return ajTrue;
}




/* @func ensQcsubmissionSetQueryStart *****************************************
**
** Set the query start element of an Ensembl QC Submission.
**
** @param [u] qcsb [EnsPQcsubmission] Ensembl QC Submission
** @param [r] qstart [ajuint] Query start
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcsubmissionSetQueryStart(EnsPQcsubmission qcsb,
                                    ajuint qstart)
{
    if(!qcsb)
        return ajFalse;

    qcsb->QueryStart = qstart;

    return ajTrue;
}




/* @func ensQcsubmissionSetQueryEnd *******************************************
**
** Set the query end element of an Ensembl QC Submission.
**
** @param [u] qcsb [EnsPQcsubmission] Ensembl QC Submission
** @param [r] qend [ajuint] Query end
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcsubmissionSetQueryEnd(EnsPQcsubmission qcsb,
                                  ajuint qend)
{
    if(!qcsb)
        return ajFalse;

    qcsb->QueryEnd = qend;

    return ajTrue;
}




/* @func ensQcsubmissionSetQueryStrand ****************************************
**
** Set the query strand element of an Ensembl QC Submission.
**
** @param [u] qcsb [EnsPQcsubmission] Ensembl QC Submission
** @param [r] qstrand [ajint] Query strand
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcsubmissionSetQueryStrand(EnsPQcsubmission qcsb,
                                     ajint qstrand)
{
    if(!qcsb)
        return ajFalse;

    qcsb->QueryStrand = qstrand;

    return ajTrue;
}




/* @func ensQcsubmissionSetTargetSequence *************************************
**
** Set the target Ensembl QC Sequence element of an Ensembl QC Submission.
**
** @param [u] qcsb [EnsPQcsubmission] Ensembl QC Submission
** @param [uN] tsequence [EnsPQcsequence] Ensembl QC Sequence
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcsubmissionSetTargetSequence(EnsPQcsubmission qcsb,
                                        EnsPQcsequence tsequence)
{
    if(!qcsb)
        return ajFalse;

    ensQcsequenceDel(&qcsb->TargetSequence);

    qcsb->TargetSequence = ensQcsequenceNewRef(tsequence);

    return ajTrue;
}




/* @func ensQcsubmissionSetTargetStart ****************************************
**
** Set the target start element of an Ensembl QC Submission.
**
** @param [u] qcsb [EnsPQcsubmission] Ensembl QC Submission
** @param [r] tstart [ajuint] Target start
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcsubmissionSetTargetStart(EnsPQcsubmission qcsb,
                                     ajuint tstart)
{
    if(!qcsb)
        return ajFalse;

    qcsb->TargetStart = tstart;

    return ajTrue;
}




/* @func ensQcsubmissionSetTargetEnd ******************************************
**
** Set the target end element of an Ensembl QC Submission.
**
** @param [u] qcsb [EnsPQcsubmission] Ensembl QC Submission
** @param [r] tend [ajuint] Target end
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcsubmissionSetTargetEnd(EnsPQcsubmission qcsb,
                                   ajuint tend)
{
    if(!qcsb)
        return ajFalse;

    qcsb->TargetEnd = tend;

    return ajTrue;
}




/* @func ensQcsubmissionSetTargetStrand ***************************************
**
** Set the target strand element of an Ensembl QC Submission.
**
** @param [u] qcsb [EnsPQcsubmission] Ensembl QC Submission
** @param [r] tstrand [ajint] Target strand
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcsubmissionSetTargetStrand(EnsPQcsubmission qcsb,
                                      ajint tstrand)
{
    if(!qcsb)
        return ajFalse;

    qcsb->TargetStrand = tstrand;

    return ajTrue;
}




/* @func ensQcsubmissionSetAnalysisJobIdentifier ******************************
**
** Set the Hive Analysis Job identifier element of an Ensembl QC Submission.
**
** @param [u] qcsb [EnsPQcsubmission] Ensembl QC Submission
** @param [r] jobid [ajuint] Hive Analysis Job identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcsubmissionSetAnalysisJobIdentifier(EnsPQcsubmission qcsb,
                                               ajuint jobid)
{
    if(!qcsb)
        return ajFalse;

    qcsb->AnalysisJobIdentifier = jobid;

    return ajTrue;
}




/* @func ensQcsubmissionGetMemsize *********************************************
**
** Get the memory size in bytes of an Ensembl QC Submission.
**
** @param [r] qcsb [const EnsPQcsubmission] Ensembl QC Submission
**
** @return [ajulong] Memory size
** @@
******************************************************************************/

ajulong ensQcsubmissionGetMemsize(const EnsPQcsubmission qcsb)
{
    ajulong size = 0;

    if(!qcsb)
        return 0;

    size += sizeof (EnsOQcsubmission);

    size += ensAnalysisGetMemsize(qcsb->Analysis);

    size += ensQcsequenceGetMemsize(qcsb->QuerySequence);
    size += ensQcsequenceGetMemsize(qcsb->TargetSequence);

    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl QC Submission object.
**
** @fdata [EnsPQcsubmission]
** @nam3rule Trace Report Ensembl QC Submission elements to debug file
**
** @argrule Trace qcsb [const EnsPQcsubmission] Ensembl QC Submission
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensQcsubmissionTrace *************************************************
**
** Trace an Ensembl QC Submission.
**
** @param [r] qcsb [const EnsPQcsubmission] Ensembl QC Submission
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcsubmissionTrace(const EnsPQcsubmission qcsb, ajuint level)
{
    AjPStr indent = NULL;

    if(!qcsb)
        return ajFalse;

    indent = ajStrNew();

    ajStrAppendCountK(&indent, ' ', level * 2);

    ajDebug("%SensQcsubmissionTrace %p\n"
            "%S  Use %u\n"
            "%S  Identifier %u\n"
            "%S  Adaptor %p\n"
            "%S  Analysis %p\n"
            "%S  QuerySequence %p\n"
            /*
              "%S  QueryStart %u\n"
              "%S  QueryEnd %u\n"
              "%S  QueryStrand %d\n"
            */
            "%S  TargetSequence %p\n"
            "%S  TargetStart %u\n"
            "%S  TargetEnd %u\n"
            "%S  TargetStrand %d\n"
            "%S  AnalysisJobIdentifier %u\n",
            indent, qcsb,
            indent, qcsb->Use,
            indent, qcsb->Identifier,
            indent, qcsb->Adaptor,
            indent, qcsb->Analysis,
            indent, qcsb->QuerySequence,
            /*
              indent, qcsb->QueryStart,
              indent, qcsb->QueryEnd,
              indent, qcsb->QueryStrand,
            */
            indent, qcsb->TargetSequence,
            indent, qcsb->TargetStart,
            indent, qcsb->TargetEnd,
            indent, qcsb->TargetStrand,
            indent, qcsb->AnalysisJobIdentifier);

    ensAnalysisTrace(qcsb->Analysis, 1);

    ensQcsequenceTrace(qcsb->QuerySequence, 1);
    ensQcsequenceTrace(qcsb->TargetSequence, 1);

    ajStrDel(&indent);

    return ajTrue;
}




/* @datasection [EnsPQcsubmissionadaptor] QC Submission Adaptor ***************
**
** Functions for manipulating Ensembl QC Submission Adaptor objects
**
** Bio::EnsEMBL::QC::DBSQL::Submissionadaptor CVS Revision:
**
** @nam2rule Qcsubmissionadaptor
**
******************************************************************************/

static const char *qcsubmissionadaptorTables[] =
{
    "submission",
    (const char *) NULL
};




static const char *qcsubmissionadaptorColumns[] =
{
    "submission.submission_id",
    "submission.analysis_id",
    "submission.query_db_id",
    "submission.query_id"
    "submission.target_db_id"
    "submission.target_id",
    "submission.target_start",
    "submission.target_end",
    "submission.target_strand",
    "submission.analysis_job_id",
    (const char *) NULL
};




static EnsOBaseadaptorLeftJoin qcsubmissionadaptorLeftJoin[] =
{
    {(const char*) NULL, (const char*) NULL}
};




static const char *qcsubmissionadaptorDefaultCondition =
    (const char*) NULL;




static const char *qcsubmissionadaptorFinalCondition =
    (const char *) NULL;




/* @funcstatic qcsubmissionadaptorFetchAllBySQL *******************************
**
** Run a SQL statement against an Ensembl Database Adaptor and consolidate the
** results into an AJAX List of Ensembl QC Submission objects.
**
** @param [u] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [uN] am [EnsPAssemblymapper] Ensembl Assembly Mapper
** @param [uN] slice [EnsPSlice] Ensembl Slice
** @param [u] qcsbs [AjPList] AJAX List of Ensembl QC Submissions
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool qcsubmissionadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                               const AjPStr statement,
                                               EnsPAssemblymapper am,
                                               EnsPSlice slice,
                                               AjPList qcsbs)
{
    ajint tstrand = 0;

    ajuint identifier = 0;
    ajuint analysisid = 0;
    ajuint qdbid      = 0;
    ajuint qsid       = 0;
    ajuint tdbid      = 0;
    ajuint tsid       = 0;
    ajuint tstart     = 0;
    ajuint tend       = 0;

    ajuint analysisjobid = 0;

    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;

    EnsPAnalysis analysis  = NULL;
    EnsPAnalysisadaptor aa = NULL;

    EnsPQcsequence qsequence   = NULL;
    EnsPQcsequence tsequence   = NULL;
    EnsPQcsequenceadaptor qcsa = NULL;

    EnsPQcsubmission qcsb         = NULL;
    EnsPQcsubmissionadaptor qcsba = NULL;

    if(ajDebugTest("qcsubmissionadaptorFetchAllBySQL"))
        ajDebug("qcsubmissionadaptorFetchAllBySQL\n"
                "  dba %p\n"
                "  statement %p\n"
                "  am %p\n"
                "  slice %p\n"
                "  qcsbs %p\n",
                dba,
                statement,
                am,
                slice,
                qcsbs);

    if(!dba)
        return ajFalse;

    if(!statement)
        return ajFalse;

    if(!qcsbs)
        return ajFalse;

    aa = ensRegistryGetAnalysisadaptor(dba);

    qcsa = ensRegistryGetQcsequenceadaptor(dba);

    qcsba = ensRegistryGetQcsubmissionadaptor(dba);

    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);

    sqli = ajSqlrowiterNew(sqls);

    while(!ajSqlrowiterDone(sqli))
    {
        identifier    = 0;
        analysisid    = 0;
        qdbid         = 0;
        qsid          = 0;
        tdbid         = 0;
        tsid          = 0;
        tstart        = 0;
        tend          = 0;
        tstrand       = 0;
        analysisjobid = 0;

        sqlr = ajSqlrowiterGet(sqli);

        ajSqlcolumnToUint(sqlr, &identifier);
        ajSqlcolumnToUint(sqlr, &analysisid);
        ajSqlcolumnToUint(sqlr, &qdbid);
        ajSqlcolumnToUint(sqlr, &qsid);
        ajSqlcolumnToUint(sqlr, &tdbid);
        ajSqlcolumnToUint(sqlr, &tsid);
        ajSqlcolumnToUint(sqlr, &tstart);
        ajSqlcolumnToUint(sqlr, &tend);
        ajSqlcolumnToInt(sqlr, &tstrand);
        ajSqlcolumnToUint(sqlr, &analysisjobid);

        ensAnalysisadaptorFetchByIdentifier(aa, analysisid, &analysis);

        ensQcsequenceadaptorFetchByIdentifier(qcsa, qsid, &qsequence);

        ensQcsequenceadaptorFetchByIdentifier(qcsa, tsid, &tsequence);

        qcsb = ensQcsubmissionNew(qcsba,
                                  identifier,
                                  analysis,
                                  qsequence,
                                  tsequence,
                                  tstart,
                                  tend,
                                  tstrand,
                                  analysisjobid);

        ajListPushAppend(qcsbs, (void *) qcsb);

        ensAnalysisDel(&analysis);

        ensQcsequenceDel(&qsequence);
        ensQcsequenceDel(&tsequence);
    }

    ajSqlrowiterDel(&sqli);

    ensDatabaseadaptorSqlstatementDel(dba, &sqls);

    return ajTrue;
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl QC Submission Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** QC Submission Adaptor. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPQcsubmissionadaptor]
** @fnote None
**
** @nam3rule New Constructor
**
** @argrule New dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @valrule * [EnsPQcsubmissionadaptor] Ensembl QC Submission Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensQcsubmissionadaptorNew ********************************************
**
** Default constructor for an Ensembl QC Submission Adaptor.
**
** Ensembl Object Adaptors are singleton objects in the sense that a single
** instance of an Ensembl Object Adaptor connected to a particular database is
** sufficient to instantiate any number of Ensembl Objects from the database.
** Each Ensembl Object will have a weak reference to the Object Adaptor that
** instantiated it. Therefore, Ensembl Object Adaptors should not be
** instantiated directly, but rather obtained from the Ensembl Registry,
** which will in turn call this function if neccessary.
**
** @see ensRegistryGetDatabaseadaptor
** @see ensRegistryGetQcsubmissionadaptor
**
** @param [u] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPQcsubmissionadaptor] Ensembl QC Submission Adaptor or NULL
** @@
******************************************************************************/

EnsPQcsubmissionadaptor ensQcsubmissionadaptorNew(
    EnsPDatabaseadaptor dba)
{
    if(!dba)
        return NULL;

    return ensBaseadaptorNew(
        dba,
        qcsubmissionadaptorTables,
        qcsubmissionadaptorColumns,
        qcsubmissionadaptorLeftJoin,
        qcsubmissionadaptorDefaultCondition,
        qcsubmissionadaptorFinalCondition,
        qcsubmissionadaptorFetchAllBySQL);
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl QC Submission Adaptor.
**
** @fdata [EnsPQcsubmissionadaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) a QC Submission Adaptor object
**
** @argrule * Pqcsba [EnsPQcsubmissionadaptor*] QC Submission Adaptor
**                                              object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensQcsubmissionadaptorDel ********************************************
**
** Default destructor for an Ensembl QC Submission Adaptor.
**
** @param [d] Pqcsba [EnsPQcsubmissionadaptor*] Ensembl QC Submission Adaptor
**                                              address
**
** @return [void]
** @@
******************************************************************************/

void ensQcsubmissionadaptorDel(EnsPQcsubmissionadaptor* Pqcsba)
{
    if(!Pqcsba)
        return;

    if(!*Pqcsba)
        return;

    if(ajDebugTest("ensQcsubmissionadaptorDel"))
        ajDebug("ensQcsubmissionadaptorDel\n"
                "  *Pqcsba %p\n",
                *Pqcsba);

    ensBaseadaptorDel(Pqcsba);

    return;
}




/* @func ensQcsubmissionadaptorFetchByIdentifier ******************************
**
** Fetch an Ensembl QC Submission via its SQL database-internal identifier.
** The caller is responsible for deleting the Ensembl QC Submission.
**
** @param [u] qcsba [EnsPQcsubmissionadaptor] Ensembl QC Submission Adaptor
** @param [r] identifier [ajuint] SQL database-internal QC Submission
**                                identifier
** @param [wP] Pqcsb [EnsPQcsubmission*] Ensembl QC Submission address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcsubmissionadaptorFetchByIdentifier(
    EnsPQcsubmissionadaptor qcsba,
    ajuint identifier,
    EnsPQcsubmission *Pqcsb)
{
    if(!qcsba)
        return ajFalse;

    if(!identifier)
        return ajFalse;

    if(!Pqcsb)
        return ajFalse;

    *Pqcsb = (EnsPQcsubmission)
        ensBaseadaptorFetchByIdentifier(qcsba, identifier);

    return ajTrue;
}




/* @func ensQcsubmissionadaptorFetchAllByAnalysisQueryTarget ******************
**
** Fetch all Ensembl QC Submissions by an Ensembl Analysis, as well as
** Query and Target Ensembl Sequences.
** The caller is responsible for deleting the Ensembl QC Submissions
** before deleting the AJAX List.
**
** @param [u] qcsba [EnsPQcsubmissionadaptor] Ensembl QC Submission Adaptor
** @param [r] analysis [const EnsPAnalysis] Ensembl Analysis
** @param [r] qdb [const EnsPQcdatabase] Query Ensembl QC Database
** @param [r] tdb [const EnsPQcdatabase] Target Ensembl QC Database
** @param [u] qcsbs [AjPList] AJAX List of Ensembl QC Submissions
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcsubmissionadaptorFetchAllByAnalysisQueryTarget(
    EnsPQcsubmissionadaptor qcsba,
    const EnsPAnalysis analysis,
    const EnsPQcdatabase qdb,
    const EnsPQcdatabase tdb,
    AjPList qcsbs)
{
    AjPStr constraint = NULL;

    if(!qcsba)
        return ajFalse;

    if(!analysis)
        return ajFalse;

    if(!qdb)
        return ajFalse;

    if(!tdb)
        return ajFalse;

    if(!qcsbs)
        return ajFalse;

    constraint = ajFmtStr("submission.analysis_id = %u "
                          "AND "
                          "submission.query_db_id = %u "
                          "AND "
                          "submission.target_db_id = %u",
                          ensAnalysisGetIdentifier(analysis),
                          ensQcdatabaseGetIdentifier(qdb),
                          ensQcdatabaseGetIdentifier(tdb));

    ensBaseadaptorGenericFetch(qcsba,
                               constraint,
                               (EnsPAssemblymapper) NULL,
                               (EnsPSlice) NULL,
                               qcsbs);

    ajStrDel(&constraint);

    return ajTrue;
}




/* @func ensQcsubmissionadaptorFetchAllByANQIDTDB *****************************
**
** Fetch all Ensembl QC Submissions by an Ensembl Analysis, as well as
** Query and Target Ensembl Sequences.
** The caller is responsible for deleting the Ensembl QC Submissions
** before deleting the AJAX List.
**
** @param [u] qcsba [EnsPQcsubmissionadaptor] Ensembl QC Submission Adaptor
** @param [r] analysis [const EnsPAnalysis] Ensembl Analysis
** @param [r] qsequence [const EnsPQcsequence] Query Ensembl QC Sequence
** @param [r] tdb [const EnsPQcdatabase] Target Ensembl QC Database
** @param [r] tsequence [const EnsPQcsequence] Target Ensembl QC Sequence
** @param [r] tstart [ajuint] Target start
** @param [r] tend [ajuint] Target end
** @param [r] tstrand [ajint] Target strand
** @param [u] qcsbs [AjPList] AJAX List of Ensembl QC Submissions
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcsubmissionadaptorFetchAllByANQIDTDB(
    EnsPQcsubmissionadaptor qcsba,
    const EnsPAnalysis analysis,
    const EnsPQcsequence qsequence,
    const EnsPQcdatabase tdb,
    const EnsPQcsequence tsequence,
    ajuint tstart,
    ajuint tend,
    ajint tstrand,
    AjPList qcsbs)
{
    AjPStr constraint = NULL;

    if(!qcsba)
        return ajFalse;

    if(!analysis)
        return ajFalse;

    if(!qsequence)
        return ajFalse;

    if(!tdb)
        return ajFalse;

    if(!qcsbs)
        return ajFalse;

    constraint = ajFmtStr("submission.analysis_id = %u "
                          "AND "
                          "submission.query_db_id = %u "
                          "AND "
                          "submission.target_db_id = %u "
                          "AND "
                          "submission.query_id = %u",
                          ensAnalysisGetIdentifier(analysis),
                          ensQcsequenceGetQcdatabaseIdentifier(qsequence),
                          ensQcdatabaseGetIdentifier(tdb),
                          ensQcsequenceGetIdentifier(qsequence));

    if(tsequence)
        ajFmtPrintAppS(&constraint,
                       " AND submission.target_id = %u",
                       ensQcsequenceGetIdentifier(tsequence));

    if(tstart && tend)
        ajFmtPrintAppS(&constraint,
                       " AND"
                       " submission.target_start >= %u"
                       " AND"
                       " submission.target_end <= %u"
                       " AND"
                       " submission.target_strand = %d",
                       tstart,
                       tend,
                       tstrand);

    ensBaseadaptorGenericFetch(qcsba,
                               constraint,
                               (EnsPAssemblymapper) NULL,
                               (EnsPSlice) NULL,
                               qcsbs);

    ajStrDel(&constraint);

    return ajTrue;
}




/* @func ensQcsubmissionadaptorFetchAllByQuery ********************************
**
** Fetch all Ensembl QC Submissions by a Query Ensembl QC Sequence.
** The caller is responsible for deleting the Ensembl QC Submissions
** before deleting the AJAX List.
**
** @param [u] qcsba [EnsPQcsubmissionadaptor] Ensembl QC Submission Adaptor
** @param [rN] analysis [const EnsPAnalysis] Ensembl Analysis
** @param [r] qdb [const EnsPQcdatabase] Query Ensembl QC Database
** @param [u] qcsbs [AjPList] AJAX List of Ensembl QC Submissions
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcsubmissionadaptorFetchAllByQuery(
    EnsPQcsubmissionadaptor qcsba,
    const EnsPAnalysis analysis,
    const EnsPQcdatabase qdb,
    AjPList qcsbs)
{
    AjPStr constraint = NULL;

    if(!qcsba)
        return ajFalse;

    if(!qdb)
        return ajFalse;

    if(!qcsbs)
        return ajFalse;

    constraint = ajFmtStr("submission.query_db_id = %u",
                          ensQcdatabaseGetIdentifier(qdb));

    if(analysis)
        ajFmtPrintAppS(&constraint,
                       " AND submission.analysis_id = %u",
                       ensAnalysisGetIdentifier(analysis));

    ensBaseadaptorGenericFetch(qcsba,
                               constraint,
                               (EnsPAssemblymapper) NULL,
                               (EnsPSlice) NULL,
                               qcsbs);

    ajStrDel(&constraint);

    return ajTrue;
}




/* @func ensQcsubmissionadaptorFetchAllByTarget *******************************
**
** Fetch all Ensembl QC Submissions by a Target Ensembl QC Sequence.
** The caller is responsible for deleting the Ensembl QC Submissions
** before deleting the AJAX List.
**
** @param [u] qcsba [EnsPQcsubmissionadaptor] Ensembl QC Submission Adaptor
** @param [rN] analysis [const EnsPAnalysis] Ensembl Analysis
** @param [r] tdb [const EnsPQcdatabase] Target Ensembl QC Database
** @param [u] qcsbs [AjPList] AJAX List of Ensembl QC Submissions
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcsubmissionadaptorFetchAllByTarget(
    EnsPQcsubmissionadaptor qcsba,
    const EnsPAnalysis analysis,
    const EnsPQcdatabase tdb,
    AjPList qcsbs)
{
    AjPStr constraint = NULL;

    if(!qcsba)
        return ajFalse;

    if(!tdb)
        return ajFalse;

    if(!qcsbs)
        return ajFalse;

    constraint = ajFmtStr("submission.target_db_id = %u",
                          ensQcdatabaseGetIdentifier(tdb));

    if(analysis)
        ajFmtPrintAppS(&constraint,
                       " AND submission.analysis_id = %u",
                       ensAnalysisGetIdentifier(analysis));

    ensBaseadaptorGenericFetch(qcsba,
                               constraint,
                               (EnsPAssemblymapper) NULL,
                               (EnsPSlice) NULL,
                               qcsbs);

    ajStrDel(&constraint);

    return ajTrue;
}




/* @func ensQcsubmissionadaptorStore ******************************************
**
** Store an Ensembl QC Submission.
**
** @param [u] qcsba [EnsPQcsubmissionadaptor] Ensembl QC Submission Adaptor
** @param [u] qcsb [EnsPQcsubmission] Ensembl QC Submission
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcsubmissionadaptorStore(EnsPQcsubmissionadaptor qcsba,
                                   EnsPQcsubmission qcsb)
{
    AjBool value = AJFALSE;

    AjPSqlstatement sqls = NULL;

    AjPStr statement = NULL;

    EnsPDatabaseadaptor dba = NULL;

    if(!qcsba)
        return ajFalse;

    if(!qcsb)
        return ajFalse;

    if(ensQcsubmissionGetAdaptor(qcsb) && ensQcsubmissionGetIdentifier(qcsb))
        return ajFalse;

    dba = ensBaseadaptorGetDatabaseadaptor(qcsba);

    statement = ajFmtStr(
        "INSERT IGNORE INTO "
        "submission "
        "SET "
        "submission.analysis_id = %u, "
        "submission.query_db_id = %u, "
        "submission.query_id = %u, "
        "submission.target_db_id = %u, "
        "submission.target_id = %u, "
        "submission.target_start = %u, "
        "submission.target_end = %u, "
        "submission.target_strand = %d, "
        "submission.analysis_job_id = %u",
        ensAnalysisGetIdentifier(qcsb->Analysis),
        ensQcsequenceGetQcdatabaseIdentifier(qcsb->QuerySequence),
        ensQcsequenceGetIdentifier(qcsb->QuerySequence),
        ensQcsequenceGetQcdatabaseIdentifier(qcsb->TargetSequence),
        ensQcsequenceGetIdentifier(qcsb->TargetSequence),
        qcsb->TargetStart,
        qcsb->TargetEnd,
        qcsb->TargetStrand,
        qcsb->AnalysisJobIdentifier);

    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);

    if(ajSqlstatementGetAffectedrows(sqls))
    {
        ensQcsubmissionSetIdentifier(qcsb, ajSqlstatementGetIdentifier(sqls));

        ensQcsubmissionSetAdaptor(qcsb, qcsba);

        value = ajTrue;
    }

    ensDatabaseadaptorSqlstatementDel(dba, &sqls);

    ajStrDel(&statement);

    return value;
}




/* @func ensQcsubmissionadaptorUpdate *****************************************
**
** Update an Ensembl QC Submission.
**
** @param [u] qcsba [EnsPQcsubmissionadaptor] Ensembl QC Submission Adaptor
** @param [r] qcsb [const EnsPQcsubmission] Ensembl QC Submission
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcsubmissionadaptorUpdate(EnsPQcsubmissionadaptor qcsba,
                                    const EnsPQcsubmission qcsb)
{
    AjBool value = AJFALSE;

    AjPSqlstatement sqls = NULL;

    AjPStr statement = NULL;

    EnsPDatabaseadaptor dba = NULL;

    if(!qcsba)
        return ajFalse;

    if(!qcsb)
        return ajFalse;

    if(!ensQcsubmissionGetIdentifier(qcsb))
        return ajFalse;

    dba = ensBaseadaptorGetDatabaseadaptor(qcsba);

    statement = ajFmtStr(
        "UPDATE IGNORE "
        "submission "
        "SET "
        "submission.analysis_id = %u, "
        "submission.query_db_id = %u, "
        "submission.query_id = %u, "
        "submission.target_db_id = %u, "
        "submission.target_id = %u, "
        "submission.target_start = %u, "
        "submission.target_end = %u, "
        "submission.target_strand = %d, "
        "submission.analysis_job_id = %u "
        "WHERE "
        "submission.submission_id = %u",
        ensAnalysisGetIdentifier(qcsb->Analysis),
        ensQcsequenceGetQcdatabaseIdentifier(qcsb->QuerySequence),
        ensQcsequenceGetIdentifier(qcsb->QuerySequence),
        ensQcsequenceGetQcdatabaseIdentifier(qcsb->TargetSequence),
        ensQcsequenceGetIdentifier(qcsb->TargetSequence),
        qcsb->TargetStart,
        qcsb->TargetEnd,
        qcsb->TargetStrand,
        qcsb->AnalysisJobIdentifier);

    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);

    if(ajSqlstatementGetAffectedrows(sqls))
        value = ajTrue;

    ensDatabaseadaptorSqlstatementDel(dba, &sqls);

    ajStrDel(&statement);

    return value;
}




/* @func ensQcsubmissionadaptorDelete *****************************************
**
** Delete an Ensembl QC Submission.
**
** @param [u] qcsba [EnsPQcsubmissionadaptor] Ensembl QC Submission Adaptor
** @param [u] qcsb [EnsPQcsubmission] Ensembl QC Submission
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQcsubmissionadaptorDelete(EnsPQcsubmissionadaptor qcsba,
                                    EnsPQcsubmission qcsb)
{
    AjBool value = AJFALSE;

    AjPSqlstatement sqls = NULL;

    AjPStr statement = NULL;

    EnsPDatabaseadaptor dba = NULL;

    if(!qcsba)
        return ajFalse;

    if(!qcsb)
        return ajFalse;

    if(!ensQcsubmissionGetIdentifier(qcsb))
        return ajFalse;

    dba = ensBaseadaptorGetDatabaseadaptor(qcsba);

    statement = ajFmtStr(
        "DELETE FROM "
        "submission "
        "WHERE "
        "submission.submission_id = %u",
        qcsb->Identifier);

    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);

    if(ajSqlstatementGetAffectedrows(sqls))
    {
        qcsb->Adaptor    = (EnsPQcsubmissionadaptor) NULL;
        qcsb->Identifier = 0;

        value = ajTrue;
    }

    ensDatabaseadaptorSqlstatementDel(dba, &sqls);

    ajStrDel(&statement);

    return value;
}
