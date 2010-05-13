/******************************************************************************
**
** @source Ensembl Registry functions
**
** @author Copyright (C) 1999 Ensembl Developers
** @author Copyright (C) 2006 Michael K. Schuster
** @modified 2009 by Alan Bleasby for incorporation into EMBOSS core
** @version $Revision: 1.13 $
** @@
**
** Bio::EnsEMBL::Registry CVS Revision:
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

#include "ensregistry.h"




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */




/* @datastatic RegistryPIdentifier ********************************************
**
** Ensembl Registry Identifier.
**
** Holds regular expressions for matching Ensembl Gene, Transcript,
** Translation and Exon stable identifiers, as well as
** Ensembl Database Adaptor species and group information.
**
** @alias RegistrySIdentifier
** @alias RegistryOIdentifier
**
** @attr RegularExpression [AjPStr] Regular expression
** @attr SpeciesName [AjPStr] Ensembl Database Adaptor species element
** @attr Group [EnsEDatabaseadaptorGroup] Ensembl Database Adaptor group
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct RegistrySIdentifier
{
    AjPStr RegularExpression;
    AjPStr SpeciesName;
    EnsEDatabaseadaptorGroup Group;
    ajuint Padding;
} RegistryOIdentifier;

#define RegistryPIdentifier RegistryOIdentifier*




/* @datastatic RegistryPCoreStyle *********************************************
**
** Ensembl Core-Style Registry.
**
** Holds an Ensembl Database Adaptor and associated Ensembl Object Adaptors
** specific for Ensembl Core-Style databases.
**
** @alias RegistrySCoreStyle
** @alias RegistryOCoreStyle
**
** @attr Databaseadaptor [EnsPDatabaseadaptor]
**         Ensembl Database Adaptor
** @attr Referenceadaptor [EnsPDatabaseadaptor]
**         Ensembl Database Adaptor
** @attr Analysisadaptor [EnsPAnalysisadaptor]
**         Ensembl Analysis Adaptor
** @attr Assemblyexceptionadaptor [EnsPAssemblyexceptionadaptor]
**         Ensembl Assembly Exception Adaptor
** @attr Assemblyexceptionfeatureadaptor [EnsPAssemblyexceptionfeatureadaptor]
**         Ensembl Assembly Exception Feature Adaptor
** @attr Assemblymapperadaptor [EnsPAssemblymapperadaptor]
**         Ensembl Assembly Mapper Adaptor
** @attr Coordsystemadaptor [EnsPCoordsystemadaptor]
**         Ensembl Coordjnate System Adaptor
** @attr Databaseentryadaptor [EnsPDatabaseentryadaptor]
**         Ensembl Database Entry Adaptor
** @attr Densityfeatureadaptor [EnsPDensityfeatureadaptor]
**         Ensembl Density Feature Adaptor
** @attr Densitytypeadaptor [EnsPDensitytypeadaptor]
**         Ensembl Density Type Adaptor
** @attr Ditagfeatureadaptor [EnsPDitagfeatureadaptor]
**         Ensembl Ditag Feature Adaptor
** @attr DNAAlignFeatureadaptor [EnsPDNAAlignFeatureadaptor]
**         Ensembl DNA Alignment Feature Adaptor
** @attr Exonadaptor [EnsPExonadaptor]
**         Ensembl Exon Adaptor
** @attr Externaldatabaseadaptor [EnsPExternaldatabaseadaptor]
**         Ensembl External Database Adaptor
** @attr Geneadaptor [EnsPGeneadaptor]
**         Ensembl Gene Adaptor
** @attr Karyotypebandadaptor [EnsPKaryotypebandadaptor]
**         Ensembl Karyotype Band Adaptor
** @attr Markeradaptor [EnsPMarkeradaptor]
**         Ensembl Marker Adaptor
** @attr Markerfeatureadaptor [EnsPMarkerfeatureadaptor]
**         Ensembl Marker Feature Adaptor
** @attr Metacoordinateadaptor [EnsPMetacoordinateadaptor]
**         Ensembl Meta-Coordinate Adaptor
** @attr Metainformationadaptor [EnsPMetainformationadaptor]
**         Ensembl Meta-Information Adaptor
** @attr Miscellaneousfeatureadaptor [EnsPMiscellaneousfeatureadaptor]
**         Ensembl Miscellaneous Feature Adaptor
** @attr Miscellaneoussetadaptor [EnsPMiscellaneoussetadaptor]
**         Ensembl Miscellaneous Set Adaptor
** @attr Predictionexonadaptor [EnsPPredictionexonadaptor]
**         Ensembl Prediction Exon Adaptor
** @attr Predictiontranscriptadaptor [EnsPPredictiontranscriptadaptor]
**         Ensembl Prediction Transcript Adaptor
** @attr Proteinalignfeatureadaptor [EnsPProteinalignfeatureadaptor]
**         Ensembl Protein Alignment Feature Adaptor
** @attr Proteinfeatureadaptor [EnsPProteinfeatureadaptor]
**         Ensembl Protein Feature Adaptor
** @attr Repeatfeatureadaptor [EnsPRepeatfeatureadaptor]
**         Ensembl Repeat Feature Adaptor
** @attr Seqregionadaptor [EnsPSeqregionadaptor]
**         Ensembl Sequence Region Adaptor
** @attr Sequenceadaptor [EnsPSequenceadaptor]
**         Ensembl Sequence Adaptor
** @attr Simplefeatureadaptor [EnsPSimplefeatureadaptor]
**         Ensembl Simple Feature Adaptor
** @attr Sliceadaptor [EnsPSliceadaptor]
**         Ensembl Slice Adaptor
** @attr Transcriptadaptor [EnsPTranscriptadaptor]
**         Ensembl Transcript Adaptor
** @attr Translationadaptor [EnsPTranslationadaptor]
**         Ensembl Translation Adaptor
** @@
******************************************************************************/

typedef struct RegistrySCoreStyle
{
    EnsPDatabaseadaptor Databaseadaptor;
    EnsPDatabaseadaptor Referenceadaptor;
    EnsPAnalysisadaptor Analysisadaptor;
    EnsPAssemblyexceptionadaptor Assemblyexceptionadaptor;
    EnsPAssemblyexceptionfeatureadaptor Assemblyexceptionfeatureadaptor;
    EnsPAssemblymapperadaptor Assemblymapperadaptor;
    EnsPCoordsystemadaptor Coordsystemadaptor;
    EnsPDatabaseentryadaptor Databaseentryadaptor;
    EnsPDensityfeatureadaptor Densityfeatureadaptor;
    EnsPDensitytypeadaptor Densitytypeadaptor;
    EnsPDitagfeatureadaptor Ditagfeatureadaptor;
    EnsPDNAAlignFeatureadaptor DNAAlignFeatureadaptor;
    EnsPExonadaptor Exonadaptor;
    EnsPExternaldatabaseadaptor Externaldatabaseadaptor;
    EnsPGeneadaptor Geneadaptor;
    EnsPKaryotypebandadaptor Karyotypebandadaptor;
    EnsPMarkeradaptor Markeradaptor;
    EnsPMarkerfeatureadaptor Markerfeatureadaptor;
    EnsPMetacoordinateadaptor Metacoordinateadaptor;
    EnsPMetainformationadaptor Metainformationadaptor;
    EnsPMiscellaneousfeatureadaptor Miscellaneousfeatureadaptor;
    EnsPMiscellaneoussetadaptor Miscellaneoussetadaptor;
    EnsPPredictionexonadaptor Predictionexonadaptor;
    EnsPPredictiontranscriptadaptor Predictiontranscriptadaptor;
    EnsPProteinalignfeatureadaptor Proteinalignfeatureadaptor;
    EnsPProteinfeatureadaptor Proteinfeatureadaptor;
    EnsPRepeatfeatureadaptor Repeatfeatureadaptor;
    EnsPSeqregionadaptor Seqregionadaptor;
    EnsPSequenceadaptor Sequenceadaptor;
    EnsPSimplefeatureadaptor Simplefeatureadaptor;
    EnsPSliceadaptor Sliceadaptor;
    EnsPTranscriptadaptor Transcriptadaptor;
    EnsPTranslationadaptor Translationadaptor;
} RegistryOCoreStyle;

#define RegistryPCoreStyle RegistryOCoreStyle*




/* @datastatic RegistryPGeneticVariation **************************************
**
** Ensembl Genetic Variation Registry.
**
** Holds an Ensembl Database Adaptor and associated Ensembl Object Adaptors
** specific for Ensembl Genetic Variation databases.
**
** @alias RegistrySGeneticVariation
** @alias RegistryOGeneticVariation
**
** @attr Databaseadaptor [EnsPDatabaseadaptor]
**         Ensembl Database Adaptor
** @attr Alleleadaptor [EnsPGvalleleadaptor]
**         Ensembl Genetic Variation Allele Adaptor
** @attr Genotypeadaptor [EnsPGvgenotypeadaptor]
**         Ensembl Genetic Variation Genotype Adaptor
** @attr Individualadaptor [EnsPGvindividualadaptor]
**         Ensembl Genetic Variation Individual Adaptor
** @attr Populationadaptor [EnsPGvpopulationadaptor]
**         Ensembl Genetic Variation Population Adaptor
** @attr Sampleadaptor [EnsPGvvsampleadaptor]
**         Ensembl Genetic Variation Sample Adaptor
** @attr Variationadaptor [EnsPGvvariationadaptor]
**         Ensembl Genetic Variation Variation Adaptor
** @@
******************************************************************************/

typedef struct RegistrySGeneticVariation
{
    EnsPDatabaseadaptor Databaseadaptor;
    EnsPGvalleleadaptor Alleleadaptor;
    EnsPGvgenotypeadaptor Genotypeadaptor;
    EnsPGvindividualadaptor Individualadaptor;
    EnsPGvpopulationadaptor Populationadaptor;
    EnsPGvsampleadaptor Sampleadaptor;
    EnsPGvvariationadaptor Variationadaptor;
} RegistryOGeneticVariation;

#define RegistryPGeneticVariation RegistryOGeneticVariation*




/* @datastatic RegistryPFunctionalGenomics ************************************
**
** Ensembl Registry Functional Genomics.
**
** Holds an Ensembl Database Adaptors and associated Ensembl Object Adaptors
** specific for Ensembl Functional Genomics databases.
**
** @alias RegistrySFunctionalGenomics
** @alias RegistryOFunctionalGenomics
**
** @attr Databaseadaptor [EnsPDatabaseadaptor]
**         Ensembl Database Adaptor
** @@
******************************************************************************/

typedef struct RegistrySFunctionalGenomics
{
    EnsPDatabaseadaptor Databaseadaptor;
} RegistryOFunctionalGenomics;

#define RegistryPFunctionalGenomics RegistryOFunctionalGenomics*




/* @datastatic RegistryPComparativeGenomics ***********************************
**
** Ensembl Comparative Genomics Registry.
**
** Holds an Ensembl Database Adaptor and associated Ensembl Object Adaptors
** specific for Ensembl Comparative Genomics databases.
**
** @alias RegistrySComparativeGenomics
** @alias RegistryOComparativeGenomics
**
** @attr Databaseadaptor [EnsPDatabaseadaptor]
**         Ensembl Database Adaptor
** @@
******************************************************************************/

typedef struct RegistrySComparativeGenomics
{
    EnsPDatabaseadaptor Databaseadaptor;
} RegistryOComparativeGenomics;

#define RegistryPComparativeGenomics RegistryOComparativeGenomics*




/* @datastatic RegistryPOntology **********************************************
**
** Ensembl Ontology Registry.
**
** Holds an Ensembl Database Adaptor and associated Ensembl Object Adaptors
** specific for Ensembl Ontology databases.
**
** @alias RegistrySOntology
** @alias RegistryOOntology
**
** @attr Databaseadaptor [EnsPDatabaseadaptor]
**         Ensembl Database Adaptor
** @@
******************************************************************************/

typedef struct RegistrySOntology
{
    EnsPDatabaseadaptor Databaseadaptor;
} RegistryOOntology;

#define RegistryPOntology RegistryOOntology*




/* @datastatic RegistryPQualityCheck ******************************************
**
** Ensembl Quality Check Registry.
**
** Holds an Ensembl Database Adaptor and associated Ensembl Object Adaptors
** specific for Ensembl Quality Check databases.
**
** @alias RegistrySQualityCheck
** @alias RegistryOQualityCheck
**
** @attr Databaseadaptor [EnsPDatabaseadaptor]
**         Ensembl Database Adaptor
** @attr Qcdatabaseadaptor [EnsPQcdatabaseadaptor]
**         Ensembl Quality Check Database Adaptor
** @attr Sequenceadaptor [EnsPQcsequenceadaptor]
**         Ensembl Quality Check Sequence Adaptor
** @attr Alignmentadaptor [EnsPQcalignmentadaptor]
**         Ensembl Quality Check Alignment Adaptor
** @attr DASFeatureadaptor [EnsPQcdasfeatureadaptor]
**         Ensembl Quality Check DAS Feature Adaptor
** @attr Variationadaptor [EnsPQcvariationadaptor]
**         Ensembl Quality Check Variation Adaptor
** @attr Submissionadaptor [EnsPQcsubmissionadaptor]
**         Ensembl Quality Check Submission Adaptor
** @@
******************************************************************************/

typedef struct RegistrySQualityCheck
{
    EnsPDatabaseadaptor Databaseadaptor;
    EnsPQcdatabaseadaptor Qcdatabaseadaptor;
    EnsPQcsequenceadaptor Sequenceadaptor;
    EnsPQcalignmentadaptor Alignmentadaptor;
    EnsPQcdasfeatureadaptor DASFeatureadaptor;
    EnsPQcvariationadaptor Variationadaptor;
    EnsPQcsubmissionadaptor Submissionadaptor;
} RegistryOQualityCheck;

#define RegistryPQualityCheck RegistryOQualityCheck*




/* @datastatic RegistryPEntry *************************************************
**
** Ensembl Registry Entry.
**
** Holds Ensembl database-specific Registry objects.
**
** @alias RegistrySEntry
** @alias RegistryOEntry
**
** @attr Registry [void*[EnsMDatabaseadaptorGroups]] Array of database-specific
**                Registry Entries, indexed by the Ensembl Database Adaptor
**                group element EnsEDatabaseadaptorGroup.
** @@
******************************************************************************/

typedef struct RegistrySEntry
{
    void *Registry[EnsMDatabaseadaptorGroups];
} RegistryOEntry;

#define RegistryPEntry RegistryOEntry*




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

static const char *registrySoftwareVersion = "56";

static AjPTable registryAliases = NULL;

static AjPTable registryEntries = NULL;

static AjPList registryIdentifiers = NULL;

static RegistryPIdentifier registryIdentifierNew(
    AjPStr expression,
    AjPStr species,
    EnsEDatabaseadaptorGroup group);

static void registryIdentifierDel(RegistryPIdentifier *Pri);

static AjBool registryCoreStyleTrace(const RegistryPCoreStyle rcs,
                                     ajuint level);

static void registryCoreStyleDel(RegistryPCoreStyle *Prcs);

static void registryGeneticVariationDel(RegistryPGeneticVariation *Prgv);

