#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajphylo_h
#define ajphylo_h

/* @data AjPPhyloDist *********************************************************
**
** Ajax phylogeny distance matrix
**
** Input can be square (all values) or lower-triangular (diagonal and below)
** or upper-triangular (diagonal and above). We can count values for the
** first 2 species to identify the format.
**
** S-format allows degree of replication for each distance (integer)
** we can check for this (twice as many numbers) otherwise we set the
** replicates to 1.
** 
** @alias AjSPhyloDist
** @alias AjOPhyloDist
**
** @attr Size [ajint] Size - number of rows and number of columns
** @attr HasReplicates [AjBool] Has (some) replicates data in file
** @attr HasMissing [AjBool] Has missing data in file
** @attr Names [AjPStr*] Row names, NULL at end
** @attr Data [float*] Distance matrix Size*Size with diagnoal 0.0
** @attr Replicates [ajint*] Replicate count default=1 missing=0
** @@
******************************************************************************/

typedef struct AjSPhyloDist
{
    ajint Size;
    AjBool HasReplicates;
    AjBool HasMissing;
    AjPStr* Names;
    float* Data;
    ajint* Replicates;
} AjOPhyloDist;

#define AjPPhyloDist AjOPhyloDist*

/* @data AjPPhyloFreq *********************************************************
**
** Ajax phylogeny frequencies.
**
** For continuous data there are always 2 alleles
** For gene frequency data there can be more than 2 alleles
**
** @alias AjSPhyloFreq
** @alias AjOPhyloFreq
**
** @attr Size [ajint] Number of rows 
** @attr Loci [ajint] Number of loci per name
** @attr Len [ajint] Number of values per name
**                    may be more than 1 per locus
** @attr ContChar [AjBool] Continuous character data if true
** @attr Within [AjBool] Individual data within species if true
** @attr Names [AjPStr*] Row names array (size is Size)
**
** @cc row grouping - multiple individual values for one 'species'
**     ContChar data only, otherwise NULL
**
** @attr Species [ajint*] Species number 1, 2, 3 for each value
**                    array size is Len
** @attr Individuals [ajint*] Allele countNumber of individuals
**                    1 or more per species
**                    array size is Loci
**
** @cc column grouping - multiple frequence values for alleles of a locus
**
** @attr Locus [ajint*] Locus number 1, 2, 3 for each value
**                     array size is Len
** @attr Allele [ajint*] Allele count 2 or more per locus
**                     array size is Loci
** @attr Data [float*] Frequency for each allele for each Name
** @@
******************************************************************************/

typedef struct AjSPhyloFreq
{
    ajint Size;
    ajint Loci;
    ajint Len;
    AjBool ContChar;
    AjBool Within;
    AjPStr* Names;

    ajint* Species;
    ajint* Individuals;

    ajint* Locus;
    ajint* Allele;
    float* Data;
} AjOPhyloFreq;

#define AjPPhyloFreq AjOPhyloFreq*

/* @data AjPPhyloProp *********************************************************
**
** Ajax phylogeny properties: weights, ancestral states, factors.
**
** Basically, all of these are one value per position
**
** Weights are converted to integers 0-9, A=10 Z=35 by phylip
** There are programs that can use multiple weights
** We can handle this by making all of these multiple,
**  and using ACD to limit them to 1 for nonm-weight data.
**
** Ancestral states are character data
**
** Factors are multi-state character data where the factor character changes
** when moving to a new character. Without this, all factors are assumed to
** be different. The default would be to make each character distinct by
** alternating 12121212 or to use 12345678901234567890.
**
** We can, in fact, convert any input string into this format for factors
** but probably we can leave them unchanged.
**
** @alias AjSPhyloProperty
** @alias AjOPhyloProperty
**
** @attr Len [ajint] string length
** @attr Size [ajint] number of strings
** @attr IsWeight [AjBool] is phylip weight values if true
** @attr IsFactor [AjBool] is phylip factor values if true
** @attr Str [AjPStr*] The original string(s)
** @@
******************************************************************************/

typedef struct AjSPhyloProp
{
    ajint Len;
    ajint Size;
    AjBool IsWeight;
    AjBool IsFactor;
    AjPStr* Str;
} AjOPhyloProp;

#define AjPPhyloProp AjOPhyloProp*

/* @data AjPPhyloState ********************************************************
**
** Ajax discrete state data.
**
** Basically, all of these are one value per position
**
** States have a limited character set, usually defined through ACD
**
** @alias AjSPhyloState
** @alias AjOPhyloState
**
** @attr Len [ajint] string length
** @attr Size [ajint] number of strings
** @attr Count [ajint] number of enzymes for restriction data
** @attr Characters [AjPStr] The allowed state characters
** @attr Names [AjPStr*] The names
** @attr Str [AjPStr*] The original string(s)
** @@
******************************************************************************/

typedef struct AjSPhyloState
{
    ajint Len;
    ajint Size;
    ajint Count;
    AjPStr Characters;
    AjPStr* Names;
    AjPStr* Str;
} AjOPhyloState;

#define AjPPhyloState AjOPhyloState*

/* @data AjPPhyloTree *********************************************************
**
** Ajax phylogeny trees
**
** For programs that read multiple tree inputs we use an array,
** and let ACD limit the others to 1 tree.
**
** @alias AjSPhyloTree
** @alias AjOPhyloTree
**
** @attr Multifurcated [AjBool] Multifurcating (..(a,b,c)..)
** @attr BaseTrifurcated [AjBool] 3-way base (a,b,c)
** @attr BaseBifurcated [AjBool] Rooted 2-way base (a,b)
** @attr BaseQuartet [AjBool] Unrooted quartet ((a,b),(c,d));
** @attr Tree [AjPStr] Newick tree string
** @@
******************************************************************************/

typedef struct AjSPhyloTree
{
    AjBool Multifurcated;
    AjBool BaseTrifurcated;
    AjBool BaseBifurcated;
    AjBool BaseQuartet;
    AjPStr Tree;
} AjOPhyloTree;

#define AjPPhyloTree AjOPhyloTree*


void           ajPhyloDistDel (AjPPhyloDist* pthis);
AjPPhyloDist   ajPhyloDistNew (void);
AjPPhyloDist   ajPhyloDistRead (AjPStr filename, ajint size, AjBool missing);
void           ajPhyloDistTrace (AjPPhyloDist thys);

void           ajPhyloFreqDel (AjPPhyloFreq* pthis);
AjPPhyloFreq   ajPhyloFreqNew (void);
AjPPhyloFreq   ajPhyloFreqRead (AjPStr filename, AjBool contchar,
				AjBool genedata, AjBool indiv);
void           ajPhyloFreqTrace (AjPPhyloFreq thys);

void           ajPhyloPropDel (AjPPhyloProp* pthis);
ajint          ajPhyloPropGetSize (AjPPhyloProp thys);
AjPPhyloProp   ajPhyloPropNew (void);
AjPPhyloProp   ajPhyloPropRead (AjPStr filename, AjPStr propchars,
			       ajint len, ajint size);

void           ajPhyloStateDel (AjPPhyloState* pthis);
AjPPhyloState  ajPhyloStateNew (void);
AjPPhyloState* ajPhyloStateRead (AjPStr filename, AjPStr statechars);

void           ajPhyloTreeDel (AjPPhyloTree* pthis);
AjPPhyloTree   ajPhyloTreeNew (void);
AjPPhyloTree*  ajPhyloTreeRead (AjPStr filename, ajint size);

#endif

#ifdef __cplusplus
}
#endif
