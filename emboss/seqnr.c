/* @source seqnr application
**
** Reads a scop families file and a scop ambiguities file and writes (i) a 
** non-redundant scop families file and (ii) a scop validation file.
**
** @author: Copyright (C) Ranjeeva Ranasinghe (rranasin@hgmp.mrc.ac.uk)
** @author: Copyright (C) Jon Ison (jison@hgmp.mrc.ac.uk)
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
**
*******************************************************************************
**  Application name
**  seqnr
**  
**  
**  
**  Summary
**  Reads a scop families file and a scop ambiguities file and writes (i) a 
**  non-redundant scop families file and (ii) a scop validation file.
**  
**  
**  
**  Input and Output
**  seqnr reads a scop families file, a scop ambiguities file and a directory 
**  of scop family alignment files.  Each alignment file should contain a 
**  structure-based sequence alignment of domains of known structure for a 
**  particular scop family.  The directory should contain an alignment for each
**  family in the scop families file.  
**  seqnr writes (i) a scop validation file, which contains all the hits from 
**  the scop families and ambiguities files, plus hits corresponding to 
**  sequences in the alignment.  Depending on the mode it is run in it may also
**  generate (ii) a scop families file in which non-redundant domains are 
**  removed from each family.  A scop classification must also be provided and
**  is the source of domain information for sequences from the alignment files.
**  seqnr ensures that sequences from the alignments are (i) excluded from the
**  scop families file, (ii) included in the validation file (if they do not 
**  already correspond to a prexisting hit) and (iii) are considered for 
**  purposes of calculating redundancy.  The path and extension of the 
**  alignment files are specified by the user.  A log file is also written.
**
**  
** 
**  Sister applications
**  A 'scop families file' contains sequence relatives (hits) for each of a 
**  number of different scop families found from psiblast searches of a 
**  sequence database.  The file contains the collated search results for the
**  indvidual scop families; only those hits of unambiguous family assignment
**  are included.  Hits of ambiguous family assignment are assigned as 
**  relatives to a scop superfamily or fold instead and are collated into a 
**  'scop ambiguities file'.  The scop families and ambiguities files are 
**  generated by seqsort and are in embl-like format.
**  A 'scop classification file' contains classification and other data for 
**  domains from the scop database.  The file is in embl-like format and is 
**  generated by scopparse.
**  A 'scop family alignment file' contains a sequence alignment of domains 
**  belonging to the same scop family.  The file is in embl-like format and is
**  annotated with records describing the SCOP classification of the family.  A
**  scop family alignment file generatd by scopalign will contain a structure-
**  based sequence alignment of domains of known structure only.  Such 
**  alignments can be extended with sequence relatives (of unknown structure)
**  to the family in question by using seqalign.
**  The scop families file is used by seqalign  
**  The scop validation file is used by modelscan, sigscan and sigplot.
**  
**  
**  
**  Notes
**  
**  
**  
**  Known bugs & warnings
**  Care must be taken when validating signatures from structure-based sequence
**  alignments, as some families in the validation file will not contain the
**  same number of SEEDs as sequences in the scop alignment file.  This is 
**  because accession numbers may not be given in the swissprot:pdb equivalence
**  table for all domains in the scop alignment file.
** 
**   
**  
**  Description
**  This program is part of a suite of EMBOSS applications that directly or 
**  indirectly make use of the protein structure databases pdb and scop.  
**  This program is part of an experimental analysis pipeline described in an
**  accompanying document.  We provide the software in the hope that it will
**  be useful.  The applications were designed for specific research purposes
**  and may not be useful or reliable in contexts other than the described 
**  pipeline.  The development of the suite was coordinated by Jon Ison to
**  whom enquiries and bug reports should be sent (email jison@hgmp.mrc.ac.uk).
**  
**  Reads a scop families file and a scop ambiguities file and writes (i) a 
**  non-redundant scop families file and (ii) a scop validation file.
** 
**  
**   
**  Algorithm
**  Redundancy is calculated for each family in the scop families file in turn.
**  Sequences in the scop alignment file corresponding to the family are 
**  considered when calculating redundancy in a process of 4 steps as follows.
**  (1) A list of Scop objects for domains in the alignment file is compiled by
**  opening the appropriate scop alignment file, extracting the scop domain 
**  codes, querying the scop classification file and retrieving the 
**  corresponding scop domains.  These domains are then pushed onto a list. 
**  (2) This list is then merged with a list of domains for the family taken 
**  from the scop families input file.  (3) The merged list is then sorted and
**  one of any pair of sequences that are identical or overlapping is removed.
**  Such overlaps will only arise with sequences from the alignment file, other
**  overlaps having been processed by seqsort (used to generate the scop 
**  families and ambiguities files that are input to seqnr). (4) Redundant 
**  domains are then identified.  
**  
**  seqnr always writes a scop validation file, which contains all the hits 
**  from the scop families and ambiguities files, plus hits corresponding to
**  sequences in the alignment file.  To avoid introducing duplicate hits in
**  the validation file when incorporating hits from the alignment, any hits
**  that have an identical accession number to an alignment sequence and which
**  overlap by at least a user-defined number of residues will NOT be included.
**  Note that the validation file must only contains hits with known accession
**  numbers.  Therefore if an accession number could not be assigned to a hit
**  from the alignment file, then that hit will NOT appear in the validation 
**  file, and a message to the log file will be written. Such cases will arise
**  where the pdb code corresponding to a hit is not listed in the 
**  swissprot:pdb equivalence table, Epdbtosp.dat. The hits in the scop 
**  families will normally have been found by a search against swissprot and 
**  therefore should all have accession numbers.  
**  
**  Hits in the validation file are flagged as NON_REDUNDANT or REDUNDANT as 
**  appropriate.  They are then flagged as 'SEED' or left as 'HIT' depending 
**  on whether the file will be used to validate signatures generated from (i)
**  structure-based sequence alignemnts (mode 1) or (ii) extended alignments 
**  (mode 2).  For mode 1, hits corresponding to sequences from the alignment 
**  files only are marked up as 'SEED'.  For mode 2, hits corresponding to 
**  sequences from the alignment file plus non-redundant sequences from the 
**  scop families input file, i.e. all the sequences in the scop families 
**  output file, will be marked up as 'SEED'.
**  
**  If seqnr is run in mode 2 it will generate a scop families file, which is
**  identical to the scop families input file, except that redundant domains 
**  are removed from each family, and sequences corresponding to domains from
**  the alignment file are also excluded.  (An extended alignment for each 
**  family can be built by using alignwrap, which extends the scop alignment
**  files with sequences from the scop families output file).
**  
**  
**  
**  Usage 
**  An example of interactive use of seqnr is shown below.
**  
**  Unix % seqnr
**  Reads a scop families file and a scop ambiguities file and writes (i) a
**  non-redundant scop families file and (ii) a scop validation file.
**  Name of the scop families file (input): /test_data/scop.fam
**  Name of ambiguities file (input): /test_data/scop.oth
**  Name of scop classification file (embl format input): /test_data/all.scop2
**  Location of scop alignment files (input) [./]: /test_data/     
**  Extension of scop alignment files [.salign]: 
**  number of overlapping residues required to define two hits as 'overlapping' [10]: 20
**  The % sequence identity redundancy threshold [95.0]: 40
**  Mode of operation
**           1 : Validation file for signatures from structure-based sequence alignemnts (no scop families file)
**           2 : Validation file for signatures from extended alignments (plus non-redundant scop families file)
**  Select mode [1]: 1
**  Name of validation file (output) [test.valid.out]: /test_data/scop.all
**  Name seqnr log file (output) [seqnr.log]: /test_data/seqnr.log
**  Processing 55074
**  Warning: Sequence length smaller than overlap limit in ajXyzScophitsOverlapAcc
**  Warning: Sequence length smaller than overlap limit in ajXyzScophitsOverlapAcc
**  Warning: Zero length sequence in SeqsetNR
**  Processing 54894
**  Warning: Sequence length smaller than overlap limit in ajXyzScophitsOverlapAcc
**  Warning: Sequence length smaller than overlap limit in ajXyzScophitsOverlapAcc
**  Warning: Zero length sequence in SeqsetNR
**  Unix % 
**  
**  Scop famililes and validation files called /test_data/scop.fam
**  and /test_data/scop.oth respectively were read and a 
**  validation file (containing the collated contents of scop.fam and scop.oth)
**  called /test_data/scop.all was written.  Scop alignment files
**  with the file extension .salign were read from the directory 
**  /test_data/.  Domain information for sequences from the 
**  alignment files was taken from the scop classification file 
**  /test_data/all.scop2.  When checking for overlaps between 
**  sequences from the scop families input file and the alignment files, a 
**  shared segment of 20 or more identical was required to class two sequences
**  as overlapping.  Two domains within the same family are classified as 
**  redundant if they share at least 40% sequence similarity;  the default 
**  residue substitution matrix EBLOSUM62 and default gap insertion and 
**  extension penalties were used for the alignments.  
**  seqnr was run in mode 1, i.e. a validation file for signatures from 
**  structure-based sequence alignemnt was generated and non non-redundant scop
**  families file was generated.
**  
**  The following command line would achieve the same result.
**  seqnr /test_data/scop.fam /test_data/scop.oth /test_data/scop.all -scopin 
**  /test_data/all.scop2 -alignpath /test_data/ -alignextn .salign -overlap 20
**  -thresh 40 -mode 1 -logf seqnr.log
**  
**  
**  
**  Input file format
**  The format of the scop families and ambiguities files is described in 
**  seqsort.c.
**  The format of the scop classification file is described in scopparse.c
**  The format of the scop alignment file is described in scopalign.c
**  
**  
**  
**  Output file format
**  The format of the scop validation (see below) is exactly the same as the
**  scop families file, described in seqsearch.c.  However, data for multiple
**  families, superfamilies or folds may occur in the file. Data for each 
**  family etc will be delimited by the record '//' on its own line.  Where a
**  list of hits to a superfamily or fold is given, clearly that entry will 
**  lack the FA (family description) or FA and SF (superfamily description)
**  records respectively.
**  
**  Excerpt from scop validation file
**  CL   Alpha and beta proteins (a/b)
**  XX
**  FO   Leucine-rich repeat, LRR (right-handed beta-alpha superhelix)
**  XX
**  NS   2
**  XX
**  NN   [1]
**  XX
**  TY   HIT
**  XX
**  SC   0.00
**  XX
**  AC   O02833
**  XX
**  RA   44 START; 556 END;
**  XX
**  SQ   SEQUENCE   513 AA;  56641 MW;  32B739CA CRC32;
**       TCACSYDDEV NELSVFCSSR NLTRLPDGIP GGTQALWLDS NNLSSIPPAA FRNLSSLAFL
**       NLQGGQLGSL EPQALLGLEN LCHLHLERNQ LRSLAVGTFA YTPALALLGL SNNRLSRLED
**       GLFEGLGNLW DLNLGWNSLA VLPDAAFRGL GGLRELVLAG NRLAYLQPAL FSGLAELREL
**       DLSRNALRAI KANVFAQLPR LQKLYLDRNL IAAVAPGAFL GLKALRWLDL SHNRVAGLLE
**       DTFPGLLGLR VLRLSHNAIA SLRPRTFEDL HFLEELQLGH NRIRQLAERS FEGLGQLEVL
**       TLDHNQLQEV KVGAFLGLTN VAVMNLSGNC LRNLPEQVFR GLGKLHSLHL EGSCLGRIRP
**       HTFAGLSGLR RLFLKDNGLV GIEEQSLWGL AELLELDLTS NQLTHLPHQL FQGLGKLEYL
**       LLSHNRLAEL PADALGPLQR AFWLDVSHNR LEALPGSLLA SLGRLRYLNL RNNSLRTFTP
**       QPPGLERLWL EGNPWDCSCP LKALRDFALQ NPS
**  XX
**  NN   [2]
**  XX
**  TY   HIT
**  XX
**  SC   0.00
**  XX
**  AC   O02678
**  XX
**  RA   48 START; 344 END;
**  XX
**  SQ   SEQUENCE   297 AA;  33312 MW;  C4F5498F CRC32;
**       SGVPDLDALT PTYSAMCPFG CHCHLRVVQC SDLGLKAVPK EISPDTMLLD LQNNDISELR
**       ADDFKGLHHL YALVLVNNKI SKIHEKAFSP LRKLQKLYIS KNHLVEIPPN LPSSLVELRI
**       HDNRIRKVPK GVFSGLRNMN CIEMGGNPLE NSGFEPGAFD GLKLNYLRIS EAKLTGIPKD
**       LPETLNELHL DHNKIQAIEL EDLLRYSKLY RLGLGHNQIR MIENGSLSFL PTLRELHLDN
**       NKLSRVPSGL PDLKLLQVVY LHTNNITKVG VNDFCPVGFG VKRAYYNGIS LFNNPVP
**  XX
**  //
**  CL   All beta proteins
**  XX
**  FO   Viral coat and capsid proteins
**  XX
**  SF   Viral coat and capsid proteins
**  XX
**  NS   1
**  XX
**  NN   [1]
**  XX
**  TY   HIT
**  XX
**  SC   0.00
**  XX
**  AC   P13418
**  XX
**  RA   4 START; 158 END;
**  XX
**  SQ   SEQUENCE   155 AA;  17476 MW;  00048440 CRC32;
**       GYVTNAFTYW RGSIVYTFKF VKTQYHSGRL RISFIPYYYN TTISTGTPDV SRTQKIVVDL
**       RTSTEVSFTV PYIASRPWLY CIRPESSWLS KDNKDGALMY NCVSGIVRTE VLNQLVAAQN
**       VFSEIDVICE VNGGPDLEFA GPTCPSMYPY AGDDT
**  XX
**  //
**  CL   Alpha and beta proteins (a+b)
**  XX
**  FO   Ferredoxin-like
**  XX
**  SF   Adenylyl and guanylyl cyclase catalytic domain
**  XX
**  FA   Adenylyl and guanylyl cyclase catalytic domain
**  XX
**  SI   55074
**  XX
**  NS   3
**  XX
**  NN   [1]
**  XX
**  TY   HIT
**  XX
**  SC   0.00
**  XX
**  AC   Q9WVI4
**  XX
**  RA   514 START; 667 END;
**  XX
**  SQ   SEQUENCE   154 AA;  17175 MW;  CCD26187 CRC32;
**       KFDDVTMLFS DIVGFTAICA QCTPMQVISM LNELYTRFDH QCGFLDIYKV ETIGDAYCVA
**       SGLHRKSLCH AKPIALMALK MMELSEEVLT PDGRPIQMRI GIHSGSVLAG VVGVRMPRYC
**       LFGNNVTLAS KFESGSHPRR INISPTTYQL LKRE
**  XX
**  NN   [2]
**  XX
**  TY   HIT
**  XX
**  SC   0.00
**  XX
**  AC   Q9DGG6
**  XX
**  RA   1032 START; 1200 END;
**  XX
**  SQ   SEQUENCE   169 AA;  19208 MW;  7DD56054 CRC32;
**       YSKNHDSGGV IFASIVNFSE FYEENYEGGK ECYRVLNELI GDFDELLSKP HYSSIEKIKT
**       IGATYMAASG LNTSQCQDSN HPHGHLQTLF EFAKEMMRVV DDFNNNMLWF NFKLRIGFNH
**       GPLTAGVIGT TKLLYDIWGD TVNIASRMDT IGVECRIQVS EETYRILSK
**  XX
**  NN   [3]
**  XX
**  TY   HIT
**  XX
**  SC   0.00
**  XX
**  AC   Q9DGG6
**  XX
**  RA   377 START; 542 END;
**  XX
**  SQ   SEQUENCE   166 AA;  18602 MW;  4B3EA1C2 CRC32;
**       QQIEQVSILF ADIVGFTKMS ANKSAHALVG LLNDLFGRFD RLCEDTKCEK ISTLGDCYYC
**       VAGCPEPRAD HAYCCIEMGL GMIKAIEQFC QEKKEMVNMR VGVHTGTVLC GILGMRRFKF
**       DVWSNDVNLA NLMEQLGVAG KVHISEATAK YLDDRYEMED GKVTER
**  XX
**  //
**  CL   Alpha and beta proteins (a+b)
**  XX
**  FO   Ferredoxin-like
**  XX
**  SF   Aspartate carbamoyltransferase, Regulatory-chain, N-terminal domain
**  XX
**  FA   Aspartate carbamoyltransferase, Regulatory-chain, N-terminal domain
**  XX
**  SI   54894
**  XX
**  NS   2
**  XX
**  NN   [1]
**  XX
**  TY   HIT
**  XX
**  SC   0.00
**  XX
**  AC   Q9YBD5
**  XX
**  RA   11 START; 105 END;
**  XX
**  SQ   SEQUENCE    95 AA;  10687 MW;  B8ACF34B CRC32;
**       VRKIRSGVVI DHIPPGRAFT MLKALGLLPP RGYRWRIAVV INAESSKLGR KDILKIEGYK
**       PRQRDLEVLG IIAPGATFNV IEDYKVVEKV KLKLP
**  XX
**  NN   [2]
**  XX
**  TY   HIT
**  XX
**  SC   0.00
**  XX
**  AC   Q9UX07
**  XX
**  RA   16 START; 108 END;
**  XX
**  SQ   SEQUENCE    93 AA;  10312 MW;  12DDDE7C CRC32;
**       VSKIRNGTVI DHIPAGRALA VLRILGIRGS EGYRVALVMN VESKKIGRKD IVKIEDRVID
**       EKEASLITLI APSATINIIR DYVVTEKRHL EVP
**  XX
**  //
**  
**  
**  
**  Data files
**  seqnr requires a residue substitution matrix.
**  
**  
**  
**  Diagnostic error messages
**  
**  
**  
**  Authors
**  Ranjeeva Ranasinghe (rranasin@hgmp.mrc.ac.uk)
**  Jon Ison (jison@hgmp.mrc.ac.uk)
**  
**  
**  
**  References
**  Please cite the authors.
**  
**  
**  
******************************************************************************/