static void registryFunctionalGenomicsDel(RegistryPFunctionalGenomics *Prfg);

static void registryComparativeGenomicsDel(RegistryPComparativeGenomics *Prcg);

static void registryOntologyDel(RegistryPOntology *Pro);

static void registryQualityCheckDel(RegistryPQualityCheck *Pqc);

static AjBool registryEntryTrace(const RegistryPEntry entry, ajuint level);

static void registryEntryDel(RegistryPEntry *Pentry);

static AjBool registryLoadCollection(EnsPDatabaseconnection dbc,
                                     AjPStr dbname);




/* @funcstatic registryIdentifierNew ******************************************
**
** Default constructor for an Ensembl Registry Identifier.
**
** @param [r] expression [AjPStr] Regular expression
** @param [r] species [AjPStr] Ensembl Database Adaptor species
** @param [r] group [EnsEDatabaseadaptorGroup] Ensembl Database Adaptor group
**
** @return [RegistryPIdentifier] Ensembl Registry Identifier or NULL
** @@
******************************************************************************/

static RegistryPIdentifier registryIdentifierNew(
    AjPStr expression,
    AjPStr species,
    EnsEDatabaseadaptorGroup group)
{
    RegistryPIdentifier ri = NULL;

    if(!expression)
        return NULL;

    if(!species)
        return NULL;

    if(!group)
        return NULL;

    AJNEW0(ri);

    ri->RegularExpression = ajStrNewS(expression);

    ri->SpeciesName = ajStrNewS(species);

    ri->Group = group;

    return ri;
}




/* @funcstatic registryIdentifierDel ******************************************
**
** Default destructor for an Ensembl Registry Identifier.
**
** @param [d] Pri [RegistryPIdentifier*] Registry Regular Identifier address
**
** @return [void]
** @@
******************************************************************************/

static void registryIdentifierDel(RegistryPIdentifier *Pri)
{
    RegistryPIdentifier pthis = NULL;

    if(!Pri)
        return;

    if(!*Pri)
        return;

    pthis = *Pri;

    ajStrDel(&pthis->RegularExpression);
    ajStrDel(&pthis->SpeciesName);

    AJFREE(pthis);

    *Pri = NULL;

    return;
}




/* @funcstatic registryCoreStyleTrace *****************************************
**
** Trace a Core-Style Registry.
**
** @param [r] rcs [const RegistryPCoreStyle] Core-Style Registry
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool registryCoreStyleTrace(const RegistryPCoreStyle rcs,
                                     ajuint level)
{
    AjPStr indent = NULL;

    if(!rcs)
        return ajFalse;

    indent = ajStrNew();

    ajStrAppendCountK(&indent, ' ', level * 2);

    ajDebug("%SregistryCoreStyleTrace %p\n"
            "%S  Databaseadaptor %p\n"
            "%S  Referenceadaptor %p\n"
            "%S  Analysisadaptor %p\n"
            "%S  Assemblyexceptionadaptor %p\n"
            "%S  Assemblyexceptionfeatureadaptor %p\n"
            "%S  Assemblymapperadaptor %p\n"
            "%S  Coordsystemadaptor %p\n"
            "%S  Databaseentryadaptor %p\n"
            "%S  DNAAlignFeatureadaptor %p\n"
            "%S  Exonadaptor %p\n"
            "%S  Externaldatabaseadaptor %p\n"
            "%S  Geneadaptor %p\n"
            "%S  Karyotypebandadaptor %p\n"
            "%S  Metainformationadaptor %p\n"
            "%S  Metacoordinateadaptor %p\n"
            "%S  Miscellaneousfeatureadaptor %p\n"
            "%S  Miscellaneoussetadaptor %p\n"
            "%S  Proteinalignfeatureadaptor %p\n"
            "%S  Proteinfeatureadaptor %p\n"
            "%S  Repeatfeatureadaptor %p\n"
            "%S  Seqregionadaptor %p\n"
            "%S  Sequenceadaptor %p\n"
            "%S  Simplefeatureadaptor %p\n"
            "%S  Sliceadaptor %p\n"
            "%S  Transcriptadaptor %p\n"
            "%S  Translationadaptor %p\n",
            indent, rcs,
            indent, rcs->Databaseadaptor,
            indent, rcs->Referenceadaptor,
            indent, rcs->Analysisadaptor,
            indent, rcs->Assemblyexceptionadaptor,
            indent, rcs->Assemblyexceptionfeatureadaptor,
            indent, rcs->Assemblymapperadaptor,
            indent, rcs->Coordsystemadaptor,
            indent, rcs->Databaseentryadaptor,
            indent, rcs->DNAAlignFeatureadaptor,
            indent, rcs->Exonadaptor,
            indent, rcs->Externaldatabaseadaptor,
            indent, rcs->Geneadaptor,
            indent, rcs->Karyotypebandadaptor,
            indent, rcs->Metainformationadaptor,
            indent, rcs->Metacoordinateadaptor,
            indent, rcs->Miscellaneousfeatureadaptor,
            indent, rcs->Miscellaneoussetadaptor,
            indent, rcs->Proteinalignfeatureadaptor,
            indent, rcs->Proteinfeatureadaptor,
            indent, rcs->Repeatfeatureadaptor,
            indent, rcs->Seqregionadaptor,
            indent, rcs->Sequenceadaptor,
            indent, rcs->Simplefeatureadaptor,
            indent, rcs->Sliceadaptor,
            indent, rcs->Transcriptadaptor,
            indent, rcs->Translationadaptor);

    ensDatabaseadaptorTrace(rcs->Databaseadaptor, level + 1);

    ajStrDel(&indent);

    return ajTrue;
}




/* @funcstatic registryCoreStyleDel *******************************************
**
** Default destructor for an Ensembl Core-Style Registry.
**
** @param [d] Prcs [RegistryPCoreStyle*] Core-Style Registry address
**
** @return [void]
** @@
******************************************************************************/

static void registryCoreStyleDel(RegistryPCoreStyle *Prcs)
{
    RegistryPCoreStyle pthis = NULL;

    if(!Prcs)
        return;

    if(!*Prcs)
        return;

    if(ajDebugTest("registryCoreStyleDel"))
    {
        ajDebug("registryCoreStyleDel\n"
                "  *Prcs %p\n",
                *Prcs);

        registryCoreStyleTrace(*Prcs, 1);
    }

    pthis = *Prcs;

    /* Delete all Ensembl Object Adaptors based on Database Adaptors. */

    ensAnalysisadaptorDel(&pthis->Analysisadaptor);

    ensAssemblyexceptionadaptorDel(&pthis->Assemblyexceptionadaptor);

    ensAssemblyexceptionfeatureadaptorDel(
        &pthis->Assemblyexceptionfeatureadaptor);

    ensAssemblymapperadaptorDel(&pthis->Assemblymapperadaptor);

    ensCoordsystemadaptorDel(&pthis->Coordsystemadaptor);

    ensDatabaseentryadaptorDel(&pthis->Databaseentryadaptor);

    ensDensityfeatureadaptorDel(&pthis->Densityfeatureadaptor);

    ensDensitytypeadaptorDel(&pthis->Densitytypeadaptor);

    ensDitagfeatureadaptorDel(&pthis->Ditagfeatureadaptor);

    ensDNAAlignFeatureadaptorDel(&pthis->DNAAlignFeatureadaptor);

    ensExonadaptorDel(&pthis->Exonadaptor);

    ensExternaldatabaseadaptorDel(&pthis->Externaldatabaseadaptor);

    ensGeneadaptorDel(&pthis->Geneadaptor);

    ensKaryotypebandadaptorDel(&pthis->Karyotypebandadaptor);

    ensMarkeradaptorDel(&pthis->Markeradaptor);

    ensMarkerfeatureadaptorDel(&pthis->Markerfeatureadaptor);

    ensMetainformationadaptorDel(&pthis->Metainformationadaptor);

    ensMetacoordinateadaptorDel(&pthis->Metacoordinateadaptor);

    ensMiscellaneousfeatureadaptorDel(&pthis->Miscellaneousfeatureadaptor);

    ensMiscellaneoussetadaptorDel(&pthis->Miscellaneoussetadaptor);

    ensPredictionexonadaptorDel(&pthis->Predictionexonadaptor);

    ensPredictiontranscriptadaptorDel(&pthis->Predictiontranscriptadaptor);

    ensProteinalignfeatureadaptorDel(&pthis->Proteinalignfeatureadaptor);

    ensProteinfeatureadaptorDel(&pthis->Proteinfeatureadaptor);

    ensRepeatfeatureadaptorDel(&pthis->Repeatfeatureadaptor);

    ensSeqregionadaptorDel(&pthis->Seqregionadaptor);

    ensSequenceadaptorDel(&pthis->Sequenceadaptor);

    ensSimplefeatureadaptorDel(&pthis->Simplefeatureadaptor);

    ensSliceadaptorDel(&pthis->Sliceadaptor);

    ensTranscriptadaptorDel(&pthis->Transcriptadaptor);

    ensTranslationadaptorDel(&pthis->Translationadaptor);

    /* Finally, delete the Ensembl Database Adaptors. */

    ensDatabaseadaptorDel(&pthis->Referenceadaptor);

    ensDatabaseadaptorDel(&pthis->Databaseadaptor);

    AJFREE(pthis);

    *Prcs = NULL;

    return;
}




/* @funcstatic registryGeneticVariationDel ************************************
**
** Default destructor for an Ensembl Genetic Variation Registry.
**
** @param [d] Prgv [RegistryPGeneticVariation*] Genetic Variation Registry
**                                              address
**
** @return [void]
** @@
******************************************************************************/

static void registryGeneticVariationDel(RegistryPGeneticVariation *Prgv)
{
    RegistryPGeneticVariation pthis = NULL;

    if(!Prgv)
        return;

    if(!*Prgv)
        return;

    pthis = *Prgv;

    /* Delete all Ensembl Object Adaptors based on the Database Adaptor. */

    ensGvindividualadaptorDel(&pthis->Individualadaptor);

    ensGvpopulationadaptorDel(&pthis->Populationadaptor);

    ensGvsampleadaptorDel(&pthis->Sampleadaptor);

    /* Finally, delete the Ensembl Database Adaptor. */

    ensDatabaseadaptorDel(&pthis->Databaseadaptor);

    AJFREE(pthis);

    *Prgv = NULL;

    return;
}




/* @funcstatic registryFunctionalGenomicsDel **********************************
**
** Default destructor for an Ensembl Functional Genomics Registry.
**
** @param [d] Prfg [RegistryPFunctionalGenomics*] Functional Genomics Registry
**                                                address
**
** @return [void]
** @@
******************************************************************************/

static void registryFunctionalGenomicsDel(RegistryPFunctionalGenomics *Prfg)
{
    RegistryPFunctionalGenomics pthis = NULL;

    if(!Prfg)
        return;

    if(!*Prfg)
        return;

    pthis = *Prfg;

    /* Delete all Ensembl Object Adaptors based on the Database Adaptor. */

    /* Finally, delete the Ensembl Database Adaptor. */

    ensDatabaseadaptorDel(&pthis->Databaseadaptor);

    AJFREE(pthis);

    *Prfg = NULL;

    return;
}




/* @funcstatic registryComparativeGenomicsDel *********************************
**
** Default destructor for an Ensembl Comparative Genomics Registry.
**
** @param [d] Prcg [RegistryPComparativeGenomics*] Comparative Genomics
**                                                 Registry address
**
** @return [void]
** @@
******************************************************************************/

static void registryComparativeGenomicsDel(RegistryPComparativeGenomics *Prcg)
{
    RegistryPComparativeGenomics pthis = NULL;

    if(!Prcg)
        return;

    if(!*Prcg)
        return;

    pthis = *Prcg;

    /* Delete all Ensembl Object Adaptors based on the Database Adaptor. */

    /* Finally, delete the Ensembl Database Adaptor. */

    ensDatabaseadaptorDel(&pthis->Databaseadaptor);

    AJFREE(pthis);

    *Prcg = NULL;

    return;
}




/* @funcstatic registryOntologyDel ********************************************
**
** Default destructor for an Ensembl Ontology Registry.
**
** @param [d] Pro [RegistryPOntology*] Ontology Registry address
**
** @return [void]
** @@
******************************************************************************/

static void registryOntologyDel(RegistryPOntology *Pro)
{
    RegistryPOntology pthis = NULL;

    if(!Pro)
        return;

    if(!*Pro)
        return;

    if(ajDebugTest("registryOntologyDel"))
        ajDebug("registryOntologyDel\n"
                "  *Pro %p\n",
                *Pro);

    pthis = *Pro;

    /* Delete all Ensembl Object Adaptors based on the Database Adaptor. */

    /* Finally, delete the Ensembl Database Adaptor. */

    ensDatabaseadaptorDel(&pthis->Databaseadaptor);

    AJFREE(pthis);

    *Pro = NULL;

    return;
}




/* @funcstatic registryQualityCheckDel ****************************************
**
** Default destructor for an Ensembl Quality Check Registry.
**
** @param [d] Pqc [RegistryPQualityCheck*] Quality Check Registry address
**
** @return [void]
** @@
******************************************************************************/

static void registryQualityCheckDel(RegistryPQualityCheck *Pqc)
{
    RegistryPQualityCheck pthis = NULL;

    if(!Pqc)
        return;

    if(!*Pqc)
        return;

    pthis = *Pqc;

    /* Delete all Ensembl Object Adaptors based on the Database Adaptor. */

    ensQcdatabaseadaptorDel(&pthis->Qcdatabaseadaptor);

    ensQcsequenceadaptorDel(&pthis->Sequenceadaptor);

    ensQcalignmentadaptorDel(&pthis->Alignmentadaptor);

    ensQcdasfeatureadaptorDel(&pthis->DASFeatureadaptor);

    ensQcvariationadaptorDel(&pthis->Variationadaptor);

    ensQcsubmissionadaptorDel(&pthis->Submissionadaptor);

    /* Finally, delete the Ensembl Database Adaptor. */

    ensDatabaseadaptorDel(&pthis->Databaseadaptor);

    AJFREE(pthis);

    *Pqc = NULL;

    return;
}




/* @funcstatic registryEntryTrace *********************************************
**
** Trace a Registry Entry.
**
** @param [r] entry [const RegistryPEntry] Registry Entry
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool registryEntryTrace(const RegistryPEntry entry, ajuint level)
{
    AjPStr indent = NULL;

    if(!entry)
        return ajFalse;

    indent = ajStrNew();

    ajStrAppendCountK(&indent, ' ', level * 2);

    ajDebug("%SregistryEntryTrace %p\n"
            "%S  Registry[0] NULL %p\n"
            "%S  Registry[1] Core %p\n"
            "%S  Registry[2] Vega %p\n"
            "%S  Registry[3] Other Features %p\n"
            "%S  Registry[4] Copy DNA %p\n"
            "%S  Registry[5] Genetic Variation %p\n"
            "%S  Registry[6] Functional Genomics %p\n"
            "%S  Registry[7] Comparative Genomics %p\n"
            "%S  Registry[8] Gene Ontology %p\n"
            "%S  Registry[9] Quality Check %p\n",
            indent, entry,
            indent, entry->Registry[0],
            indent, entry->Registry[1],
            indent, entry->Registry[2],
            indent, entry->Registry[3],
            indent, entry->Registry[4],
            indent, entry->Registry[5],
            indent, entry->Registry[6],
            indent, entry->Registry[7],
            indent, entry->Registry[8],
            indent, entry->Registry[9]);

    ajStrDel(&indent);

    return ajTrue;
}




/* @funcstatic registryEntryDel ***********************************************
**
** Default destructor for an Ensembl Registry Entry.
**
** @param [d] Pentry [RegistryPEntry*] Ensembl Registry Entry address
**
** @return [void]
** @@
******************************************************************************/

