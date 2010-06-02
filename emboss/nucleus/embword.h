#ifdef __cplusplus
extern "C"
{
#endif

#ifndef embWord_h
#define embWord_h




/* @data EmbPWordMatch ********************************************************
**
** NUCLEUS data structure for word matches
**
** @attr seq1start [ajuint] match start point in original sequence
** @attr seq2start [ajuint] match start point in comparison sequence
** @attr sequence [const AjPSeq] need in case we build multiple matches here
**                         so we know which one the match belongs to
** @attr length [ajint] length of match
** @attr Padding [char[4]] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EmbSWordMatch {
  ajuint seq1start;
  ajuint seq2start;
  const AjPSeq sequence;
  ajint length;
  char Padding[4];
} EmbOWordMatch;
#define EmbPWordMatch EmbOWordMatch*




/* @data EmbPWord *************************************************************
**
** NUCLEUS data structure for words
**
** @attr fword [const char*] Original word
** @attr seqlocs [AjPTable] Table of word start positions in multiple sequences
** @attr count [ajint] Total number of locations in all sequences
** @attr Padding [char[4]] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EmbSWord {
  const char *fword;
  AjPTable seqlocs;
  ajint count;
  char Padding[4];
} EmbOWord;
#define EmbPWord EmbOWord*




/* @data EmbPWordSeqLocs ******************************************************
**
** NUCLEUS data structure for word locations in a given sequence
**
** @attr seq [const AjPSeq] Sequence for word start positions
** @attr locs [AjPList] List of word start positions in the sequence
** @@
******************************************************************************/

typedef struct EmbSWordSeqLocs {
  const AjPSeq seq;
  AjPList locs;
} EmbOWordSeqLocs;
#define EmbPWordSeqLocs EmbOWordSeqLocs*



/* @datastatic EmbPWordRK *****************************************************
**
** Data structure that extends EmbPWord objects for efficient access
** by Rabin-Karp search. It is constructed using embWordInitRabinKarpSearch
** method for a given sequence-set.
**
** Possible improvements could be achieved by scanning all other words
** during preprocessing to find out a minimum length that can be skipped
** safely when a word is matched.
**
** @attr word [const EmbPWord] Original word object
** @attr seqindxs [ajuint*] Positions in the seqset
**                          for each sequence the word has been seen
** @attr nnseqlocs [ajuint*] Number of word start positions for each sequence
** @attr locs [ajuint**] List of word start positions for each sequence
** @attr hash [ajulong] Hash value for the word
** @attr nseqs [ajuint] Number of sequences word has been seen
** @attr nAllMatches [ajuint] Total number of all matches in all sequences
** @attr nSeqMatches [ajuint] Number of sequences with at least one match
** @attr lenAllMatches [ajulong] Total score/length of all the matches
**                               in all sequences
** @attr Padding [char[8]] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EmbSWordRK {
    const EmbPWord word;
    ajuint* seqindxs;
    ajuint* nnseqlocs;
    ajuint** locs;
    ajulong hash;
    ajuint nseqs;
    ajuint nAllMatches;
    ajuint nSeqMatches;
    ajulong lenAllMatches;
    char Padding[8];
} EmbOWordRK;

#define EmbPWordRK EmbOWordRK*




/*
** Prototype definitions
*/

AjPList embWordBuildMatchTable (const AjPTable seq1MatchTable,
				const AjPSeq seq2, ajint orderit);
void    embWordClear (void);
void    embWordExit(void);
void    embWordFreeTable(AjPTable *table);
AjBool  embWordGetTable (AjPTable *table, const AjPSeq seq);
void    embWordLength (ajint wordlen);
AjBool  embWordMatchIter (AjIList iter, ajint* start1, ajint* start2,
			  ajint* len, const AjPSeq* seq);
EmbPWordMatch embWordMatchListAppend(AjPList hitlist, const AjPSeq seq,
        const ajuint seq1start, ajuint seq2start, ajint length);
void    embWordMatchListDelete (AjPList* plist);
void    embWordMatchListPrint (AjPFile file, const AjPList list);
void    embWordPrintTable  (const AjPTable table);
void    embWordPrintTableF (const AjPTable table, AjPFile outf);
void    embWordPrintTableFI (const AjPTable table, ajint mincount,
			     AjPFile outf);
void    embWordMatchListConvToFeat(const AjPList list,
				   AjPFeattable *tab1, AjPFeattable *tab2,
				   const AjPSeq seq1, const AjPSeq seq2);

void    embWordMatchMin(AjPList matchlist);
void    embWordUnused(void);

EmbPWordMatch embWordMatchFirstMax(AjPList matches);

ajuint embWordRabinKarpSearch(const AjPStr sseq,
                              const AjPSeqset seqset,
                              const EmbPWordRK* patterns,
                              ajuint plen, ajuint nwords,
                              AjPList* l, ajuint* lastlocation,
                              AjBool checkmode);

ajuint embWordRabinKarpInit(const AjPTable table,
	                    EmbPWordRK**, ajuint wordlen,
	                    const AjPSeqset seqset);


/*
** End of prototype definitions
*/

#endif

#ifdef __cplusplus
}
#endif