#include "emboss.h"






/* @prog seqnr **************************************************************
**
** Reads a scop families file and a scop ambiguities file and writes (i) a 
** non-redundant scop families file and (ii) a scop validation file.
**
******************************************************************************/
int main(int argc, char **argv)
{
    /* Variables */
    /* File input and output */
    AjPFile logf         = NULL;   /* log file pointer */
    AjPFile inf          = NULL;   /* file containing the unprocessed hits */
    AjPFile vinf         = NULL;   /* file pointer for the validation file */
    AjPFile outf         = NULL;   /* the outfile for the processed families */
    AjPStr  voutfname    = NULL;   /* the validation output file name */
    AjPFile voutf        = NULL;   /* the pointer for the validation output file */
    AjPFile scop_inf     = NULL;   /* file pointer for Escop.dat */
    AjPFile aln_inf      = NULL;   /* file pointer to alignment file */
    AjPStr alignpath     = NULL;   /* path to the seed alignment files */
    AjPStr alignextn     = NULL;   /* extension for seed alignment files */
    AjPStr alignname     = NULL;   /* name of alignment file */


    /* Parameters */
    float gapopen        =0.0;     /* Gap insertion penalty */
    float gapextend      =0.0;     /* Gap extension penalty */
    float thresh         =0.0;     /* Threshold for non-redundancy */
    AjPMatrixf matrix    =NULL;    /* Substitution matrix */
    

    /* Housekeeping */
    ajint     overlap    = 0;      /* number of overlapping residues required to define two 
				      hits as 'overlapping' */
    AjPStr    exec       = NULL;   /* The UNIX command line to be executed*/
    ajint     i          = 0;      /* loop counter */
    AjIList   iter       = NULL;   /* Reused list iterator */
    AjPInt    keep       = NULL;   /* 1: This sequence was kept after redundancy removal,
                                      0: it was discarded */
    ajint     nsetnr     = 0;      /* No. proteins in the non-redundant set */
    AjPScop    tmp_scop  = NULL;   /* temp. scop object pointer for objects popped from scop_align*/
    AjPScophit tmp_hit   = NULL;   /* temp. pointer to a scophit object from famin_hits */
    AjPSeq     tmp_seq   = NULL;   /* A temp. AjPSeq pointer for constructing famin_seqs */
    AjPStr     *mode     =NULL;    /* Mode of operation from acd*/
    AjPList    tmp_list  =NULL;    /* Temp. list for freeing memory */
    AjBool    isseed1    =ajFalse; /* Used for targetting overlaps with alignemnt sequences */
    AjBool    isseed2    =ajFalse; /* Used for targetting overlaps with alignemnt sequences */
    AjPScophit hit1        = NULL; /* Used for targetting overlaps with alignemnt sequences */
    AjPScophit hit2        = NULL; /* Used for targetting overlaps with alignemnt sequences */

    
    /* Alignment */
    AjPScopalg align     = NULL;   /* Seed alignment corresponding to the current hitlist */
    AjPList  scop_align  = NULL;   /* list of scop objects from the alignment file */

    
    /* SCOP database */
    AjPList   scop_list  = NULL;   /* list of scop objects for entire Escop.dat */
    AjPScop  *scop_arr   = NULL;   /* Array of Scop objects derived from scop_list ordered by 
				      Domain Id*/
    ajint     scop_dim   = 0;      /* Size of scop_arr */
    
    
    /* Family being processed */
    AjPHitlist famin     = NULL;   /* The family from the scop families that will be processed */
    AjPList famin_list   = NULL;   /* A list containing famin only */
    AjPList famin_hits   = NULL;   /* A list of Scophits corresponding to famin.   Scophits 
				      derived from the alignment file are eventually added to 
				      this list, which is then processed. */
    AjPHitlist famout_valid= NULL;  /* A Hitlist corresponding to famin_hits before redundant domains
				      have been removed and hits targetted for removal have been 
				      removed.  It is this Hitlist that is written to the validation
				      file */
    AjPList famin_seqs   = NULL;   /* A list of AjPSeq's correspdonding to famin_hits. Used for
				      calculating redundancy removal. */

    AjPList famout_hits  = NULL;   /* A list of Scophit objects that has been processed */
    AjPHitlist famout    = NULL;   /* The processed family as a hitlist (from famout_hits) */


    AjPScophit align_hit = NULL;   /* Temp. variable for a Scophit derived from the alignment file */



    
    /* Read data from acd */
    embInit("seqnr",argc,argv);
    
    inf       = ajAcdGetInfile("inf");

    vinf      = ajAcdGetInfile("vinf");
    voutfname = ajAcdGetString("voutfname");
    scop_inf  = ajAcdGetInfile("scopin");
    alignpath = ajAcdGetString("alignpath");
    alignextn = ajAcdGetString("alignextn");
    thresh    = ajAcdGetFloat("thresh");
    matrix    = ajAcdGetMatrixf("datafile");
    gapopen   = ajAcdGetFloat("gapopen");
    gapextend = ajAcdGetFloat("gapextend");
    mode      = ajAcdGetList("mode");
    logf      = ajAcdGetOutfile("logf");
    overlap   = ajAcdGetInt("overlap");

    if(ajStrChar(*mode, 0) == '2')
    {
	outf      = ajAcdGetOutfile("outf");
    }
    


    alignname = ajStrNew();
    exec     = ajStrNew();





    /* Copy contents of validation input file to create validation output file, 
       open the output file.*/
    ajFmtPrintS(&exec, "cp %S %S\n", ajFileGetName(vinf), voutfname);
    system(ajStrStr(exec));
    voutf=ajFileNewApp(voutfname);

    /* Create list of scop objects for entire Escop.dat */
    scop_list  = ajListNew();
    while((ajXyzScopReadC(scop_inf,"*",&tmp_scop)))
        ajListPushApp(scop_list,tmp_scop);
    
    /* Order the list of Scop objects by Domain Id and create an array */
    ajListSort(scop_list, ajXyzScopCompId);
    scop_dim = ajListToArray(scop_list,(void ***)&scop_arr);


    /* Error handing if Escop.dat was empty */
    if(!scop_dim)
    {
        ajWarn("Empty list in ajXyzScopalgToScop\n");
	ajFileClose(&logf);
	ajFileClose(&inf);
	if(ajStrChar(*mode, 0) == '2')
	{
	    ajFileClose(&outf);
	}
	
	ajFileClose(&vinf);
	ajFileClose(&voutf);
	ajStrDel(&voutfname);
	ajFileClose(&scop_inf);
	ajStrDel(&alignpath);
	ajStrDel(&alignextn);
	ajMatrixfDel(&matrix);

	ajStrDel(&alignname);
	
	ajExit();
	return 1;
    }
    
    

    
    /* Read Hitlist for the family from the scop families that will be processed, 
       each entry delimited by a "//" is read and processed in turn */
    while(ajXyzHitlistRead(inf,"//",&famin)) 
    {
	ajFmtPrint("Processing %d\n", famin->Sunid_Family);
	ajFmtPrintF(logf, "//\n%d\n", famin->Sunid_Family);

/*JISON        keep        = ajIntNew(); */
        famin_list  = ajListNew();
        famin_hits  = ajListNew();
        famin_seqs  = ajListNew();
        famout_hits = ajListNew();
	tmp_list    = ajListNew();

	

        /* Open structure-based sequence alignment file corresponding to the family.
	   If seed alignment is absent no point in continuing */
	ajStrFromInt(&alignname, famin->Sunid_Family);
	ajStrApp(&alignname,alignextn);
        if(!(aln_inf=ajFileNewDF(alignpath,alignname)))
        {
            ajWarn("The alignment file %S%S could not be found\n",alignpath,alignname);
	    ajFmtPrintF(logf, "Alignment file %S%S not found\n",alignpath,alignname);
	    ajIntDel(&keep);
	    ajListDel(&famin_list);
	    ajListDel(&famin_hits);
	    ajListDel(&famin_seqs);
	    ajListDel(&famout_hits);
	    ajListDel(&tmp_list);
            continue;
        }

        /* Read seed alignment into scopalg structure */
        ajXyzScopalgRead(aln_inf,&align);


        /* Construct list of scop objects from the alignment file */
        scop_align = ajListNew();
        
        if(!ajXyzScopalgToScop(align,scop_arr,scop_dim,&scop_align))
	    ajFatal("ajXyzScopalgToScop");
	
	/* scop_align must be freed.
	   scop_align points to data in scop_arr so do NOT free nodes
	   */


        /* Push famin onto a list (the list contains this single Hitlist only)
	   No need to pop famin_list and free the single node, so long as famin 
	   is freed seperately.
	   Convert this list to a list of Scophit objects.
	   Must pop famin_hits and free the nodes that are allocated by 
	   ajXyzHitlistToScophits  	 */

        ajListPushApp(famin_list,famin);
        ajXyzHitlistToScophits(famin_list,&famin_hits);


        /* Add the scop objects from the alignment file to the main list */
        while(ajListPop(scop_align,(void **)&tmp_scop))
        {
            align_hit = ajXyzScophitNew(); 
            ajXyzScopToScophit(tmp_scop,&align_hit);

	    if(ajStrMatchC(align_hit->Acc, "Not_available"))
		ajFmtPrintF(logf, "No swissprot sequence for domain %S from alignment "
			    "so no hit will be given in validation file\n", 
			    tmp_scop->Entry);
	    
	    

	    /* For structure-based sequence alignemnts, hits corresponding to sequences 
	       from the alignment files only are marked up as 'SEED').
	       For extended alignments, these hits plus non-redundant hits (i.e. in
	       the scop families output file) will be 'SEED'.*/
	    /*
	    if(ajStrChar(*mode, 0) == '1')
	    {
		ajStrAssC(&(align_hit->Typeobj), "SEED");
	    }
	    */
	    ajStrAssC(&(align_hit->Typeobj), "SEED");


            ajXyzScophitTarget(&align_hit);
            ajListPushApp(famin_hits,align_hit);
	    
	    /*The memory here will be freed when famin_hits is popped and the nodes are freed */
        }

        /* Delete original Hitlist and the derived list
           We now just have a list of Scophit's called <famin_hits> */
        ajXyzHitlistDel(&famin);
	famin=NULL;
	ajListDel(&famin_list);
	famin_list=NULL;
	

        /* 
	   Sort the list and target hits that are identical or overlapping
	   (the Target2 element will set ajTrue).
	   Overlap will only arise with alignment sequences, other overlaps    
	   will have been processed by seqsort.

	   Accession number and start/end point will not be available 
	   for some alignment sequences - ajXyzScophitsOverlapAcc will
	   return ajFalse for these.

	   */
        
	
	/* JON ajListSort(famin_hits,ajXyzScophitCompAcc); */
	ajListSort2(famin_hits,ajXyzScophitCompAcc, ajXyzScophitCompStart);
	   
	
	/* DIAGNOSTICS 
	ajFmtPrint("\n\n\nContents of famin_hits list\n");
	iter= ajListIter(famin_hits);
	while((hit1=(AjPScophit)ajListIterNext(iter)))
	    ajFmtPrint("%S (%d-%d, %S, %B %B)\n", 
		       hit1->Acc, hit1->Start, hit1->End, hit1->Typeobj, hit1->Target, hit1->Target2);
	ajListIterFree(iter);
	*/

	iter = ajListIter(famin_hits);
	hit1 = (AjPScophit)ajListIterNext(iter);
	   
	while((hit2=(AjPScophit)ajListIterNext(iter)))
	{ 

	    /*DIAGNOSTICS */
	    /*
	    ajFmtPrint("Comparing %S (%d-%d, %S, %B %B) to %S (%d-%d, %S, %B %B)\n", 
		       hit1->Acc, hit1->Start, hit1->End, hit1->Typeobj, hit1->Target, hit1->Target2, 
		       hit2->Acc, hit2->Start, hit2->End, hit2->Typeobj, hit2->Target, hit2->Target2);
	    */
	    
		       

/*	      ajFmtPrint("Comparing %S (%S) to %S (%S)\n", 
		       hit1->Acc, hit1->Typeobj, 
		       hit2->Acc, hit2->Typeobj);  */
	    
	    
	    if(ajXyzScophitsOverlapAcc(hit1,hit2,overlap))
	    {
		/*target the hit that is not a SEED  */
		isseed1 = ajStrMatchC(hit1->Typeobj, "SEED");
		isseed2 = ajStrMatchC(hit2->Typeobj, "SEED");
		
		if(isseed1 && isseed2)
		{
		    ajWarn("2 domains in the alignment share the same accession "
			   "number and overlap.\n"
			   "Only one will be given in the validation file.");
		    ajFmtPrintF(logf, "Overlap between 2 SEEDs (acc. %S) from alignment file.\n", 
				hit1->Acc);
		    ajXyzScophitTarget2(&hit1);
		}
		else if(!isseed1 && !isseed2)
		    ajFatal("Overlap between two non-SEED's which should never happen");
		else if(!isseed1)
		    {
			ajXyzScophitTarget2(&hit1);

			/*DIAGNOSTICS
			ajFmtPrint("Targetted hit1   %S (%d-%d, %B %B) : %S (%d-%d, %B %B)\n", 
				   hit1->Acc, hit1->Start, hit1->End, hit1->Target, hit1->Target2, 
				   hit2->Acc, hit2->Start, hit2->End, hit2->Target, hit2->Target2);
				   */

		    }
		
		else if(!isseed2)
		{
			ajXyzScophitTarget2(&hit2);

		    /*DIAGNOSTICS
			ajFmtPrint("Targetted hit2   %S (%d-%d, %B %B) : %S (%d-%d, %B %B)\n", 
				   hit1->Acc, hit1->Start, hit1->End, hit1->Target, hit1->Target2, 
				   hit2->Acc, hit2->Start, hit2->End, hit2->Target, hit2->Target2);
				   */

		    }
		

	    }   
	    /* move one node along  */
	    hit1 = hit2;
	}
	   
	ajListIterFree(iter);
	iter=NULL;

	/* DIAGNOSTICS
	printf("\n\n");
	iter = ajListIter(famin_hits);
	while((hit2=(AjPScophit)ajListIterNext(iter)))
	{
	    ajFmtPrint("%S (%B)\n", hit2->Acc, hit2->Target2);
	}
	printf("\n\n");
	ajListIterFree(iter);
	iter=NULL;
	*/



	/* Construct a sequence set from the main list of Scophit's */
        iter = ajListIter(famin_hits);        
        while((tmp_hit=(AjPScophit)ajListIterNext(iter)))
        {
	    /* Do not want to include domains from the alignment that 
	       duplicate the same accesion number and overlap ! */
	    if(tmp_hit->Target2)
		continue;
	    
            tmp_seq = ajSeqNew();
            ajStrAss(&tmp_seq->Name,tmp_hit->Acc);
            ajStrAss(&tmp_seq->Seq,tmp_hit->Seq);
            ajListPushApp(famin_seqs,tmp_seq);
	    /* famin_seqs must be popped and the nodes freed */
        }
        ajListIterFree(iter);
        iter=NULL;
	


	


        /* Remove the redundancy from the sequence set */
/*JISON*/        keep        = ajIntNew();
        embXyzSeqsetNR(famin_seqs, &keep, &nsetnr, matrix, gapopen, gapextend, thresh);

        /* clean up famin_seqs */
        iter=ajListIter(famin_seqs);
        while((tmp_seq=(AjPSeq)ajListIterNext(iter)))
            ajSeqDel(&tmp_seq);
        ajListIterFree(iter);
        ajListDel(&famin_seqs);
	

        /* Create a list of Scophit objects that has been processed */
        for(iter = ajListIter(famin_hits), i = 0;
	    (tmp_hit = (AjPScophit)ajListIterNext(iter));i++)
        {
	    /* Redundancy will not have been calculated for domains from the alignment that
	       duplicate the same accesion number and overlap ! */
	    if(tmp_hit->Target2)
	    {
		i--;
		/* Push the hit onto tmp_list for later free'ing */
		ajListPushApp(tmp_list,tmp_hit);
		continue;
	    }
	    

	    /* For extended alignemnts, all hits in the scop families output 
	       file are marked up as 'SEED' */
	    if(ajStrChar(*mode, 0) == '2')
	    {
		ajStrAssC(&(tmp_hit->Typeobj), "SEED");
	    }

            if(ajIntGet(keep,i))
	    {
		ajStrAssC(&(tmp_hit->Group), "NON_REDUNDANT");
		ajListPushApp(famout_hits,tmp_hit);
	    }
	    else
	    {
		ajStrAssC(&(tmp_hit->Group), "REDUNDANT");

/*		if(ajStrMatchC(tmp_hit->Acc, "P02213"))
		    printf("*********REDUNDANT written\n"); */
		

		/* Push the hit onto tmp_list for later free'ing */
		ajListPushApp(tmp_list,tmp_hit);
	    }
	    

	    /* All the memory for famin_hits is caught by tmp_list or famout_hits.
	       Both must be popped and the nodes freed */
	}   


/*JISON*/        ajIntDel(&keep);
	
        ajListIterFree(iter);
        iter = NULL;


	/* DIAGNOSTICS
	printf("\n\n");
	iter = ajListIter(famin_hits);
	while((hit2=(AjPScophit)ajListIterNext(iter)))
	{
	    ajFmtPrint("%S (%B)\n", hit2->Acc, hit2->Target2);
	}
	printf("\n\n");
	ajListIterFree(iter);
	iter=NULL;
	*/


	/* Write Hitlist for printing out to validation file.

	   This corresponds to famin_hits before redundant domains and 
	   hits targetted for removal have been removed (garbage collection).  
	   
	   ajXyzScophitsAccToHitlist will only write hits to the Hitlist 
	   if an accession number is given.
	   
	   Also, any hits that overlap with an alignment sequence with identical 
	   accession number will NOT be written (Target2==ajTrue for these).

	   This avoids introducing duplicate hits in the validation file
	   when incorporating hits from the alignment.
	    

	   */

	/* famout_valid and iter must be NULL in this context */
        ajXyzScophitsAccToHitlist(famin_hits,&famout_valid,&iter);
        ajListDel(&famin_hits);
        ajListIterFree(iter);
        iter = NULL;

	/* Write validation file */
        ajXyzHitlistWrite(voutf,famout_valid);
        ajXyzHitlistDel(&famout_valid);
        famout_valid = NULL;

	

        /* Remove targeted hits ... i.e. the ones from the alignment*/
        ajListGarbageCollect(famout_hits,ajXyzScophitDelWrap,
			     (int(*)(const void*))ajXyzScophitCheckTarget);



        /* write a hitlist of the processed scophits */
	/* famout and iter must be NULL in this context */
        ajXyzScophitsToHitlist(famout_hits,&famout,&iter);
	/* famout must be freed */
	
        ajListIterFree(iter);
        iter = NULL;

        /* free the nodes in famout_hits and the list itself  */
        iter=ajListIter(famout_hits);
        while((tmp_hit=(AjPScophit)ajListIterNext(iter)))
            ajXyzScophitDel(&tmp_hit);
        ajListIterFree(iter);
        ajListDel(&famout_hits);


	/* We are now done with the main list of Scophit's. Memory in tmp_list 
	   can also be freed*/
        iter=ajListIter(tmp_list);
        while((tmp_hit=(AjPScophit)ajListIterNext(iter)))
            ajXyzScophitDel(&tmp_hit);
        ajListIterFree(iter);
        ajListDel(&tmp_list);
	iter=NULL;


        /* write the processed family (hitlist) to file in EMBL format */
	if(ajStrChar(*mode, 0) == '2')
	{
	    ajXyzHitlistWrite(outf,famout);
	}
	
        ajXyzHitlistDel(&famout);
        famout = NULL;



        /* clean up */
/*JISON        ajIntDel(&keep); */
        ajXyzScopalgDel(&align);
        ajFileClose(&aln_inf);
        ajListDel(&scop_align);
        
    }
    
    ajFileClose(&vinf);
    ajFileClose(&voutf);

    ajStrDel(&voutfname);


    /* clean up */
    ajFileClose(&logf);
    ajFileClose(&inf);
    if(ajStrChar(*mode, 0) == '2')
    {
	ajFileClose(&outf);
    }
    
    ajFileClose(&scop_inf);
    ajStrDel(&alignpath);
    ajStrDel(&alignextn);
    ajMatrixfDel(&matrix);

    ajStrDel(&alignname);
    ajStrDel(&exec);
    

    iter=ajListIter(scop_list);
    while((tmp_scop=(AjPScop)ajListIterNext(iter)))
        ajXyzScopDel(&tmp_scop);
    ajListIterFree(iter);
    ajListDel(&scop_list);
    ajListDel(&tmp_list);
    
    AJFREE(scop_arr);
    
    ajStrDel(&mode[0]);
    AJFREE(mode);


    ajExit();
    return 0;

}