static void registryEntryDel(RegistryPEntry *Pentry)
{
    RegistryPEntry pthis = NULL;

    if(!Pentry)
        return;

    if(!*Pentry)
        return;

    if(ajDebugTest("registryEntryDel"))
    {
        ajDebug("registryEntryDel\n"
                "  *Pentry %p\n",
                *Pentry);

        registryEntryTrace(*Pentry, 1);
    }

    pthis = *Pentry;

    registryCoreStyleDel((RegistryPCoreStyle *)
                         &pthis->Registry[ensEDatabaseadaptorGroupCore]);

    registryCoreStyleDel((RegistryPCoreStyle *)
                         &pthis->Registry[ensEDatabaseadaptorGroupVega]);

    registryCoreStyleDel((RegistryPCoreStyle *)
                         &pthis->Registry[ensEDatabaseadaptorGroupOtherFeatures]);

    registryCoreStyleDel((RegistryPCoreStyle *)
                         &pthis->Registry[ensEDatabaseadaptorGroupCopyDNA]);

    registryGeneticVariationDel((RegistryPGeneticVariation *)
                                &pthis->Registry[ensEDatabaseadaptorGroupGeneticVariation]);

    registryFunctionalGenomicsDel((RegistryPFunctionalGenomics *)
                                  &pthis->Registry[ensEDatabaseadaptorGroupFunctionalGenomics]);

    registryComparativeGenomicsDel((RegistryPComparativeGenomics *)
                                   &pthis->Registry[ensEDatabaseadaptorGroupComparativeGenomics]);

    registryOntologyDel((RegistryPOntology *)
                        &pthis->Registry[ensEDatabaseadaptorGroupOntology]);

    registryQualityCheckDel((RegistryPQualityCheck *)
                            &pthis->Registry[ensEDatabaseadaptorGroupQualityCheck]);

    AJFREE(pthis);

    *Pentry = NULL;

    return;
}




/* @filesection ensregistry ***************************************************
**
** @nam1rule ens Function belongs to the AJAX Ensembl library
** @nam2rule Registry Ensembl Registry objects
**
******************************************************************************/




/* @datasection [none] Ensembl Registry ********************************
**
** Functions for Ensembl Registry
**
**
******************************************************************************/




/* @section functions *********************************************************
**
** @fdata [none]
** @fcategory misc
**
******************************************************************************/




/* @func ensRegistryInit ******************************************************
**
** Initialises the Ensembl Registry.
**
** @return [void]
** @@
******************************************************************************/

void ensRegistryInit(void)
{
    registryAliases = ajTablestrNewCaseLen(0);

    registryEntries = ajTablestrNewLen(0);

    registryIdentifiers = ajListNew();

    return;
}




/* @func ensRegistryClear *****************************************************
**
** Clear the Ensembl Registry.
**
** @return [void]
** @@
******************************************************************************/

void ensRegistryClear(void)
{
    void **keyarray = NULL;
    void **valarray = NULL;

    register ajuint i = 0;

    ajTableToarrayKeysValues(registryEntries, &keyarray, &valarray);

    for(i = 0; keyarray[i]; i++)
    {
        ajTableRemove(registryEntries, (const void *) keyarray[i]);

        ajStrDel((AjPStr *) &keyarray[i]);

        registryEntryDel((RegistryPEntry *) &valarray[i]);
    }

    AJFREE(keyarray);
    AJFREE(valarray);

    return;
}




/* @func ensRegistryExit ******************************************************
**
** Frees the Ensembl Registry.
**
** @return [void]
** @@
******************************************************************************/

void ensRegistryExit(void)
{
    RegistryPIdentifier ri = NULL;

    /* Free the AJAX Table of aliases. */

    ajTablestrFree(&registryAliases);

    /* Clear and free the AJAX Table of Registry Entries. */

    ensRegistryClear();

    ajTableFree(&registryEntries);

    /* Clear and free the AJAX List of Registry Identifiers. */

    while(ajListPop(registryIdentifiers, (void **) &ri))
        registryIdentifierDel(&ri);

    ajListFree(&registryIdentifiers);

    return;
}




/* @func ensRegistryAddAlias **************************************************
**
** Add an alias for a (scientific) species name to the Ensembl Registry.
**
** @param [r] species [const AjPStr] Species
** @param [r] alias [const AjPStr] Alias
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRegistryAddAlias(const AjPStr species, const AjPStr alias)
{
    if(!species)
        return ajFalse;

    if(!alias)
        return ajFalse;

    if(ajDebugTest("ensRegistryAddAlias"))
        ajDebug("ensRegistryAddAlias\n"
                "  species '%S'\n"
                "  alias '%S'\n",
                species,
                alias);

    if(ajTableFetch(registryAliases, (const void *) alias))
    {
        ajDebug("ensRegistryAddAlias has already added alias '%S'.\n", alias);

        return ajFalse;
    }

    ajTablePut(registryAliases,
               (void *) ajStrNewS(alias),
               (void *) ajStrNewS(species));

    return ajTrue;
}




/* @func ensRegistryRemoveAlias ***********************************************
**
** Remove an alias from the Ensembl Registry.
**
** @param [r] alias [const AjPStr] Alias
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRegistryRemoveAlias(const AjPStr alias)
{
    AjPStr key = NULL;
    AjPStr val = NULL;

    if(!alias)
        return ajFalse;

    val = (AjPStr) ajTableRemoveKey(registryAliases,
                                    (const void *) alias,
                                    (void **) &key);

    ajStrDel(&key);
    ajStrDel(&val);

    return ajTrue;
}




/* @func ensRegistryGetSpecies ************************************************
**
** Get the species name to an alias from the Ensembl Registry.
**
** @param [r] alias [const AjPStr] Alias
**
** @return [AjPStr] Species name or NULL
** @@
******************************************************************************/

AjPStr ensRegistryGetSpecies(const AjPStr alias)
{
    if(!alias)
        return NULL;

    return (AjPStr) ajTableFetch(registryAliases, (const void *) alias);
}




/* @func ensRegistryLoadAliases ***********************************************
**
** Read aliases for (scientific) species names from an "EnsemblAliases.dat"
** data file and add them to the Ensembl Registry.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRegistryLoadAliases(void)
{
    AjBool block = AJFALSE;
    AjBool debug = AJFALSE;

    AjPFile infile = NULL;

    AjPStr line    = NULL;
    AjPStr species = NULL;

    debug = ajDebugTest("ensRegistryLoadAliases");

    if(debug)
        ajDebug("ensRegistryLoadAliases\n");

    infile = ajDatafileNewInNameC("EnsemblAliases.dat");

    if(!infile)
    {
        ajWarn("ensRegistryLoadAliases could no load "
               "EnsemblAliases.dat file.");

        return ajFalse;
    }

    line = ajStrNew();

    species = ajStrNew();

    while(ajReadlineTrim(infile, &line))
    {
        /*
        ** Remove excess white space and ignore all lines that start with
        ** a '#' sign. Commented out lines, however, should not break blocks.
        */

        ajStrRemoveWhiteSpaces(&line);

        if(ajStrGetCharFirst(line) == '#')
            continue;

        /*
        ** Remove comments on the same line as an alias, before again removing
        ** excess white space. Finally, remove the quotes, if any, and
        ** register the remaining string as alias.
        */

        ajStrCutComments(&line);

        ajStrQuoteStrip(&line);

        ajStrRemoveWhiteSpaces(&line);

        if(!ajStrGetLen(line))
        {
            /* An empty line indicates the end of a block. */

            block = ajFalse;

            continue;
        }

        if(block)
        {
            /* This is a subsequent line of a block, which is an alias. */

            ensRegistryAddAlias(species, line);
        }
        else
        {
            /* This is the first line of a block, which is the species. */

            ajStrAssignS(&species, line);

            ensRegistryAddAlias(species, species);

            block = ajTrue;
        }
    }

    ajStrDel(&species);
    ajStrDel(&line);

    ajFileClose(&infile);

    if(debug)
    {
        ajDebug("ensRegistryLoadAliases\n");

        ensRegistryTraceAliases(1);
    }

    return ajTrue;
}




/* @func ensRegistryTraceAliases **********************************************
**
** Trace Ensembl Registry Aliases.
**
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRegistryTraceAliases(ajuint level)
{
    AjPStr indent = NULL;

    indent = ajStrNew();

    ajStrAppendCountK(&indent, ' ', level * 2);

    ajDebug("%SensRegistryTraceAliases %p\n",
            indent, registryAliases);

    ajTablestrTrace(registryAliases);

    ajStrDel(&indent);

    return ajTrue;
}




/* @func ensRegistryTraceEntries **********************************************
**
** Trace Ensembl Registry Aliases.
**
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRegistryTraceEntries(ajuint level)
{
    void **keyarray = NULL;
    void **valarray = NULL;

    register ajuint i = 0;

    AjPStr indent = NULL;

    indent = ajStrNew();

    ajStrAppendCountK(&indent, ' ', level * 2);

    ajDebug("%SensRegistryTraceEntries %p\n",
            indent, registryEntries);

    ajTableToarrayKeysValues(registryEntries, &keyarray, &valarray);

    for(i = 0; keyarray[i]; i++)
    {
        ajDebug("%S  Species '%S'\n", indent, (AjPStr) keyarray[i]);

        registryEntryTrace((RegistryPEntry) valarray[i], level + 2);
    }

    AJFREE(keyarray);
    AJFREE(valarray);

    ajStrDel(&indent);

    return ajTrue;
}




/* @func ensRegistryAddDatabaseadaptor ****************************************
**
** Add an Ensembl Database Adaptor to the Ensembl Registry.
**
** @param [r] dbc [EnsPDatabaseconnection] Ensembl Database Connection
** @param [u] database [AjPStr] Database name (optional). If not provided, the
**                              database name in the Database Connection will
**                              be used.
** @param [u] alias [AjPStr] Species name or alias
** @param [r] group [EnsEDatabaseadaptorGroup] Group
** @param [r] multi [AjBool] Multiple species
** @param [r] identifier [ajuint] Species identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
** This function will aim to resolve an eventual alias to a valid species name.
** If the alias has not been registered before, it will strip underscores from
** the alias to directly permit an Ensembl database name prefix, such as
** "homo_sapiens". If an alias without underscores has also not been registered
** before, it will register the alias without underscores as species and the
** the prefix with underscores as species.
** Ideally aliases have been loaded from the corresponding Ensembl data file
** (EnsemblAliases.dat) before.
******************************************************************************/

AjBool ensRegistryAddDatabaseadaptor(EnsPDatabaseconnection dbc,
                                     AjPStr database,
                                     AjPStr alias,
                                     EnsEDatabaseadaptorGroup group,
                                     AjBool multi,
                                     ajuint identifier)
{
    AjBool registered = AJFALSE;

    AjPStr species = NULL;
    AjPStr temp    = NULL;

    RegistryPEntry entry             = NULL;
    RegistryPCoreStyle rcs           = NULL;
    RegistryPGeneticVariation rgv    = NULL;
    RegistryPFunctionalGenomics rfg  = NULL;
    RegistryPComparativeGenomics rcg = NULL;
    RegistryPOntology ro             = NULL;

    if(!dbc)
        return ajFalse;

    if(!database)
        return ajFalse;

    if(!alias)
        return ajFalse;

    if(ajDebugTest("ensRegistryAddDatabaseadaptor"))
    {
        ajDebug("ensRegistryAddDatabaseadaptor\n"
                "  dbc %p\n"
                "  database '%S'\n"
                "  alias '%S'\n"
                "  group '%s'\n"
                "  multi %B\n"
                "  identifier %u\n",
                dbc,
                database,
                alias,
                ensDatabaseadaptorGroupToChar(group),
                multi,
                identifier);

        ensDatabaseconnectionTrace(dbc, 1);
    }

    /*
    ** Resolve an eventual species alias to the species name.
    ** Check if the alias eventually an Ensembl database name prefix with
    ** underscores (e. g. 'homo_sapiens') has already been registered as an
    ** alias before.
    */

    species = ensRegistryGetSpecies(alias);

    if(!species)
    {
        /*
        ** If this was not the case check if the alias without
        ** underscores has been registered as an alias before.
        */

        temp = ajStrNewS(alias);

        ajStrExchangeCC(&temp, "_", " ");

        species = ensRegistryGetSpecies(temp);

        if(!species)
        {
            /*
            ** If the alias without underscores has also not been registered
            ** as an alias before, register it as species and alias before
            ** registering it as species with the underscores as an alias.
            */

            ensRegistryAddAlias(temp, temp);
            ensRegistryAddAlias(temp, alias);

            species = ensRegistryGetSpecies(alias);
        }

        ajStrDel(&temp);
    }

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries, (const void *) species);

    if(!entry)
    {
        AJNEW0(entry);

        ajTablePut(registryEntries,
                   (void *) ajStrNewRef(species),
                   (void *) entry);
    }

    switch(group)
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            if(entry->Registry[group])
                rcs = (RegistryPCoreStyle) entry->Registry[group];
            else
            {
                AJNEW0(rcs);

                entry->Registry[group] = (void *) rcs;
            }

            if(rcs->Databaseadaptor)
            {
                if(!ensDatabaseadaptorMatchComponents(rcs->Databaseadaptor,
                                                      dbc,
                                                      species,
                                                      group,
                                                      multi,
                                                      identifier))
                    ajWarn("ensRegistryAddDatabaseadaptor already has an "
                           "Ensembl Database Adaptor of the same "
                           "species '%S' and group '%s' registered.",
                           species,
                           ensDatabaseadaptorGroupToChar(group));

                registered = ajFalse;
            }
            else
            {
                rcs->Databaseadaptor =
                    ensDatabaseadaptorNew(dbc,
                                          database,
                                          species,
                                          group,
                                          multi,
                                          identifier);

                registered = ajTrue;
            }

            break;

        case ensEDatabaseadaptorGroupGeneticVariation:

            if(entry->Registry[group])
                rgv = (RegistryPGeneticVariation) entry->Registry[group];
            else
            {
                AJNEW0(rgv);

                entry->Registry[group] = (void *) rgv;
            }

            if(rgv->Databaseadaptor)
            {
                if(!ensDatabaseadaptorMatchComponents(rgv->Databaseadaptor,
                                                      dbc,
                                                      species,
                                                      group,
                                                      multi,
                                                      identifier))
                    ajWarn("ensRegistryAddDatabaseadaptor already has an "
                           "Ensembl Database Adaptor of the same "
                           "species '%S' and group '%s' registered.",
                           species,
                           ensDatabaseadaptorGroupToChar(group));

                registered = ajFalse;
            }
            else
            {
                rgv->Databaseadaptor =
                    ensDatabaseadaptorNew(dbc,
                                          database,
                                          species,
                                          group,
                                          multi,
                                          identifier);

                registered = ajTrue;
            }

            break;

        case ensEDatabaseadaptorGroupFunctionalGenomics:

            if(entry->Registry[group])
                rfg = (RegistryPFunctionalGenomics) entry->Registry[group];
            else
            {
                AJNEW0(rfg);

                entry->Registry[group] = (void *) rfg;
            }

            if(rfg->Databaseadaptor)
            {
                if(!ensDatabaseadaptorMatchComponents(rfg->Databaseadaptor,
                                                      dbc,
                                                      species,
                                                      group,
                                                      multi,
                                                      identifier))
                    ajWarn("ensRegistryAddDatabaseadaptor already has an "
                           "Ensembl Database Adaptor of the same "
                           "species '%S' and group '%s' registered.",
                           species,
                           ensDatabaseadaptorGroupToChar(group));

                registered = ajFalse;
            }
            else
            {
                rfg->Databaseadaptor =
                    ensDatabaseadaptorNew(dbc,
                                          database,
                                          species,
                                          group,
                                          multi,
                                          identifier);

                registered = ajTrue;
            }

            break;

        case ensEDatabaseadaptorGroupComparativeGenomics:

            if(entry->Registry[group])
                rcg = (RegistryPComparativeGenomics) entry->Registry[group];
            else
            {
                AJNEW0(rcg);

                entry->Registry[group] = (void *) rcg;
            }

            if(rcg->Databaseadaptor)
            {
                if(!ensDatabaseadaptorMatchComponents(rcg->Databaseadaptor,
                                                      dbc,
                                                      species,
                                                      group,
                                                      multi,
                                                      identifier))
                    ajWarn("ensRegistryAddDatabaseadaptor already has an "
                           "Ensembl Database Adaptor of the same "
                           "species '%S' and group '%s' registered.",
                           species,
                           ensDatabaseadaptorGroupToChar(group));

                registered = ajFalse;
            }
            else
            {
                rcg->Databaseadaptor =
                    ensDatabaseadaptorNew(dbc,
                                          database,
                                          species,
                                          group,
                                          multi,
                                          identifier);

                registered = ajTrue;
            }

            break;

        case ensEDatabaseadaptorGroupOntology:

            if(entry->Registry[group])
                ro = (RegistryPOntology) entry->Registry[group];
            else
            {
                AJNEW0(ro);

                entry->Registry[group] = (void *) ro;
            }

            if(ro->Databaseadaptor)
            {
                if(!ensDatabaseadaptorMatchComponents(ro->Databaseadaptor,
                                                      dbc,
                                                      species,
                                                      group,
                                                      multi,
                                                      identifier))
                    ajWarn("ensRegistryAddDatabaseadaptor already has an "
                           "Ensembl Database Adaptor of the same "
                           "species '%S' and group '%s' registered.",
                           species,
                           ensDatabaseadaptorGroupToChar(group));

                registered = ajFalse;
            }
            else
            {
                ro->Databaseadaptor =
                    ensDatabaseadaptorNew(dbc,
                                          database,
                                          species,
                                          group,
                                          multi,
                                          identifier);

                registered = ajTrue;
            }

            break;

        case ensEDatabaseadaptorGroupQualityCheck:

        case ensEDatabaseadaptorGroupPipeline:

        case ensEDatabaseadaptorGroupHive:

        case ensEDatabaseadaptorGroupCoreExpressionEST:

        case ensEDatabaseadaptorGroupCoreExpressionGNF:

        case ensEDatabaseadaptorGroupAncestral:

        case ensEDatabaseadaptorGroupWebsite:

            break;

        default:

            ajWarn("ensRegistryAddDatabaseadaptor got a request for an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n", group);

            registered = ajFalse;
    }

    return registered;
}




/* @func ensRegistryRemoveDatabaseadaptor *************************************
**
** Remove an Ensembl Database Adaptor from the Ensembl Registry and delete it.
**
** @param [d] Pdba [EnsPDatabaseadaptor*] Ensembl Database Adaptor address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRegistryRemoveDatabaseadaptor(EnsPDatabaseadaptor *Pdba)
{
    EnsEDatabaseadaptorGroup group = ensEDatabaseadaptorGroupNULL;

    RegistryPEntry entry             = NULL;
    RegistryPCoreStyle rcs           = NULL;
    RegistryPGeneticVariation rgv    = NULL;
    RegistryPFunctionalGenomics rfg  = NULL;
    RegistryPComparativeGenomics rcg = NULL;
    RegistryPOntology ro             = NULL;

    if(!Pdba)
        return ajFalse;

    if(!*Pdba)
        return ajFalse;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(*Pdba));

    if(!entry)
    {
        ajWarn("ensRegistryRemoveDatabaseadaptor could not get "
               "a Registry Entry for species '%S'.\n",
               ensDatabaseadaptorGetSpecies(*Pdba));

        *Pdba = (EnsPDatabaseadaptor) NULL;

        return ajTrue;
    }

    group = ensDatabaseadaptorGetGroup(*Pdba);

    switch(group)
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle) entry->Registry[group];

            if(rcs)
            {
                if(rcs->Databaseadaptor == *Pdba)
                    registryCoreStyleDel((RegistryPCoreStyle*)
                                         &entry->Registry[group]);
            }
            else
                ajWarn("ensRegistryRemoveDatabaseadaptor got "
                       "an Ensembl Database Adaptor, which is not "
                       "registered?\n");

            break;

        case ensEDatabaseadaptorGroupGeneticVariation:

            rgv = (RegistryPGeneticVariation) entry->Registry[group];

            if(rgv)
            {
                if(rgv->Databaseadaptor == *Pdba)
                    registryGeneticVariationDel((RegistryPGeneticVariation *)
                                                &entry->Registry[group]);
            }
            else
                ajWarn("ensRegistryRemoveDatabaseadaptor got "
                       "an Ensembl Database Adaptor, which is not "
                       "registered?\n");

            break;

        case ensEDatabaseadaptorGroupFunctionalGenomics:

            rfg = (RegistryPFunctionalGenomics) entry->Registry[group];

            if(rfg)
            {
                if(rfg->Databaseadaptor == *Pdba)
                    registryFunctionalGenomicsDel(
                        (RegistryPFunctionalGenomics *)
                        &entry->Registry[group]);
            }
            else
                ajWarn("ensRegistryRemoveDatabaseadaptor got "
                       "an Ensembl Database Adaptor, which is not "
                       "registered?\n");

            break;

        case ensEDatabaseadaptorGroupComparativeGenomics:

            rcg = (RegistryPComparativeGenomics) entry->Registry[group];

            if(rcg)
            {
                if(rcg->Databaseadaptor == *Pdba)
                    registryComparativeGenomicsDel(
                        (RegistryPComparativeGenomics *)
                        &entry->Registry[group]);
            }
            else
                ajWarn("ensRegistryRemoveDatabaseadaptor got "
                       "an Ensembl Database Adaptor, which is not "
                       "registered?\n");

            break;

        case ensEDatabaseadaptorGroupOntology:

            ro = (RegistryPOntology) entry->Registry[group];

            if(ro)
            {
                if(ro->Databaseadaptor == *Pdba)
                    registryOntologyDel((RegistryPOntology *)
                                        &entry->Registry[group]);
            }
            else
                ajWarn("ensRegistryRemoveDatabaseadaptor got "
                       "an Ensembl Database Adaptor, which is not "
                       "registered?\n");

            break;

        case ensEDatabaseadaptorGroupQualityCheck:

        case ensEDatabaseadaptorGroupPipeline:

        case ensEDatabaseadaptorGroupHive:

        case ensEDatabaseadaptorGroupCoreExpressionEST:

        case ensEDatabaseadaptorGroupCoreExpressionGNF:

        case ensEDatabaseadaptorGroupAncestral:

        case ensEDatabaseadaptorGroupWebsite:

            break;

        default:

            ajWarn("ensRegistryRemoveDatabaseadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n", group);
    }

    /*
    ** TODO: Test if this entry remains completely empty.
    ** If this was the case it could be removed.
    ** Should we also automatically delete all Databaseadaptors that get the
    ** DNA via the Core adaptor in case the Core adaptor gets deleted?
    */

    /* Clear the Ensembl Database Adaptor pointer. */

    *Pdba = (EnsPDatabaseadaptor) NULL;

    return ajTrue;
}




/* @func ensRegistryGetAllDatabaseadaptors ************************************
**
** Get all Ensembl Database Adaptors from the Ensembl Registry
** and optionally filter them by species or group elements.
**
** @param [rN] group [EnsEDatabaseadaptorGroup] Ensembl Database Adaptor group
** @param [rN] alias [const AjPStr] Ensembl Database Adaptor species
** @param [w] dbas [AjPList] AJAX List of Ensembl Database Adaptors
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRegistryGetAllDatabaseadaptors(EnsEDatabaseadaptorGroup group,
                                         const AjPStr alias,
                                         AjPList dbas)
{
    void **keyarray = NULL;
    void **valarray = NULL;

    register ajuint i = 0;
    register EnsEDatabaseadaptorGroup j = ensEDatabaseadaptorGroupNULL;

    AjPStr species = NULL;

    EnsPDatabaseadaptor dba = NULL;

    RegistryPCoreStyle rcs           = NULL;
    RegistryPGeneticVariation rgv    = NULL;
    RegistryPFunctionalGenomics rfg  = NULL;
    RegistryPComparativeGenomics rcg = NULL;
    RegistryPOntology ro             = NULL;

    if(!dbas)
        return ajFalse;

    species = ensRegistryGetSpecies(alias);

    ajTableToarrayKeysValues(registryEntries, &keyarray, &valarray);

    for(i = 0; keyarray[i]; i++)
    {
        for(j = 1; j <= EnsMDatabaseadaptorGroups; j++)
        {
            if(group && (group != j))
                continue;

            switch(j)
            {
                case ensEDatabaseadaptorGroupCore:

                case ensEDatabaseadaptorGroupVega:

                case ensEDatabaseadaptorGroupOtherFeatures:

                case ensEDatabaseadaptorGroupCopyDNA:

                    rcs = (RegistryPCoreStyle)
                        (((RegistryPEntry) valarray[i])->Registry[j]);

                    if(rcs && rcs->Databaseadaptor)
                    {
                        dba = rcs->Databaseadaptor;

                        if(alias)
                        {
                            if(ajStrMatchS(species,
                                           ensDatabaseadaptorGetSpecies(dba)))
                                ajListPushAppend(dbas, (void *) dba);
                        }
                        else
                            ajListPushAppend(dbas, (void *) dba);
                    }

                    break;

                case ensEDatabaseadaptorGroupGeneticVariation:

                    rgv = (RegistryPGeneticVariation)
                        (((RegistryPEntry) valarray[i])->Registry[j]);

                    if(rgv && rgv->Databaseadaptor)
                    {
                        dba = rgv->Databaseadaptor;

                        if(alias)
                        {
                            if(ajStrMatchS(species,
                                           ensDatabaseadaptorGetSpecies(dba)))
                                ajListPushAppend(dbas, (void *) dba);
                        }
                        else
                            ajListPushAppend(dbas, (void *) dba);
                    }

                    break;

                case ensEDatabaseadaptorGroupFunctionalGenomics:

                    rfg = (RegistryPFunctionalGenomics)
                        (((RegistryPEntry) valarray[i])->Registry[j]);

                    if(rfg && rfg->Databaseadaptor)
                    {
                        dba = rfg->Databaseadaptor;

                        if(alias)
                        {
                            if(ajStrMatchS(species,
                                           ensDatabaseadaptorGetSpecies(dba)))
                                ajListPushAppend(dbas, (void *) dba);
                        }
                        else
                            ajListPushAppend(dbas, (void *) dba);
                    }

                    break;

                case ensEDatabaseadaptorGroupComparativeGenomics:

                    rcg = (RegistryPComparativeGenomics)
                        (((RegistryPEntry) valarray[i])->Registry[j]);

                    if(rcg && rcg->Databaseadaptor)
                    {
                        dba = rcg->Databaseadaptor;

                        if(alias)
                        {
                            if(ajStrMatchS(species,
                                           ensDatabaseadaptorGetSpecies(dba)))
                                ajListPushAppend(dbas, (void *) dba);
                        }
                        else
                            ajListPushAppend(dbas, (void *) dba);
                    }

                    break;

                case ensEDatabaseadaptorGroupOntology:

                    ro = (RegistryPOntology)
                        (((RegistryPEntry) valarray[i])->Registry[j]);

                    if(ro && ro->Databaseadaptor)
                    {
                        dba = ro->Databaseadaptor;

                        if(alias)
                        {
                            if(ajStrMatchS(species,
                                           ensDatabaseadaptorGetSpecies(dba)))
                                ajListPushAppend(dbas, (void *) dba);
                        }
                        else
                            ajListPushAppend(dbas, (void *) dba);
                    }

                    break;

                case ensEDatabaseadaptorGroupQualityCheck:

                case ensEDatabaseadaptorGroupPipeline:

                case ensEDatabaseadaptorGroupHive:

                case ensEDatabaseadaptorGroupCoreExpressionEST:

                case ensEDatabaseadaptorGroupCoreExpressionGNF:

                case ensEDatabaseadaptorGroupAncestral:

                case ensEDatabaseadaptorGroupWebsite:

                    break;

                default:

                    ajWarn("ensRegistryGetAllDatabaseadaptors got an"
                           "unexpected group %d.\n", j);
            }
        }
    }

    AJFREE(keyarray);
    AJFREE(valarray);

    return ajTrue;
}




/* @datasection [EnsPDatabaseadaptor] Ensembl Registry database adaptor *******
**
** Functions for Ensembl Registry
**
**
******************************************************************************/




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Registry object.
**
** @fdata [EnsPDatabaseadaptor]
** @fnote None
**
** @nam3rule Get Return Registry attribute(s)
** @nam4rule GetAnalysisadaptor Return the
**             Ensembl Analysis Adaptor
** @nam4rule GetAssemblyexceptionadaptor Return the
**             Ensembl Assembly Exception Adaptor
** @nam4rule GetAssemblyexceptionfeatureadaptor Return the
**             Ensembl Assembly Exception Feature Adaptor
** @nam4rule GetAssemblymapperadaptor Return the
**             Ensembl Assembly Mapper Adaptor
** @nam4rule GetAttributeadaptor Return the
**             Ensembl Attribute Adaptor
** @nam4rule GetCoordsystemadaptor Return the
**             Ensembl Coordinate System Adaptor
** @nam4rule GetDatabaseentryadaptor Return the
**             Ensembl Database Entry Adaptor
** @nam4rule GetDensityfeatureadaptor Return the
**             Ensembl Density Feature Adaptor
** @nam4rule GetDensitytypeadaptor Return the
**             Ensembl Density Type Adaptor
** @nam4rule GetDitagadaptor Return the
**             Ensembl Ditag Adaptor
** @nam4rule GetDitagfeatureadaptor Return the
**             Ensembl Ditag Feature Adaptor
** @nam4rule GetDNAAlignFeatureadaptor Return the
**             Ensembl DNA Align Feature Adaptor
** @nam4rule GetExonadaptor Return the
**             Ensembl Exon Adaptor
** @nam4rule GetExternaldatabaseadaptor Return the
**             Ensembl External Database Adaptor
** @nam4rule GetGeneadaptor Return the
**             Ensembl Gene Adaptor
** @nam4rule GetKaryotypebandadaptor Return the
**             Ensembl Karyotype Band Adaptor
** @nam4rule GetMarkeradaptor Return the
**             Ensembl Marker Adaptor
** @nam4rule GetMarkerfeatureadaptor Return the
**             Ensembl Marker Feature Adaptor
** @nam4rule GetMetacoordinateadaptor Return the
**            Ensembl Meta-Coordinate Adaptor
** @nam4rule GetMetainformationadaptor Return the
**            Ensembl Meta-Information Adaptor
** @nam4rule GetMiscellaneousfeatureadaptor Return the
**             Ensembl Miscellaneous Feature Adaptor
** @nam4rule GetMiscellaneoussetadaptor Return the
**             Ensembl Miscellaneous Set Adaptor
** @nam4rule GetPredictionexonadaptor Return the
**             Ensembl Prediction Exon Adaptor
** @nam4rule GetPredictiontransciptadaptor Return the
**             Ensembl Prediction Transcript Adaptor
** @nam4rule GetProteinalignfeatureadaptor Return the
**             Ensembl Protein Align Feature Adaptor
** @nam4rule GetProteinfeatureadaptor Return the
**             Ensembl Protein Feature Adaptor
** @nam4rule GetRepeatconsensusadaptor Return the
**             Ensembl Repeat Consensus Adaptor
** @nam4rule GetRepeatfeatureadaptor Return the
**             Ensembl Repeat Feature Adaptor
** @nam4rule GetSeqregionadaptor Return the
**             Ensembl Sequence Region Adaptor
** @nam4rule GetSimplefeatureadaptor Return the
**             Ensembl Simple Feature Adaptor
** @nam4rule GetSequenceadaptor Return the
**             Ensembl Sequence Adaptor
** @nam4rule GetSliceadaptor Return the
**             Ensembl Slice Adaptor
** @nam4rule GetTranscriptadaptor Return the
**             Ensembl Transcript Adaptor
** @nam4rule GetTranslationadaptor Return the
**             Ensembl Translation Adaptor
** @nam4rule GetQcdatabaseadaptor Return the
**             Ensembl Quality Check Database Adaptor
** @nam4rule GetQcsequenceadaptor Return the
**             Ensembl Quality Check Sequence Adaptor
** @nam4rule GetQcalignmentadaptor Return the
**             Ensembl Quality Check Alignment Adaptor
** @nam4rule GetQcdasfeatureadaptor Return the
**             Ensembl Quality Check DAS Feature Adaptor
** @nam4rule GetQcvariationadaptor Return the
**             Ensembl Quality Check Variation Adaptor
** @nam4rule GetQcsubmissionadaptor Return the
**             Ensembl Quality Check Submission Adaptor
** @nam4rule GetGvalleleadaptor Return the
**             Ensembl Genetic Variation Allele Adaptor
** @nam4rule GetGvgenotypeadaptor Return the
**             Ensembl Genetic Variation Genotype Adaptor
** @nam4rule GetGvindividualadaptor Return the
**             Ensembl Genetic Variation Individual Adaptor
** @nam4rule GetGvpopulationadaptor Return the
**             Ensembl Genetic Variation Population Adaptor
** @nam4rule GetGvsampleadaptor Return the
**             Ensembl Genetic Variation Sample Adaptor
** @nam4rule GetGvvariationadaptor Return the
**             Ensembl Genetic Variation Variation Adaptor
**
** @argrule * dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @valrule Analysisadaptor [EnsPAnalysisadaptor]
**            Ensembl Analysis Adaptor
** @valrule Assemblyexceptionadaptor [EnsPAssemblyexceptionadaptor]
**            Ensembl Assembly Exception Adaptor
** @valrule Assemblyexceptionfeatureadaptor [EnsPAssemblyexceptionfeatureadaptor]
**            Ensembl Assembly Exception Feature Adaptor
** @valrule Assemblymapperadaptor [EnsPAssemblymapperadaptor]
**            Ensembl Assembly Mapper Adaptor
** @valrule Attributeadaptor [EnsPAttributeadaptor]
**            Ensembl Attribute Adaptor
** @valrule Coordsystemadaptor [EnsPCoordsystemadaptor]
**            Ensembl Coordinate System Adaptor
** @valrule Databaseentryadaptor [EnsPDatabaseentryadaptor]
**            Ensembl Database Entry Adaptor
** @valrule Densityfeatureadaptor [EnsPDensityfeatureadaptor]
**            Ensembl Density Feature Adaptor
** @valrule Densitytypeadaptor [EnsPDensitytypeadaptor]
**            Ensembl Density Type Adaptor
** @valrule Ditagadaptor [EnsPDitagadaptor]
**            Ensembl Ditag Adaptor
** @valrule Ditagfeatureadaptor [EnsPDitagfeatureadaptor]
**            Ensembl Ditag Feature Adaptor
** @valrule DNAAlignFeatureadaptor [EnsPDNAAlignFeatureadaptor]
**            Ensembl DNA Align Feature Adaptor
** @valrule Exonadaptor [EnsPExonadaptor]
**            Ensembl Exon Adaptor
** @valrule Externaldatabaseadaptor [EnsPExternaldatabaseadaptor]
**            Ensembl External Database Adaptor
** @valrule Geneadaptor [EnsPGeneadaptor]
**            Ensembl Gene Adaptor
** @valrule Karyotypebandadaptor [EnsPKaryotypebandadaptor]
**            Ensembl Karyotype Band Adaptor
** @valrule Markeradaptor [EnsPMarkeradaptor]
**            Ensembl Marker Adaptor
** @valrule Markerfeatureadaptor [EnsPMarkerfeatureadaptor]
**            Ensembl Marker Feature Adaptor
** @valrule Metacoordinateadaptor [EnsPMetacoordinateadaptor]
**            Ensembl Meta-Coordinate Adaptor
** @valrule Metainformationadaptor [EnsPMetainformationadaptor]
**            Ensembl Meta-Information Adaptor
** @valrule Miscellaneousfeatureadaptor [EnsPMiscellaneousfeatureadaptor]
**            Ensembl Miscellaneous Feature Adaptor
** @valrule Miscellaneoussetadaptor [EnsPMiscellaneoussetadaptor]
**            Ensembl Miscellaneous Set Adaptor
** @valrule Predictionexonadaptor [EnsPPredictionexonadaptor]
**            Ensembl Prediction Exon Adaptor
** @valrule Predictiontranscriptadaptor [EnsPPredictiontranscriptadaptor]
**            Ensembl Prediction Transcript Adaptor
** @valrule Proteinalignfeatureadaptor [EnsPProteinalignfeatureadaptor]
**            Ensembl Protein Align Feature Adaptor
** @valrule Proteinfeatureadaptor [EnsPProteinfeatureadaptor]
**            Ensembl Protein Feature Adaptor
** @valrule Repeatconsensusadaptor [EnsPRepeatconsensusadaptor]
**            Ensembl Repeat Consensus Adaptor
** @valrule Repeatfeatureadaptor [EnsPRepeatfeatureadaptor]
**            Ensembl Repeat Feature Adaptor
** @valrule Seqregionadaptor [EnsPSeqregionadaptor]
**            Ensembl Sequence Region Adaptor
** @valrule Simplefeatureadaptor [EnsPSimplefeatureadaptor]
**            Ensembl Simple Feature Adaptor
** @valrule Sequenceadaptor [EnsPSequenceadaptor]
**            Ensembl Sequence Adaptor
** @valrule Sliceadaptor [EnsPSliceadaptor]
**            Ensembl Slice Adaptor
** @valrule Transcriptadaptor [EnsPTranscriptadaptor]
**            Ensembl Transcript Adaptor
** @valrule Translationadaptor [EnsPTranslationadaptor]
**            Ensembl Translation Adaptor
** @valrule Qcdatabaseadaptor [EnsPQcdatabaseadaptor]
**            Ensembl Quality Check Database Adaptor
** @valrule Qcsequenceadaptor [EnsPQcsequenceadaptor]
**            Ensembl Quality Check Sequence Adaptor
** @valrule Qcalignmentadaptor [EnsPQcalignmentadaptor]
**            Ensembl Quality Check Alignment Adaptor
** @valrule Qcdasfeatureadaptor [EnsPQcdasfeatureadaptor]
**            Ensembl Quality Check DAS Feature Adaptor
** @valrule Qcvariationadaptor [EnsPQcvariationadaptor]
**            Ensembl Quality Check Variation Adaptor
** @valrule Qcsubmissionadaptor [EnsPQcsubmissionadaptor]
**            Ensembl Quality Check Submission Adaptor
** @valrule Gvalleleadaptor [EnsPGvalleleadaptor]
**            Ensembl Genetic Variation Allele Adaptor
** @valrule Gvgenotypeadaptor [EnsPGvgenotypeadaptor]
**            Ensembl Genetic Variation Genotype Adaptor
** @valrule Gvindividualadaptor [EnsPGvindividualadaptor]
**            Ensembl Genetic Variation Individual Adaptor
** @valrule Gvpopulationadaptor [EnsPGvpopulationadaptor]
**            Ensembl Genetic Variation Population Adaptor
** @valrule Gvsampleadaptor [EnsPGvsampleadaptor]
**            Ensembl Genetic Variation Sample Adaptor
** @valrule Gvvariationadaptor [EnsPGvvariationadaptor]
**            Ensembl Genetic Variation Variation Adaptor
**
** @fcategory use
******************************************************************************/




/* @func ensRegistryGetDatabaseadaptor ****************************************
**
** Get an Ensembl Database Adaptor from the Ensembl Registry.
**
** @cc Bio::EnsEMBL::Registry::get_DBAdaptor
** @param [r] group [EnsEDatabaseadaptorGroup] Ensembl Database Adaptor group
** @param [r] alias [const AjPStr] Scientific species name or alias name
**
** @return [EnsPDatabaseadaptor] Ensembl Database Adaptor or NULL
** @@
******************************************************************************/

EnsPDatabaseadaptor ensRegistryGetDatabaseadaptor(
    EnsEDatabaseadaptorGroup group,
    const AjPStr alias)
{
    AjBool debug = AJFALSE;

    AjPStr species = NULL;

    RegistryPEntry entry             = NULL;
    RegistryPCoreStyle rcs           = NULL;
    RegistryPGeneticVariation rgv    = NULL;
    RegistryPFunctionalGenomics rfg  = NULL;
    RegistryPComparativeGenomics rcg = NULL;
    RegistryPOntology ro             = NULL;

    debug = ajDebugTest("ensRegistryGetDatabaseadaptor");

    if(debug)
        ajDebug("ensRegistryGetDatabaseadaptor\n"
                "  group %d\n"
                "  alias '%S'\n",
                group,
                alias);

    if(!group)
        return NULL;

    if(!alias)
        return NULL;

    species = ensRegistryGetSpecies(alias);

    if(debug)
        ajDebug("ensRegistryGetDatabaseadaptor alias '%S' -> species '%S'\n",
                alias, species);

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries, (const void *) species);

    if(!entry)
    {
        ajDebug("ensRegistryGetDatabaseadaptor could not get a "
                "Registry Entry for group '%s' and species '%S'.\n",
                ensDatabaseadaptorGroupToChar(group), species);

        return NULL;
    }

    switch(group)
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle) entry->Registry[group];

            if(rcs)
                return rcs->Databaseadaptor;
            else
            {
                ajDebug("ensRegistryGetDatabaseadaptor could not get an "
                        "Ensembl Database Adaptor for group '%s' and "
                        "species '%S'.\n",
                        ensDatabaseadaptorGroupToChar(group), species);

                return NULL;
            }

            break;

        case ensEDatabaseadaptorGroupGeneticVariation:

            rgv = (RegistryPGeneticVariation) entry->Registry[group];

            if(rgv)
                return rgv->Databaseadaptor;
            else
            {
                ajDebug("ensRegistryGetDatabaseadaptor could not get an "
                        "Ensembl Database Adaptor for group '%s' and "
                        "species '%S'.\n",
                        ensDatabaseadaptorGroupToChar(group), species);

                return NULL;
            }

            break;

        case ensEDatabaseadaptorGroupFunctionalGenomics:

            rfg = (RegistryPFunctionalGenomics) entry->Registry[group];

            if(rfg)
                return rfg->Databaseadaptor;
            else
            {
                ajDebug("ensRegistryGetDatabaseadaptor could not get an "
                        "Ensembl Database Adaptor for group '%s' and "
                        "species '%S'.\n",
                        ensDatabaseadaptorGroupToChar(group), species);

                return NULL;
            }

            break;

        case ensEDatabaseadaptorGroupComparativeGenomics:

            rcg = (RegistryPComparativeGenomics) entry->Registry[group];

            if(rcg)
                return rcg->Databaseadaptor;
            else
            {
                ajDebug("ensRegistryGetDatabaseadaptor could not get an "
                        "Ensembl Database Adaptor for group '%s' and "
                        "species '%S'.\n",
                        ensDatabaseadaptorGroupToChar(group), species);

                return NULL;
            }

            break;

        case ensEDatabaseadaptorGroupOntology:

            ro = (RegistryPOntology) entry->Registry[group];

            if(ro)
                return ro->Databaseadaptor;
            else
            {
                ajDebug("ensRegistryGetDatabaseadaptor could not get an "
                        "Ensembl Database Adaptor for group '%s' and "
                        "species '%S'.\n",
                        ensDatabaseadaptorGroupToChar(group), species);

                return NULL;
            }

            break;

        case ensEDatabaseadaptorGroupQualityCheck:

        case ensEDatabaseadaptorGroupPipeline:

        case ensEDatabaseadaptorGroupHive:

        case ensEDatabaseadaptorGroupCoreExpressionEST:

        case ensEDatabaseadaptorGroupCoreExpressionGNF:

        case ensEDatabaseadaptorGroupAncestral:

        case ensEDatabaseadaptorGroupWebsite:

            break;

        default:

            ajWarn("ensRegistryGetDatabaseadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n", group);
    }

    return NULL;
}




/* @func ensRegistryGetReferenceadaptor ***************************************
**
** Get an Ensembl Reference Adaptor from the Ensembl Registry.
**
** An Ensembl Reference Adaptor is an Ensembl Database Adaptor that is
** connected to an Ensembl core-style database that serves as the reference
** for a particular species.
** Reference databases have the rather large sequence and assembly tables
** populated, backing the following adaptors. In the set of release databases,
** the core database serve as the reference. Other core-style databases
** (e.g. cdna, otherfeatures, vega) are linked to the reference database by
** setting the Reference Adaptor and thus avoiding redundant storage of large
** data sets.
**
** @see ensRegistryGetAssemblyexceptionadaptor
** @see ensRegistryGetAssemblyexceptionfeatureadaptor
** @see ensRegistryGetAssemblymapperadaptor
** @see ensRegistryGetCoordsystemadaptor
** @see ensRegistryGetKaryotypebandadaptor
** @see ensRegistryGetRepeatconsensusadaptor
** @see ensRegistryGetRepeatfeatureadaptor
** @see ensRegistryGetSequenceadaptor
**
** @cc Bio::EnsEMBL::Registry::get_DNAAdaptor
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPDatabaseadaptor] Ensembl Database Adaptor or NULL
** @@
******************************************************************************/

EnsPDatabaseadaptor ensRegistryGetReferenceadaptor(EnsPDatabaseadaptor dba)
{
    AjBool debug = AJFALSE;

    EnsPDatabaseadaptor ra = NULL;

    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    debug = ajDebugTest("ensRegistryGetReferenceadaptor");

    if(debug)
    {
        ajDebug("ensRegistryGetReferenceadaptor\n"
                "  dba %p\n",
                dba);

        ensDatabaseadaptorTrace(dba, 1);
    }

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            ra = (rcs->Referenceadaptor) ?
                rcs->Referenceadaptor :
                rcs->Databaseadaptor;

            break;

        default:

            ajWarn("ensRegistryGetReferenceadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    if(debug)
    {
        ajDebug("ensRegistryGetReferenceadaptor dba %p ra %p\n", dba, ra);

        ensDatabaseadaptorTrace(ra, 1);
    }

    return ra;
}




/* @func ensRegistryGetAnalysisadaptor ****************************************
**
** Get an Ensembl Analysis Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPAnalysisadaptor] Ensembl Analysis Adaptor or NULL
** @@
******************************************************************************/

EnsPAnalysisadaptor ensRegistryGetAnalysisadaptor(EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->Analysisadaptor)
                rcs->Analysisadaptor =
                    ensAnalysisadaptorNew(dba);

            return rcs->Analysisadaptor;

            break;

        default:

            ajWarn("ensRegistryGetAnalysisadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetAssemblyexceptionadaptor *******************************
**
** Get an Ensembl Assembly Exception Adaptor from the Ensembl Registry.
**
** If an Ensembl Reference Adaptor has been set for this database, the adaptor
** returned will be based on the Ensembl Reference Adaptor rather than the
** Ensembl Database Adaptor that was passed in.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPAssemblyexceptionadaptor] Ensembl Assembly Exception Adaptor
**                                        or NULL
** @@
******************************************************************************/

EnsPAssemblyexceptionadaptor ensRegistryGetAssemblyexceptionadaptor(
    EnsPDatabaseadaptor dba)
{
    EnsPDatabaseadaptor ra = NULL;

    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    ra = ensRegistryGetReferenceadaptor(dba);

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(ra));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(ra))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(ra)];

            if(!rcs->Assemblyexceptionadaptor)
                rcs->Assemblyexceptionadaptor =
                    ensAssemblyexceptionadaptorNew(ra);

            return rcs->Assemblyexceptionadaptor;

            break;

        default:

            ajWarn("ensRegistryGetAssemblyexceptionadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(ra));
    }

    return NULL;
}




/* @func ensRegistryGetAssemblyexceptionfeatureadaptor ************************
**
** Get an Ensembl Assembly Exception Feature Adaptor from the Ensembl Registry.
**
** If an Ensembl Reference Adaptor has been set for this database, the adaptor
** returned will be based on the Ensembl Reference Adaptor rather than the
** Ensembl Database Adaptor that was passed in.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPAssemblyexceptionfeatureadaptor] Ensembl Assembly Exception
**                                               Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPAssemblyexceptionfeatureadaptor ensRegistryGetAssemblyexceptionfeatureadaptor(
    EnsPDatabaseadaptor dba)
{
    EnsPDatabaseadaptor ra = NULL;

    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    ra = ensRegistryGetReferenceadaptor(dba);

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(ra));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(ra))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(ra)];

            if(!rcs->Assemblyexceptionfeatureadaptor)
                rcs->Assemblyexceptionfeatureadaptor =
                    ensAssemblyexceptionfeatureadaptorNew(ra);

            return rcs->Assemblyexceptionfeatureadaptor;

            break;

        default:

            ajWarn("ensRegistryGetAssemblyexceptionfeatureadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(ra));
    }

    return NULL;
}




/* @func ensRegistryGetAssemblymapperadaptor **********************************
**
** Get an Ensembl Assembly Mapper Adaptor from the Ensembl Registry.
**
** If an Ensembl Reference Adaptor has been set for this database, the adaptor
** returned will be based on the Ensembl Reference Adaptor rather than the
** Ensembl Database Adaptor that was passed in.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPAssemblymapperadaptor] Ensembl Assembly Mapper Adaptor or NULL
** @@
******************************************************************************/

EnsPAssemblymapperadaptor ensRegistryGetAssemblymapperadaptor(
    EnsPDatabaseadaptor dba)
{
    EnsPDatabaseadaptor ra = NULL;

    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    ra = ensRegistryGetReferenceadaptor(dba);

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(ra));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(ra))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(ra)];

            if(!rcs->Assemblymapperadaptor)
                rcs->Assemblymapperadaptor =
                    ensAssemblymapperadaptorNew(ra);

            return rcs->Assemblymapperadaptor;

            break;

        default:

            ajWarn("ensRegistryGetAssemblymapperadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(ra));
    }

    return NULL;
}




/* @func ensRegistryGetAttributeadaptor ***************************************
**
** Get an Ensembl Attribute Adaptor from the Ensembl Registry.
** The Ensembl Attribute Adaptor is an alias for an
** Ensembl Database Adaptor connected to an Ensembl Core-style database.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPAttributeadaptor] Ensembl Attribute Adaptor or NULL
** @@
******************************************************************************/

EnsPAttributeadaptor ensRegistryGetAttributeadaptor(
    EnsPDatabaseadaptor dba)
{
    if(!dba)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            return dba;

            break;

        default:

            ajWarn("ensRegistryGetAttributeadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetCoordsystemadaptor *************************************
**
** Get an Ensembl Coordinate System Adaptor from the Ensembl Registry.
**
** If an Ensembl Reference Adaptor has been set for this database, the adaptor
** returned will be based on the Ensembl Reference Adaptor rather than the
** Ensembl Database Adaptor that was passed in.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPCoordsystemadaptor] Ensembl Coordinate System Adaptor or NULL
** @@
******************************************************************************/

EnsPCoordsystemadaptor ensRegistryGetCoordsystemadaptor(
    EnsPDatabaseadaptor dba)
{
    EnsPDatabaseadaptor ra = NULL;

    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    ra = ensRegistryGetReferenceadaptor(dba);

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(ra));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(ra))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(ra)];

            if(!rcs->Coordsystemadaptor)
                rcs->Coordsystemadaptor =
                    ensCoordsystemadaptorNew(ra);

            return rcs->Coordsystemadaptor;

            break;

        default:

            ajWarn("ensRegistryGetCoordsystemadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(ra));
    }

    return NULL;
}




/* @func ensRegistryGetDatabaseentryadaptor ***********************************
**
** Get an Ensembl Database Entry Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPDatabaseentryadaptor] Ensembl Database Entry Adaptor or NULL
** @@
******************************************************************************/

EnsPDatabaseentryadaptor ensRegistryGetDatabaseentryadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->Databaseentryadaptor)
                rcs->Databaseentryadaptor =
                    ensDatabaseentryadaptorNew(dba);

            return rcs->Databaseentryadaptor;

            break;

        default:

            ajWarn("ensRegistryGetDatabaseentryadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetDensityfeatureadaptor **********************************
**
** Get an Ensembl Density Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPDensityfeatureadaptor] Ensembl Density Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPDensityfeatureadaptor ensRegistryGetDensityfeatureadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->Densityfeatureadaptor)
                rcs->Densityfeatureadaptor =
                    ensDensityfeatureadaptorNew(dba);

            return rcs->Densityfeatureadaptor;

            break;

        default:

            ajWarn("ensRegistryGetDensityfeatureadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetDensitytypeadaptor *************************************
**
** Get an Ensembl Density Type Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPDensitytypeadaptor] Ensembl Density Type Adaptor or NULL
** @@
******************************************************************************/

EnsPDensitytypeadaptor ensRegistryGetDensitytypeadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->Densitytypeadaptor)
                rcs->Densitytypeadaptor =
                    ensDensitytypeadaptorNew(dba);

            return rcs->Densitytypeadaptor;

            break;

        default:

            ajWarn("ensRegistryGetDensitytypeadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetDitagadaptor *******************************************
**
** Get an Ensembl Ditag Adaptor from the Ensembl Registry.
** The Ensembl Ditag Adaptor is an alias for an
** Ensembl Database Adaptor connected to an Ensembl Core-style database.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPDitagadaptor] Ensembl Ditag Adaptor or NULL
** @@
******************************************************************************/

EnsPDitagadaptor ensRegistryGetDitagadaptor(
    EnsPDatabaseadaptor dba)
{
    if(!dba)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            return dba;

            break;

        default:

            ajWarn("ensRegistryGetDitagadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetDitagfeatureadaptor ************************************
**
** Get an Ensembl Ditag Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPDitagfeatureadaptor] Ensembl Ditag Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPDitagfeatureadaptor ensRegistryGetDitagfeatureadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->Ditagfeatureadaptor)
                rcs->Ditagfeatureadaptor =
                    ensDitagfeatureadaptorNew(dba);

            return rcs->Ditagfeatureadaptor;

            break;

        default:

            ajWarn("ensRegistryGetDitagfeatureadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetDNAAlignFeatureadaptor *********************************
**
** Get an Ensembl DNA Align Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPDNAAlignFeatureadaptor] Ensembl DNA Align Feature Adaptor
**                                      or NULL
** @@
******************************************************************************/

EnsPDNAAlignFeatureadaptor ensRegistryGetDNAAlignFeatureadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->DNAAlignFeatureadaptor)
                rcs->DNAAlignFeatureadaptor =
                    ensDNAAlignFeatureadaptorNew(dba);

            return rcs->DNAAlignFeatureadaptor;

            break;

        default:

            ajWarn("ensRegistryGetDNAAlignFeatureadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetExonadaptor ********************************************
**
** Get an Ensembl Exon Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPExonadaptor] Ensembl Exon Adaptor or NULL
** @@
******************************************************************************/

EnsPExonadaptor ensRegistryGetExonadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->Exonadaptor)
                rcs->Exonadaptor =
                    ensExonadaptorNew(dba);

            return rcs->Exonadaptor;

            break;

        default:

            ajWarn("ensRegistryGetExonadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetExternaldatabaseadaptor ********************************
**
** Get an Ensembl External Database Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPExternaldatabaseadaptor] Ensembl External Database Adaptor or
**                                       NULL
** @@
******************************************************************************/

EnsPExternaldatabaseadaptor ensRegistryGetExternaldatabaseadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->Externaldatabaseadaptor)
                rcs->Externaldatabaseadaptor =
                    ensExternaldatabaseadaptorNew(dba);

            return rcs->Externaldatabaseadaptor;

            break;

        default:

            ajWarn("ensRegistryGetExternaldatabaseadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetGeneadaptor ********************************************
**
** Get an Ensembl Gene Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPGeneadaptor] Ensembl Gene Adaptor or NULL
** @@
******************************************************************************/

EnsPGeneadaptor ensRegistryGetGeneadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->Geneadaptor)
                rcs->Geneadaptor =
                    ensGeneadaptorNew(dba);

            return rcs->Geneadaptor;

            break;

        default:

            ajWarn("ensRegistryGetGeneadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetKaryotypebandadaptor ***********************************
**
** Get an Ensembl Karyotype Band Adaptor from the Ensembl Registry.
**
** If an Ensembl Reference Adaptor has been set for this database, the adaptor
** returned will be based on the Ensembl Reference Adaptor rather than the
** Ensembl Database Adaptor that was passed in.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPKaryotypebandadaptor] Ensembl Karyotype Band Adaptor or NULL
** @@
******************************************************************************/

EnsPKaryotypebandadaptor ensRegistryGetKaryotypebandadaptor(
    EnsPDatabaseadaptor dba)
{
    EnsPDatabaseadaptor ra = NULL;

    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    ra = ensRegistryGetReferenceadaptor(dba);

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(ra));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(ra))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(ra)];

            if(!rcs->Karyotypebandadaptor)
                rcs->Karyotypebandadaptor =
                    ensKaryotypebandadaptorNew(ra);

            return rcs->Karyotypebandadaptor;

            break;

        default:

            ajWarn("ensRegistryGetKaryotypebandadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(ra));
    }

    return NULL;
}




/* @func ensRegistryGetMarkeradaptor ******************************************
**
** Get an Ensembl Marker Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPMarkeradaptor] Ensembl Marker Adaptor or NULL
** @@
******************************************************************************/

EnsPMarkeradaptor ensRegistryGetMarkeradaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->Markeradaptor)
                rcs->Markeradaptor =
                    ensMarkeradaptorNew(dba);

            return rcs->Markeradaptor;

            break;

        default:

            ajWarn("ensRegistryGetMarkeradaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetMarkerfeatureadaptor ***********************************
**
** Get an Ensembl Marker Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPMarkerfeatureadaptor] Ensembl Marker Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPMarkerfeatureadaptor ensRegistryGetMarkerfeatureadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->Markerfeatureadaptor)
                rcs->Markerfeatureadaptor =
                    ensMarkerfeatureadaptorNew(dba);

            return rcs->Markerfeatureadaptor;

            break;

        default:

            ajWarn("ensRegistryGetMarkerfeatureadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetMetainformationadaptor *********************************
**
** Get an Ensembl Meta-Information Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPMetainformationadaptor] Ensembl Meta-Information Adaptor
**                                      or NULL
** @@
******************************************************************************/

EnsPMetainformationadaptor ensRegistryGetMetainformationadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->Metainformationadaptor)
                rcs->Metainformationadaptor =
                    ensMetainformationadaptorNew(dba);

            return rcs->Metainformationadaptor;

            break;

        default:

            ajWarn("ensRegistryGetMetainformationadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetMetacoordinateadaptor **********************************
**
** Get an Ensembl Meta-Coordinate Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPMetacoordinateadaptor] Ensembl Meta-Coordinate Adaptor or NULL
** @@
******************************************************************************/

EnsPMetacoordinateadaptor ensRegistryGetMetacoordinateadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->Metacoordinateadaptor)
                rcs->Metacoordinateadaptor =
                    ensMetacoordinateadaptorNew(dba);

            return rcs->Metacoordinateadaptor;

            break;

        default:

            ajWarn("ensRegistryGetMetacoordinateadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetMiscellaneousfeatureadaptor ****************************
**
** Get an Ensembl Miscellaneous Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPMiscellaneousfeatureadaptor] Ensembl Miscellaneous
**                                           Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPMiscellaneousfeatureadaptor ensRegistryGetMiscellaneousfeatureadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->Miscellaneousfeatureadaptor)
                rcs->Miscellaneousfeatureadaptor =
                    ensMiscellaneousfeatureadaptorNew(dba);

            return rcs->Miscellaneousfeatureadaptor;

            break;

        default:

            ajWarn("ensRegistryGetMiscellaneousfeatureadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetMiscellaneoussetadaptor ********************************
**
** Get an Ensembl Miscellaneous Set Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPMiscellaneoussetadaptor] Ensembl Miscellaneous Set Adaptor
**                                       or NULL
** @@
******************************************************************************/

EnsPMiscellaneoussetadaptor ensRegistryGetMiscellaneoussetadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->Miscellaneoussetadaptor)
                rcs->Miscellaneoussetadaptor =
                    ensMiscellaneoussetadaptorNew(dba);

            return rcs->Miscellaneoussetadaptor;

            break;

        default:

            ajWarn("ensRegistryGetMiscellaneoussetadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetPredictionexonadaptor **********************************
**
** Get an Ensembl Prediction Exon Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPPredictionexonadaptor] Ensembl Prediction Exon Adaptor or NULL
** @@
******************************************************************************/

EnsPPredictionexonadaptor ensRegistryGetPredictionexonadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->Predictionexonadaptor)
                rcs->Predictionexonadaptor =
                    ensPredictionexonadaptorNew(dba);

            return rcs->Predictionexonadaptor;

            break;

        default:

            ajWarn("ensRegistryGetPredictionexonadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetPredictiontranscriptadaptor ****************************
**
** Get an Ensembl Prediction Transcript Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPPredictiontranscriptadaptor] Ensembl Prediction
**                                           Transcript Adaptor or NULL
** @@
******************************************************************************/

EnsPPredictiontranscriptadaptor ensRegistryGetPredictiontranscriptadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->Predictiontranscriptadaptor)
                rcs->Predictiontranscriptadaptor =
                    ensPredictiontranscriptadaptorNew(dba);

            return rcs->Predictiontranscriptadaptor;

            break;

        default:

            ajWarn("ensRegistryGetPredictiontranscriptadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetProteinalignfeatureadaptor *****************************
**
** Get an Ensembl Protein Align Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPProteinalignfeatureadaptor] Ensembl Protein Align Feature
**                                          Adaptor or NULL
** @@
******************************************************************************/

EnsPProteinalignfeatureadaptor ensRegistryGetProteinalignfeatureadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->Proteinalignfeatureadaptor)
                rcs->Proteinalignfeatureadaptor =
                    ensProteinalignfeatureadaptorNew(dba);

            return rcs->Proteinalignfeatureadaptor;

            break;

        default:

            ajWarn("ensRegistryGetProteinalignfeatureadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetProteinfeatureadaptor **********************************
**
** Get an Ensembl Protein Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPProteinfeatureadaptor] Ensembl Protein Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPProteinfeatureadaptor ensRegistryGetProteinfeatureadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->Proteinfeatureadaptor)
                rcs->Proteinfeatureadaptor =
                    ensProteinfeatureadaptorNew(dba);

            return rcs->Proteinfeatureadaptor;

            break;

        default:

            ajWarn("ensRegistryGetProteinfeatureadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetRepeatconsensusadaptor *********************************
**
** Get an Ensembl Repeat Consensus Adaptor from the Ensembl Registry.
** The Ensembl Repeat Consensus Adaptor is an alias for an
** Ensembl Database Adaptor connected to an Ensembl Core-style database.
**
** If an Ensembl Reference Adaptor has been set for this database, the adaptor
** returned will be based on the Ensembl Reference Adaptor rather than the
** Ensembl Database Adaptor that was passed in.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPRepeatconsensusadaptor] Ensembl Repeat Consensus Adaptor
**                                      or NULL
** @@
******************************************************************************/

EnsPRepeatconsensusadaptor ensRegistryGetRepeatconsensusadaptor(
    EnsPDatabaseadaptor dba)
{
    EnsPDatabaseadaptor ra = NULL;

    if(!dba)
        return NULL;

    ra = ensRegistryGetReferenceadaptor(dba);

    switch(ensDatabaseadaptorGetGroup(ra))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            return ra;

            break;

        default:

            ajWarn("ensRegistryGetRepeatconsensusadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(ra));
    }

    return NULL;
}




/* @func ensRegistryGetRepeatfeatureadaptor ***********************************
**
** Get an Ensembl Repeat Feature Adaptor from the Ensembl Registry.
**
** If an Ensembl Reference Adaptor has been set for this database, the adaptor
** returned will be based on the Ensembl Reference Adaptor rather than the
** Ensembl Database Adaptor that was passed in.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPRepeatfeatureadaptor] Ensembl Repeat Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPRepeatfeatureadaptor ensRegistryGetRepeatfeatureadaptor(
    EnsPDatabaseadaptor dba)
{
    EnsPDatabaseadaptor ra = NULL;

    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    ra = ensRegistryGetReferenceadaptor(dba);

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(ra));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(ra))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(ra)];

            if(!rcs->Repeatfeatureadaptor)
                rcs->Repeatfeatureadaptor =
                    ensRepeatfeatureadaptorNew(ra);

            return rcs->Repeatfeatureadaptor;

            break;

        default:

            ajWarn("ensRegistryGetRepeatfeatureadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(ra));
    }

    return NULL;
}




/* @func ensRegistryGetSeqregionadaptor ***************************************
**
** Get an Ensembl Sequence Region Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPSeqregionadaptor] Ensembl Sequence Region Adaptor or NULL
** @@
******************************************************************************/

EnsPSeqregionadaptor ensRegistryGetSeqregionadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->Seqregionadaptor)
                rcs->Seqregionadaptor =
                    ensSeqregionadaptorNew(dba);

            return rcs->Seqregionadaptor;

            break;

        default:

            ajWarn("ensRegistryGetSeqregionadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetSimplefeatureadaptor ***********************************
**
** Get an Ensembl Simple Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPSimplefeatureadaptor] Ensembl Simple Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPSimplefeatureadaptor ensRegistryGetSimplefeatureadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->Simplefeatureadaptor)
                rcs->Simplefeatureadaptor =
                    ensSimplefeatureadaptorNew(dba);

            return rcs->Simplefeatureadaptor;

            break;

        default:

            ajWarn("ensRegistryGetSimplefeatureadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetSequenceadaptor ****************************************
**
** Get an Ensembl Sequence Adaptor from the Ensembl Registry.
**
** If an Ensembl Reference Adaptor has been set for this database, the adaptor
** returned will be based on the Ensembl Reference Adaptor rather than the
** Ensembl Database Adaptor that was passed in.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPSequenceadaptor] Ensembl Sequence Adaptor or NULL
** @@
******************************************************************************/

EnsPSequenceadaptor ensRegistryGetSequenceadaptor(
    EnsPDatabaseadaptor dba)
{
    EnsPDatabaseadaptor ra = NULL;

    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    ra = ensRegistryGetReferenceadaptor(dba);

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(ra));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(ra))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(ra)];

            if(!rcs->Sequenceadaptor)
                rcs->Sequenceadaptor =
                    ensSequenceadaptorNew(ra);

            return rcs->Sequenceadaptor;

            break;

        default:

            ajWarn("ensRegistryGetSequenceadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(ra));
    }

    return NULL;
}




/* @func ensRegistryGetSliceadaptor *******************************************
**
** Get an Ensembl Slice Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPSliceadaptor] Ensembl Slice Adaptor or NULL
** @@
******************************************************************************/

EnsPSliceadaptor ensRegistryGetSliceadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->Sliceadaptor)
                rcs->Sliceadaptor =
                    ensSliceadaptorNew(dba);

            return rcs->Sliceadaptor;

            break;

        default:

            ajWarn("ensRegistryGetSliceadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetTranscriptadaptor **************************************
**
** Get an Ensembl Transcript Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPTranscriptadaptor] Ensembl Transcript Adaptor or NULL
** @@
******************************************************************************/

EnsPTranscriptadaptor ensRegistryGetTranscriptadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->Transcriptadaptor)
                rcs->Transcriptadaptor =
                    ensTranscriptadaptorNew(dba);

            return rcs->Transcriptadaptor;

            break;

        default:

            ajWarn("ensRegistryGetTranscriptadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetTranslationadaptor *************************************
**
** Get an Ensembl Translation Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPTranslationadaptor] Ensembl Translation Adaptor or NULL
** @@
******************************************************************************/

EnsPTranslationadaptor ensRegistryGetTranslationadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle rcs = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupCore:

        case ensEDatabaseadaptorGroupVega:

        case ensEDatabaseadaptorGroupOtherFeatures:

        case ensEDatabaseadaptorGroupCopyDNA:

            rcs = (RegistryPCoreStyle)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rcs->Translationadaptor)
                rcs->Translationadaptor =
                    ensTranslationadaptorNew(dba);

            return rcs->Translationadaptor;

            break;

        default:

            ajWarn("ensRegistryGetTranslationadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetQcdatabaseadaptor **************************************
**
** Get an Ensembl Quality Check Database Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPQcdatabaseadaptor] Ensembl Quality Check Database Adaptor
**                                 or NULL
** @@
******************************************************************************/

EnsPQcdatabaseadaptor ensRegistryGetQcdatabaseadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry      = NULL;
    RegistryPQualityCheck eqc = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupQualityCheck:

            eqc = (RegistryPQualityCheck)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!eqc->Databaseadaptor)
                eqc->Qcdatabaseadaptor =
                    ensQcdatabaseadaptorNew(dba);

            return eqc->Qcdatabaseadaptor;

            break;

        default:

            ajWarn("ensRegistryGetQcdatabaseadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetQcsequenceadaptor **************************************
**
** Get an Ensembl Quality Check Sequence Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPQcsequenceadaptor] Ensembl Quality Check Sequence Adaptor
**                                 or NULL
** @@
******************************************************************************/

EnsPQcsequenceadaptor ensRegistryGetQcsequenceadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry      = NULL;
    RegistryPQualityCheck eqc = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupQualityCheck:

            eqc = (RegistryPQualityCheck)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!eqc->Sequenceadaptor)
                eqc->Sequenceadaptor =
                    ensQcsequenceadaptorNew(dba);

            return eqc->Sequenceadaptor;

            break;

        default:

            ajWarn("ensRegistryGetQcsequenceadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetQcalignmentadaptor *************************************
**
** Get an Ensembl Quality Check Alignment Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPQcalignmentadaptor] Ensembl Quality Check Alignment Adaptor
**                                  or NULL
** @@
******************************************************************************/

EnsPQcalignmentadaptor ensRegistryGetQcalignmentadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry      = NULL;
    RegistryPQualityCheck eqc = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupQualityCheck:

            eqc = (RegistryPQualityCheck)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!eqc->Alignmentadaptor)
                eqc->Alignmentadaptor =
                    ensQcalignmentadaptorNew(dba);

            return eqc->Alignmentadaptor;

            break;

        default:

            ajWarn("ensRegistryGetQcalignmentadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetQcdasfeatureadaptor ************************************
**
** Get an Ensembl Quality Check DAS Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPQcdasfeatureadaptor] Ensembl Quality Check DAS Feature Adaptor
**                                   or NULL
** @@
******************************************************************************/

EnsPQcdasfeatureadaptor ensRegistryGetQcdasfeatureadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry      = NULL;
    RegistryPQualityCheck eqc = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupQualityCheck:

            eqc = (RegistryPQualityCheck)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!eqc->DASFeatureadaptor)
                eqc->DASFeatureadaptor =
                    ensQcdasfeatureadaptorNew(dba);

            return eqc->DASFeatureadaptor;

            break;

        default:

            ajWarn("ensRegistryGetQcdasfeatureadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetQcvariationadaptor *************************************
**
** Get an Ensembl Quality Check Variation Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPQcvariationadaptor] Ensembl Quality Check Variation Adaptor
**                                  or NULL
** @@
******************************************************************************/

EnsPQcvariationadaptor ensRegistryGetQcvariationadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry      = NULL;
    RegistryPQualityCheck eqc = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupQualityCheck:

            eqc = (RegistryPQualityCheck)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!eqc->Variationadaptor)
                eqc->Variationadaptor =
                    ensQcvariationadaptorNew(dba);

            return eqc->Variationadaptor;

            break;

        default:

            ajWarn("ensRegistryGetQcvariationadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetQcsubmissionadaptor ************************************
**
** Get an Ensembl Quality Check Submission Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPQcsubmissionadaptor] Ensembl Quality Check Submission Adaptor
**                                   or NULL
** @@
******************************************************************************/

EnsPQcsubmissionadaptor ensRegistryGetQcsubmissionadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry      = NULL;
    RegistryPQualityCheck eqc = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupQualityCheck:

            eqc = (RegistryPQualityCheck)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!eqc->Submissionadaptor)
                eqc->Submissionadaptor =
                    ensQcsubmissionadaptorNew(dba);

            return eqc->Submissionadaptor;

            break;

        default:

            ajWarn("ensRegistryGetQcsubmissionadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetGvalleleadaptor ****************************************
**
** Get an Ensembl Genetic Variation Allele Adaptor from the
** Ensembl Registry.
** The Ensembl Genetic Variation Allele Adaptor is an alias for an
** Ensembl Database Adaptor connected to an Ensembl Variation database.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPGvalleleadaptor] Ensembl Genetic Variation
**                               Allele Adaptor or NULL
** @@
******************************************************************************/

EnsPGvalleleadaptor ensRegistryGetGvalleleadaptor(
    EnsPDatabaseadaptor dba)
{
    if(!dba)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupGeneticVariation:

            return dba;

            break;

        default:

            ajWarn("ensRegistryGetGvalleleadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetGvgenotypeAdaptor **************************************
**
** Get an Ensembl Genetic Variation Genotype Adaptor from the
** Ensembl Registry.
** The Ensembl Genetic Variation Genotype Adaptor is an alias for an
** Ensembl Database Adaptor connected to an Ensembl Variation database.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPGvgenotypeadaptor] Ensembl Genetic Variation
**                                 Genotype Adaptor or NULL
** @@
******************************************************************************/

EnsPGvgenotypeadaptor ensRegistryGetGvgenotypeadaptor(
    EnsPDatabaseadaptor dba)
{
    if(!dba)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupGeneticVariation:

            return dba;

            break;

        default:

            ajWarn("ensRegistryGetGvgenotypeAdaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetGvindividualadaptor ************************************
**
** Get an Ensembl Genetic Variation Individual Adaptor from the
** Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPGvindividualadaptor] Ensembl Genetic Variation
**                                   Individual Adaptor or NULL
** @@
******************************************************************************/

EnsPGvindividualadaptor ensRegistryGetGvindividualadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry          = NULL;
    RegistryPGeneticVariation rgv = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupGeneticVariation:

            rgv = (RegistryPGeneticVariation)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rgv->Individualadaptor)
                rgv->Individualadaptor =
                    ensGvindividualadaptorNew(dba);

            return rgv->Individualadaptor;

            break;

        default:

            ajWarn("ensRegistryGetGvindividualadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetGvpopulationadaptor ************************************
**
** Get an Ensembl Genetic Variation Population Adaptor from the
** Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPGvpopulationadaptor] Ensembl Genetic Variation
**                                   Population Adaptor or NULL
** @@
******************************************************************************/

EnsPGvpopulationadaptor ensRegistryGetGvpopulationadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry          = NULL;
    RegistryPGeneticVariation rgv = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupGeneticVariation:

            rgv = (RegistryPGeneticVariation)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rgv->Populationadaptor)
                rgv->Populationadaptor =
                    ensGvpopulationadaptorNew(dba);

            return rgv->Populationadaptor;

            break;

        default:

            ajWarn("ensRegistryGetGvpopulationadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetGvsampleadaptor ****************************************
**
** Get an Ensembl Genetic Variation Sample Adaptor from the
** Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPGvsampleadaptor] Ensembl Genetic Variation
**                               Sample Adaptor or NULL
** @@
******************************************************************************/

EnsPGvsampleadaptor ensRegistryGetGvsampleadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry          = NULL;
    RegistryPGeneticVariation rgv = NULL;

    if(!dba)
        return NULL;

    entry = (RegistryPEntry)
        ajTableFetch(registryEntries,
                     (const void *) ensDatabaseadaptorGetSpecies(dba));

    if(!entry)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupGeneticVariation:

            rgv = (RegistryPGeneticVariation)
                entry->Registry[ensDatabaseadaptorGetGroup(dba)];

            if(!rgv->Sampleadaptor)
                rgv->Sampleadaptor =
                    ensGvsampleadaptorNew(dba);

            return rgv->Sampleadaptor;

            break;

        default:

            ajWarn("ensRegistryGetGvsampleadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @func ensRegistryGetGvvariationadaptor *************************************
**
** Get an Ensembl Genetic Variation Variation Adaptor from the
** Ensembl Registry.
** The Ensembl Genetic Variation Variation Adaptor is an alias for an
** Ensembl Database Adaptor connected to an Ensembl Variation database.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                  Variation Adaptor or NULL
** @@
******************************************************************************/

EnsPGvvariationadaptor ensRegistryGetGvvariationadaptor(
    EnsPDatabaseadaptor dba)
{
    if(!dba)
        return NULL;

    switch(ensDatabaseadaptorGetGroup(dba))
    {
        case ensEDatabaseadaptorGroupGeneticVariation:

            return dba;

            break;

        default:

            ajWarn("ensRegistryGetGvvariationadaptor got an "
                   "Ensembl Database Adaptor "
                   "with an unexpected group %d.\n",
                   ensDatabaseadaptorGetGroup(dba));
    }

    return NULL;
}




/* @funcstatic registryLoadCollection *****************************************
**
** Load species form a collection database, register species names in the
** alias table and instatiate Ensembl Database Adaptor objects.
**
** @param [u] dbc [EnsPDatabaseconnection] Ensembl Database Connection
** @param [r] dbname [AjPStr] Database name
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool registryLoadCollection(EnsPDatabaseconnection dbc,
                                     AjPStr dbname)
{
    char *txtdbname = NULL;

    ajuint identifier = 0;

    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;

    AjPStr alias     = NULL;
    AjPStr species   = NULL;
    AjPStr statement = NULL;

    if(!dbc)
        return ajFalse;

    ensDatabaseconnectionEscapeC(dbc, &txtdbname, dbname);

    statement = ajFmtStr("SELECT "
                         "%s.meta.species_id, "
                         "%s.meta.meta_value "
                         "FROM "
                         "%s.meta "
                         "WHERE "
                         "%s.meta.meta_key = 'species.db_name'",
                         txtdbname,
                         txtdbname,
                         txtdbname,
                         txtdbname);

    ajCharDel(&txtdbname);

    sqls = ensDatabaseconnectionSqlstatementNew(dbc, statement);

    sqli = ajSqlrowiterNew(sqls);

    while(!ajSqlrowiterDone(sqli))
    {
        identifier = 0;
        alias = ajStrNew();
        species = ajStrNew();

        sqlr = ajSqlrowiterGet(sqli);

        ajSqlcolumnToUint(sqlr, &identifier);
        ajSqlcolumnToStr(sqlr, &alias);

        ajStrAssignS(&species, alias);

        ajStrExchangeCC(&species, "_", " ");

        ensRegistryAddAlias(species, alias);

        ensRegistryAddDatabaseadaptor(dbc,
                                      dbname,
                                      species,
                                      ensEDatabaseadaptorGroupCore,
                                      ajTrue,
                                      identifier);

        ajStrDel(&alias);

        ajStrDel(&species);
    }

    ajSqlrowiterDel(&sqli);

    ajSqlstatementDel(&sqls);

    ajStrDel(&statement);

    return ajTrue;
}




/* @func ensRegistryLoadFromServer ********************************************
**
** Automatically register databases on an SQL server with the Ensembl Registry.
**
** @param [u] dbc [EnsPDatabaseconnection] Ensembl Database Connection
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRegistryLoadFromServer(EnsPDatabaseconnection dbc)
{
    AjBool debug = AJFALSE;

    AjPRegexp multire      = NULL;
    AjPRegexp speciesre    = NULL;
    AjPRegexp collectionre = NULL;

    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;

    AjPStr statement = NULL;
    AjPStr dbname    = NULL;
    AjPStr group     = NULL;
    AjPStr prefix    = NULL;
    AjPStr dbversion = NULL;
    AjPStr swversion = NULL;
    AjPStr multi     = NULL;

    EnsEDatabaseadaptorGroup egroup = ensEDatabaseadaptorGroupNULL;

    debug = ajDebugTest("ensRegistryLoadFromServer");

    if(debug)
    {
        ajDebug("ensRegistryLoadFromServer\n"
                "  dbc %p\n",
                dbc);

        ensDatabaseconnectionTrace(dbc, 1);
    }

    if(!dbc)
        return ajFalse;

    /*
    ** FIXME: Experimentally, load aliases for species names from a data file.
    ** This should probably go into an ensInit function, although it may not
    ** be neccessary to load this every time?
    */

    ensRegistryLoadAliases();

    multi = ajStrNewC("DEFAULT");

    multire = ajRegCompC("^ensembl_([a-z]+)_([0-9]+)");

    collectionre =
        ajRegCompC("^(\\w+)_(collection_core_(?:\\d+_)?(\\d+)_(\\w+))");

    speciesre =
        ajRegCompC("^([a-z]+_[a-z]+)_([a-z]+)_([0-9]+)_([0-9]+[a-z]*)");

    statement = ajStrNewC("SHOW DATABASES");

    sqls = ensDatabaseconnectionSqlstatementNew(dbc, statement);

    if(!sqls)
        ajFatal("ensRegistryLoadFromServer SQL statement failed.\n"
                "Please check the SQL server address '%S', "
                "your network connection or that any firewalls "
                "permitt outgong TCP/IP connections on port '%S'.\n",
                ensDatabaseconnectionGetHostName(dbc),
                ensDatabaseconnectionGetHostPort(dbc));

    sqli = ajSqlrowiterNew(sqls);

    while(!ajSqlrowiterDone(sqli))
    {
        dbname = ajStrNew();

        sqlr = ajSqlrowiterGet(sqli);

        ajSqlcolumnToStr(sqlr, &dbname);

        /* Ensembl Core Collection databases have to be matched first. */

        if(ajRegExec(collectionre, dbname))
        {
            prefix    = ajStrNew();
            swversion = ajStrNew();
            dbversion = ajStrNew();

            ajRegSubI(collectionre, 1, &prefix);
            ajRegSubI(collectionre, 2, &dbversion);
            ajRegSubI(collectionre, 3, &swversion);

            registryLoadCollection(dbc, dbname);

            /*
            ** TODO: Finish this and test the regular expression above whether
            ** it really works!
            */
        }

        /* Species-specific databases */

        if(ajRegExec(speciesre, dbname))
        {
            prefix    = ajStrNew();
            group     = ajStrNew();
            swversion = ajStrNew();
            dbversion = ajStrNew();

            ajRegSubI(speciesre, 1, &prefix);
            ajRegSubI(speciesre, 2, &group);
            ajRegSubI(speciesre, 3, &swversion);
            ajRegSubI(speciesre, 4, &dbversion);

            if(ajStrMatchCaseC(swversion, registrySoftwareVersion))
            {
                egroup = ensDatabaseadaptorGroupFromStr(group);

                if(egroup)
                    ensRegistryAddDatabaseadaptor(dbc,
                                                  dbname,
                                                  prefix,
                                                  egroup,
                                                  ajFalse,
                                                  0);
                else
                    ajWarn("ensRegistryLoadFromServer got unexpected group "
                           "string '%S' for database name '%S'.\n",
                           group, dbname);
            }

            ajStrDel(&prefix);
            ajStrDel(&group);
            ajStrDel(&swversion);
            ajStrDel(&dbversion);
        }

        /* Multi-species databases */

        if(ajRegExec(multire, dbname))
        {
            group     = ajStrNew();
            swversion = ajStrNew();

            ajRegSubI(multire, 1, &group);
            ajRegSubI(multire, 2, &swversion);

            if(ajStrMatchCaseC(swversion, registrySoftwareVersion))
            {
                egroup = ensDatabaseadaptorGroupFromStr(group);

                if(egroup)
                    ensRegistryAddDatabaseadaptor(dbc,
                                                  dbname,
                                                  multi,
                                                  egroup,
                                                  ajFalse,
                                                  0);

                else
                    ajWarn("ensRegistryLoadFromServer got unexpected group "
                           "string '%S' for database name '%S'.\n",
                           group, dbname);
            }

            ajStrDel(&group);
            ajStrDel(&swversion);
        }

        ajStrDel(&dbname);
    }

    ajSqlrowiterDel(&sqli);

    ajSqlstatementDel(&sqls);

    ajRegFree(&collectionre);
    ajRegFree(&multire);
    ajRegFree(&speciesre);

    ajStrDel(&statement);
    ajStrDel(&multi);

    /*
    ** TODO: At the end of this another loop is required to set core adaptors
    ** as reference adaptors for all core-style databases except the core.
    ** See
    ** Bio::EnsEMBL::Utils::ConfigRegistry::load_core
    ** Bio::EnsEMBL::Utils::ConfigRegistry::load_adaptors
    ** Bio::EnsEMBL::Utils::ConfigRegistry::load_and_attach_dnadb_to_core
    */

    if(debug)
    {
        ajDebug("ensRegistryLoadFromServer\n");

        ensRegistryTraceEntries(1);
    }

    return ajTrue;
}




/* @func ensRegistryLoadIdentifiers *******************************************
**
** Read regular expressions for Ensembl Gene, Transcript, Translation and Exon
** stable identifiers, as well as Ensembl Database species and group
** information from an "EnsemblIdentifiers.dat" data file and add them to the
** Ensembl Registry.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRegistryLoadIdentifiers(void)
{
    AjBool debug = AJFALSE;

    AjPFile infile = NULL;

    AjPStr line       = NULL;
    AjPStr expression = NULL;
    AjPStr alias      = NULL;
    AjPStr group      = NULL;
    AjPStr space      = NULL;
    AjPStr species    = NULL;

    AjPStrTok token = NULL;

    EnsEDatabaseadaptorGroup egroup = ensEDatabaseadaptorGroupNULL;

    RegistryPIdentifier ri = NULL;

    debug = ajDebugTest("ensRegistryLoadIdentifiers");

    if(debug)
        ajDebug("ensRegistryLoadIdentifiers\n");

    infile = ajDatafileNewInNameC("EnsemblIdentifiers.dat");

    if(!infile)
    {
        ajWarn("ensRegistryLoadIdentifiers could no load "
               "EnsemblIdentifiers.dat file.");

        return ajFalse;
    }

    line = ajStrNew();

    expression = ajStrNew();

    alias = ajStrNew();
    group = ajStrNew();
    space = ajStrNew();

    while(ajReadlineTrim(infile, &line))
    {
        if(ajStrCutComments(&line))
        {
            if(debug)
                ajDebug("ensRegistryLoadIdentifiers original line '%S'\n",
                        line);

            ajStrTokenAssignC(&token, line, "\"");

            if(!ajStrTokenNextFind(&token, &expression))
                ajWarn("ensRegistryLoadIdentifiers could not parse "
                       "regular expression from line '%S'.\n", line);

            if(!ajStrTokenNextFind(&token, &space))
                ajWarn("ensRegistryLoadIdentifiers could not parse "
                       "begin of species from line '%S'.\n", line);

            if(!ajStrTokenNextFind(&token, &alias))
                ajWarn("ensRegistryLoadIdentifiers could not parse "
                       "species from line '%S'.\n", line);

            if(!ajStrTokenNextFind(&token, &space))
                ajWarn("ensRegistryLoadIdentifiers could not parse "
                       "begin of group from line '%S'.\n", line);

            if(!ajStrTokenNextFind(&token, &group))
                ajWarn("ensRegistryLoadIdentifiers could not parse "
                       "group from line '%S'.\n", line);

            ajStrTokenDel(&token);

            if(debug)
                ajDebug("ensRegistryLoadIdentifiers "
                        "regular expression '%S' "
                        "alias '%S' "
                        "group '%S'\n",
                        expression,
                        alias,
                        group);

            species = ensRegistryGetSpecies(alias);

            if(!species)
            {
                ajWarn("ensRegistryLoadIdentifiers could not resolve '%S' "
                       "to valid species name.\n", alias);

                continue;
            }

            egroup = ensDatabaseadaptorGroupFromStr(group);

            if(!egroup)
            {
                ajWarn("ensRegistryLoadIdentifiers could not get group for "
                       "string '%S'.\n", group);

                continue;
            }

            ri = registryIdentifierNew(expression, species , egroup);

            ajListPushAppend(registryIdentifiers, (void *) ri);
        }
    }

    ajStrDel(&expression);
    ajStrDel(&alias);
    ajStrDel(&group);
    ajStrDel(&space);
    ajStrDel(&line);

    ajFileClose(&infile);

    return ajTrue;
}




/* @func ensRegistryGetSpeciesGroup *******************************************
**
** Evaluate Ensembl Database Adaptor species and group elements on the basis
** of an Ensembl stable identifier.
**
** @param [r] identifier [const AjPStr] Ensembl stable identifier
** @param [u] Pspecies [AjPStr*] Ensembl Database Adaptor species
** @param [w] Pgroup [EnsEDatabaseadaptorGroup*] Ensembl Database Adaptor group
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRegistryGetSpeciesGroup(const AjPStr identifier,
                                  AjPStr *Pspecies,
                                  EnsEDatabaseadaptorGroup *Pgroup)
{
    AjIList iter = NULL;

    AjPRegexp re = NULL;

    RegistryPIdentifier ri = NULL;

    if(!identifier)
        return ajFalse;

    if(!Pspecies)
        return ajFalse;

    if(!Pgroup)
        return ajFalse;

    ajStrAssignClear(Pspecies);

    *Pgroup = ensEDatabaseadaptorGroupNULL;

    iter = ajListIterNew(registryIdentifiers);

    while(!ajListIterDone(iter))
    {
        ri = (RegistryPIdentifier) ajListIterGet(iter);

        re = ajRegComp(ri->RegularExpression);

        if(ajRegExec(re, identifier))
        {
            ajStrAssignS(Pspecies, ri->SpeciesName);

            *Pgroup = ri->Group;

            ajRegFree(&re);

            break;
        }

        ajRegFree(&re);
    }

    ajListIterDel(&iter);

    return ajTrue;
}
