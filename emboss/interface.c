/* @source interface application
**
** Reads coordinate files and writes inter-chain contact files
** 
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
******************************************************************************
** 
** 
** 
** 
** 
** 
** Operation
** 
** interface parses an embl-like clean coordinate file generated by the 
** pdbparse application and writes a file of inter-chain residue-residue contact 
** data in embl-like format.  The file contains residue contact data for all 
** pairs of chains in each model in the coordinate file.
**
** Contact between two residues is defined as when the van der Waals surface of 
** any atom of the first residue comes within the threshold contact distance of 
** the van der Waals surface of any atom of the second residue. The threshold 
** contact distance is a user-defined distance with a default value of 1 
** Angstrom.
** 
** The following van der Waals radii are used: 
** C:1.8 Angstrom 
** O:1.4 Angstrom 
** N:1.7 Angstrom 
** S:2.0 Angstrom
** H:1.0 Angstrom (default for other or unknown atom types)
**
** The embl-like format used for the contact files (Figure 1) uses the following 
** records:
** (1)  ID - the 4-character PDB identifier code.
** (2)  DE - bibliographic information.  The text "Residue-residue contact 
** data" is always given.
** (3)  EX - experimental information. The value of the threshold contact 
** distance is given as a floating point number after 'THRESH'. The number of 
** models and number of polypeptide chains are given after 'NMOD' and 'NCHA' 
** respectively. domain coordinate files a 1 is always given. Following the EX 
** record, the file will have a section containing a PA, IN and SM records 
** (see below) for each chain pair. The sections for each chain pair of a model 
** are given after the MO record.
** (4) MO - model number. The number given in brackets after this record 
** indicates the start of a section of model-specific data. 
** (5) PA - chain pair number.  The numbers given either side of the ':' after 
** this record indicate the start of a section of chain pair-specific data.
** (6) IN - chain specific data. The characters
** given after ID1 and ID2 are the PDB chain identifiers for the pair taken from the input file, (a '.` 
** given in cases where a chain identifier was not specified in the original 
** pdb file). The number of amino acid residues comprising each chain is given 
** after NR1 and NR2 respectively. The number of residue-residue contacts is given after NSMCON. 
** (7) SM - Line of residue contact data. Pairs of amino acid identifiers and 
** residue numbers are delimited by a ';'. Residue numbers are taken from the 
** clean coordinate file and give a correct index into the sequence (i.e. they
** are not necessarily the same as the original pdb file).  The first residue 
** belongs to the first partner of the chain-pair, the second residue belongs 
** to the second partner.
** (8) XX - used for spacing.
** (9) // - given on the last line of the file only.
** 
** Note - SM records are used for contacts between either either side-chain 
** or main-chain atoms as defined above.  In a future implementation, SS will
** be used for side-chain only contacts, MM will be used for main-chain only 
** contacts, and there will probably be several other forms of contact too.
** 
** interface generates a log file an excerpt of which is shown (Figure 2). If 
** there is a problem in processing a coordinate file, three lines containing 
** the record '//', the pdb identifier code and an error message respectively 
** are written. The text 'WARN  file open error filename',
** 'ERROR file read error filename' or 'ERROR file write error filename ' will 
** be reported when an error was encountered during a file open, read or write 
** respectively.  Various other error messages may also be given (in case of 
** difficulty email Jon Ison, jison@hgmp.mrc.ac.uk).
**
**
** Figure 1 Excerpt from interface output file
**  ID   2hhb
**  XX
**  DE   Residue-residue side-chain contact data
**  XX
**  EX   THRESH 10.0; NMOD 1; NCHA 2;
**  XX
**  MO   [1]
**  XX
**  PA   1:2
**  XX
**  IN   ID1 A; ID2 B; NR1 146; NR2 127; NSMCON 2514;
**  XX
**  SM   VAL 1 ; HIS 2
**  SM   VAL 1 ; LEU 3
**  SM   VAL 1 ; THR 4
**  SM   VAL 1 ; PRO 5
**  SM   VAL 1 ; GLU 6
**  SM   VAL 1 ; GLU 7
**  SM   VAL 1 ; LYS 8
**  SM   VAL 1 ; VAL 11
**  SM   VAL 1 ; PHE 71
**
**
** Figure 2 Excerpt of log file
** //
** DS002__
** WARN  Could not open for reading cpdb file s002.pxyz
** //
**
** Important Note
**
** Interface is designed to work with OLD FORMAT clean pdb files - i.e. the 
** ones currently found in /data/cpdb/ on the HGMP server.  To convert to 
** parsing of new format files (when available) change ajXyzCpdbReadOld to 
** ajXyzCpdbRead().
******************************************************************************/






#include "emboss.h"
#include "math.h"

static AjBool interface_WriteFile(AjPFile logf, AjPFile outf, float thresh, 
				     float ignore, AjPPdb pdb, AjPVdwall vdw);

static AjBool interface_ContactMapWrite(AjPFile outf, AjPInt2d mat, char *txt, 
						  ajint mod, ajint chn1, ajint chn2, AjPPdb pdb);

static AjBool interface_ContactMapCalc(AjPInt2d *mat, ajint *ncon, 
					    float thresh, float ignore, ajint mod, ajint chn1, 
					    ajint chn2, AjPPdb pdb, AjPVdwall vdw);






/* @prog interface *******************************************************
**
** Testing
**
******************************************************************************/

int main(ajint argc, char **argv)
{
    AjPStr     msg           =NULL;	/* Error message */
    AjPStr     temp          =NULL;	/* Temp string */

    AjPFile    cpdb_inf      =NULL;     /* cpdb input file pointer */
    AjPFile    con_outf      =NULL;     /* contact output file pointer */
    AjPFile    logf          =NULL;     /* log file pointer*/
    AjPFile    vdwf          =NULL;     /* van der Waals file pointer*/
    AjPStr     vdwfstr       =NULL;
    
    AjPPdb     pdb           =NULL;

    float      thresh        =0;
    float      ignore        =0;
    
    AjPVdwall  vdw           =NULL;     /* Structure for vdw radii */
    

    
    /* Initialise strings */
    temp          = ajStrNew();
    msg           = ajStrNew();
    vdwfstr       = ajStrNew();


    /* Read data from acd */
    embInit("interface",argc,argv); 
    cpdb_inf      = ajAcdGetInfile("in");
    con_outf      = ajAcdGetOutfile("out");
    logf          = ajAcdGetOutfile("conerrf");
    thresh        = ajAcdGetFloat("thresh");
    ignore        = ajAcdGetFloat("ignore");
    vdwfstr       = ajAcdGetString("vdwf");


    
    /* Allocate and read Vdwall object */
    ajFileDataNew(vdwfstr,&vdwf);
    if(!vdwf)
	ajFatal("Cannot open %S",vdwfstr);

    if(!ajXyzVdwallRead(vdwf, &vdw))
	ajFatal("Error reading vdw radii file\n");


    /* Read pdb structure */
    if(!ajXyzCpdbReadOld(cpdb_inf, &pdb))	       
    {
	ajFmtPrintS(&msg, "ERROR file read error");
	ajWarn(ajStrStr(msg));
	ajFmtPrintF(logf, "ERROR  file read error\n");
	
	ajFileClose(&cpdb_inf);
	
	ajXyzPdbDel(&pdb);
	
	ajStrDel(&temp);	
	ajStrDel(&msg);
	ajStrDel(&vdwfstr);
	
	ajExit();
	return 0;
    }


    /* Check that pdb file contained at least 2 chains */
    if(pdb->Nchn <= 1)
    {
	ajFmtPrintS(&msg, "WARN  PDB file does not contain enough chains");
	ajWarn(ajStrStr(msg));
	ajFmtPrintF(logf, "WARN  PDB file does not contain enough chains");
		
	ajFileClose(&cpdb_inf);
	
	ajXyzPdbDel(&pdb);
	
	ajStrDel(&temp);	
	ajStrDel(&msg);
	ajStrDel(&vdwfstr);
	
	ajExit();
	return 0;
    }


    /* Write contacts file */
    if(!interface_WriteFile(logf, con_outf, thresh, ignore, pdb, vdw))
    {
	ajFmtPrintS(&msg, "ERROR  file write error");
	ajWarn(ajStrStr(msg));
	ajFmtPrintF(logf, "ERROR  file write error\n");

	ajFileClose(&cpdb_inf);
	ajFileClose(&con_outf);

	ajXyzPdbDel(&pdb);
	
	ajStrDel(&temp);	
	ajStrDel(&msg);
	ajStrDel(&vdwfstr);

	ajExit();
	return 0;
    }


    /* Write diagnostic */
    ajFmtPrint("%S\n", pdb->Pdb);   
    ajFmtPrintF(logf, "%S\n", pdb->Pdb);   
	

    /* Tidy up*/
    ajFileClose(&cpdb_inf);
    ajFileClose(&con_outf);

    ajXyzPdbDel(&pdb);

    ajStrDel(&temp);	
    ajStrDel(&msg);
    ajStrDel(&vdwfstr);
    
    ajFileClose(&logf);
    ajFileClose(&vdwf);

    ajXyzVdwallDel(&vdw);
    

    /* Return */
    ajExit();
    return 0;
}	









/* @funcstatic interface_WriteFile *****************************************
**
** Writes a file of INTER-chain residue-residue contact data in embl-like 
** format for a pdb structure.
**
** @param [w] logf   [AjPFile]     Output file stream (log file)
** @param [w] outf   [AjPFile]     Output file stream (contact file)
** @param [r] thresh [float]       Threshold contact distance
** @param [r] ignore [float]       Threshold ignore distance
** @param [r] pdb    [AjPPdb]      Pdb object
** @param [r] vdw    [AjPVdwall]   Vdwall object
**
** @return [AjBool] True on success
** @@
**
******************************************************************************/

static AjBool interface_WriteFile(AjPFile logf, AjPFile outf, float thresh, 
				     float ignore, AjPPdb pdb, AjPVdwall vdw)
{
    AjPInt2d    mat   =NULL;		/* Contact map */
    ajint       w,x,y,z;		/* Loop counters */
    ajint       ncon  =0;		/* No. contacts (1's) in matrix */
    




    /* Error checking on args */
    if(!logf || !outf || !pdb)
	return ajFalse;
    
    
    /* Print out header data */
    ajFmtPrintF(outf, "%-5s%S\n", "ID", pdb->Pdb);
    ajFmtPrintF(outf, "XX\n");
    ajFmtPrintF(outf, "%-5s%s\n", 
		"DE", "Residue-residue side-chain contact data");
    ajFmtPrintF(outf, "XX\n");
    ajFmtPrintF(outf, "%-5s%s%.1f; %s%.1f; NMOD %d; NCHA %d;\n", "EX", 
		"THRESH ", thresh, "IGNORE ", ignore, pdb->Nmod, pdb->Nchn);


    /* Start of loop to print out data for each model / chain */
    for(x=0;x<pdb->Nmod;x++)
    {
	/* Print out model-specific data */
	ajFmtPrintF(outf, "XX\n%-5s[%d]\n", "MO", x+1);	
	ajFmtPrintF(outf, "XX\n");	

	for(y=0;y<pdb->Nchn;y++)
	{
	    for(w=y+1;w<pdb->Nchn;w++)
	    {
		/* Cope with chains with no residues */
		if((pdb->Chains[w]->Nres == 0) || (pdb->Chains[y]->Nres == 0))
		{
		    /* Print out chain pair-specific data */
		    ajFmtPrintF(outf, "%-5s%d:%d\n", "PA", y+1, w+1);	
		    ajFmtPrintF(outf, "XX\n");	
		    ajFmtPrintF(outf, "%-5sID1 %c; ID2 %c; NR1 %d; NR2 %d; NSMCON 0;\n", 
				"IN", 
				pdb->Chains[y]->Id,
				pdb->Chains[w]->Id,
				pdb->Chains[y]->Nres,
				pdb->Chains[w]->Nres);
		    ajFmtPrintF(outf, "XX\n");	

		    /* Empty matrix ! */
		    ajFmtPrintF(outf, ".\nXX\n");	
		}
		else
		{
		    

		    /* Allocate memory for the contact map (a 2d int array, 
		       no. rows = no. res. in chain y,
		       no. columns = no. res. in chain w)*/
		    mat = ajInt2dNewL((ajint)pdb->Chains[y]->Nres);   	
		    for(z=0;z<pdb->Chains[y]->Nres;++z)
			ajInt2dPut(&mat, z, pdb->Chains[w]->Nres-1, (ajint) 0);

	    
		    /* Write the contact map */

		    if(!interface_ContactMapCalc(&mat, &ncon, thresh, ignore, 
						 x+1, y+1, w+1, pdb,  vdw))
		    {
			ajFmtPrintF(logf, "ERROR  Writing contact map\n");
			ajInt2dDel(&mat);
			return ajFalse;
		    }


		    /* Print out chain pair-specific data */
		    ajFmtPrintF(outf, "%-5s%d:%d\n", "PA", y+1, w+1);	
		    ajFmtPrintF(outf, "XX\n");	
		    ajFmtPrintF(outf, "%-5sID1 %c; ID2 %c; NR1 %d; NR2 %d; NSMCON %d;\n", 
				"IN", 
				pdb->Chains[y]->Id,
				pdb->Chains[w]->Id,
				pdb->Chains[y]->Nres,
				pdb->Chains[w]->Nres,
				ncon);
		    ajFmtPrintF(outf, "XX\n");	


		    if(ncon)
		    {
			/*Matrix written OK and has at least 1 contact*/
			interface_ContactMapWrite(outf, mat, "SM", x+1, y+1, w+1, pdb);   
			ajFmtPrintF(outf, "XX\n");	
		    }
		    else 
		    {
			/* Empty matrix ! */
			ajFmtPrintF(outf, ".\nXX\n");	
		    }


		    /* Free memory for matrix */
		    ajInt2dDel(&mat);
		}
		
	    }
	}
    }
    /* End of loop to print out data for each model / chain */
    ajFmtPrintF(outf, "//\n");    


    /* Return */
    return ajTrue;
}





/* @funcstatic interface_ContactMapWrite **********************************
**
** Writes data in a contact map to file for a certain model and chain pair
** in a pdb structure. The contact map must contain inter-chain contacts.  
** The rows refer to residues in chain 1 and the columns to residues in 
** chain 2. 
**
** @param [w] outf [AjPFile]     Output file stream (contact file)
** @param [r] mat  [AjPInt2d]    Contact map
** @param [r] txt  [char*]        Text to print at start of each line
** @param [r] mod  [ajint]         Model number
** @param [r] chn1 [ajint]         Chain number 1 (rows)
** @param [r] chn2 [ajint]         Chain number 2 (columns)
** @param [r] pdb  [AjPPdb]      Pdb object
** 
** It writes a line to the output file for each contact. In the example below, 
** txt has a value of "SM":
** 
** SM   His 2 ; Val 8
** SM   Ala 6 ; Trp 21
** 
** @return [AjBool] True if file was succesfully written.
** @@
**
****************************************************************************/
static AjBool interface_ContactMapWrite(AjPFile outf, AjPInt2d mat, 
						  char *txt, ajint mod, 
						  ajint chn1, ajint chn2, 
						  AjPPdb pdb)
{
    ajint      x,y;			/* Loop counters */    
    AjPStr   res1;			/* ID of residue 1 */
    AjPStr   res2;			/* ID of residue 2 */


    /* Error checking on args */
    if(!outf || !mat || !pdb || !txt)
    {
	ajWarn("Null arg. passed to interface_PrintContactMap");
	return ajFalse;
    }
    

    /* Allocate strings */
    res1 = ajStrNew();
    res2 = ajStrNew();


    /* Start of loop for printing out contacts */
    for(x=0; x<pdb->Chains[chn1-1]->Nres; x++)
	for(y=0; y<pdb->Chains[chn2-1]->Nres; y++)
	{
	    if((ajInt2dGet(mat, x, y)==1))
	    {
		/* Assign residue id */
		if(!ajBaseAa1ToAa3(ajStrChar(pdb->Chains[chn1-1]->Seq, x), 
				   &res1))
		{
		    ajStrDel(&res1);
		    ajStrDel(&res2);  
		    ajWarn("Index out of range in "
			   "interface_ContactMapWrite");		
		    return ajFalse;
		}
		if(!ajBaseAa1ToAa3(ajStrChar(pdb->Chains[chn2-1]->Seq, y), 
				   &res2))
		{
		    ajStrDel(&res1);
		    ajStrDel(&res2);  
		    ajWarn("Index out of range in "
			   "interface_ContactMapWrite");		
		    return ajFalse;
		}
 		/* Print out the contact */
		ajFmtPrintF(outf, "%-5s%S %d ; %S %d\n", txt, res1, x+1, 
			    res2, y+1);	
	    }
	}

    
    /*Tidy up */
    ajStrDel(&res1);
    ajStrDel(&res2);  


    /* Return */
    return ajTrue;
}	









/* @funcstatic contacts_ContactMapCalc **************************************
**
** Write a contact map for a certain model and chain in a pdb structure. The
** contact map must contain intra-chain contacts. Rows and columns in the 
** square contact map correspond to residues in the chain.
**
** @param [w] mat    [AjPInt2d]   Contact map
** @param [w] ncon   [ajint *]    No. contacts
** @param [r] dim    [ajint]      Dimension of matrix (no. residues) 
** @param [r] thresh [float]      Threshold distance at which contact between 
**                                two residues is defined.
** @param [r] ignore [float]      Threshold "ignore" distance - this is a 
**                                speed-up.  Contact is not checked for between 
**                                residues with CA atoms a further distance 
**                                apart than this.
** @param [r] mod    [ajint]      Model number
** @param [r] chn    [ajint]      Chain number
** @param [r] pdb    [AjPPdb]     Pdb object
** @param [r] pdb    [AjPVdw]     Vdw object
** 
** @return [AjBool] True if file was succesfully written.
** @@
**
****************************************************************************/
static AjBool interface_ContactMapCalc(AjPInt2d *mat, ajint *ncon, 
					    float thresh, float ignore, ajint mod, ajint chn1,
					    ajint chn2, AjPPdb pdb, AjPVdwall vdw)
{	
    /* Contact is checked for between two residues, res1 (belonging to chn1)
       and res2 (belonging to chn2)*/
    
    
    AjPAtom    *arr1  =NULL;   /* Array of AjPAtom objects from list of AjPAtom objects
				 for chain <chn1> from <pdb>*/
    AjPAtom    *arr2  =NULL;   /* Array of AjPAtom objects from list of AjPAtom objects
				 for chain <chn2> from <pdb>*/
    ajint       siz1  =0;      /* Size of <arr1> */
    ajint       siz2  =0;      /* Size of <arr2> */
    
    
    ajint       idx1first=0;   /* Index in <arr1> of first atom belonging to model <mod> chain <chn1> */
    ajint       idx2first=0;   /* Index in <arr2> of first atom belonging to model <mod> chain <chn1> */
    ajint       idx1last=0;    /* Index in <arr1> of last atom belonging to model <mod> chain <chn1> */
    ajint       idx2last=0;    /* Index in <arr2> of last atom belonging to model <mod> chain <chn1> */
    
    ajint       res1first=0;   /* Residue number of first atom belonging to model <mod> chain <chn1> */
    ajint       res1last=0;    /* Residue number of last atom belonging to model <mod> chain <chn2> */
    ajint       res2first=0;   /* Residue number of first atom belonging to model <mod> chain <chn1> */
    ajint       res2last=0;    /* Residue number of last atom belonging to model <mod> chain <chn2> */
    
    
    ajint       idx1=0;       /* Index in <arr1> for atom from residue 1 */
    ajint       idx2=0;       /* Index in <arr2> for atom from residue 2 */
    
    ajint       idx1firstres=0;  /* Index in <arr1> of first atom from residue 1 */
    ajint       idx2firstres=0;  /* Index in <arr2> of first atom from residue 2 */
    
    ajint       res1=0;       /* Residue number of residue 1 */
    ajint       res2=0;       /* Residue number of residue 2 */
    
    AjBool      done=ajFalse; /* Flag */
    
    float       dis=0.0;      /* Inter-atomic distance */
    



    /* Error checking on args */
    if(!mat || !pdb || !chn1 || !chn2 || !mod)
    {	
	ajWarn("Null or zero arg. passed to interface_ContactMapCalc");
	return ajFalse;
    }
    

    /* Initialise no. contacts to zero */
    (*ncon)=0;


    
    /*Convert the AjPList of atoms to an array of AjPAtom*/
    if(!(siz1=ajListToArray((AjPList)pdb->Chains[chn1-1]->Atoms,
			   (void ***)&arr1)))
    {
	ajWarn("Zero sized list of sequences passed into "
	       "interface_ContactMapCalc");
	return ajFalse;
    }


    if(!(siz2=ajListToArray((AjPList)pdb->Chains[chn2-1]->Atoms,
			   (void ***)&arr2)))
    {
	ajWarn("Zero sized list of sequences passed into "
	       "interface_ContactMapCalc");
	return ajFalse;
    }



/*    
    ajFmtPrintF(xxxtemp, "Last node %d res %d\n", siz-1, arr[siz-1]->Idx);
    ajFmtPrint("Last node %d res %d\n", siz-1, arr[siz-1]->Idx);    
  */
  


    /*
    for(x=0;x<siz;x++)
    {
	ajFmtPrintF(xxxtemp, "idx%5d res%5d\n", x, arr[x]->Idx);
	fflush(xxxtemp->fp);
    }
    */
    


    
    /* Find index in <arr1> of first atom belonging to correct model for <chn1> */
    for(done=ajFalse, idx1first=0; idx1first<siz1; idx1first++) 
	/* Find the correct model */
	if(arr1[idx1first]->Mod==mod && arr1[idx1first]->Chn==chn1 && arr1[idx1first]->Type=='P')
	{
	    done=ajTrue;
	    res1first=arr1[idx1first]->Idx;
	    break;
	}


    if(!done)
    {
	ajWarn("Model not found in interface_ContactMapCalc");
	AJFREE(arr1);
	return ajFalse;
    } 
    
    

/*    printf("idx1first : %d   idx1last : %d    siz1 : %d\n", idx1first, idx1last, siz1);
    fflush(stdout); */

    /* Find index in <arr1> of last atom belonging to correct model for <chn1> */
    for(idx1last=idx1first; idx1last<siz1; idx1last++)
    {
	if(arr1[idx1last]->Mod!=mod  || arr1[idx1last]->Chn!=chn1 || arr1[idx1last]->Type!='P')
	{
	    idx1last--;
	    break;	
	}
    }
    

    if(idx1last==siz1)
	idx1last--;
    if(idx1last == idx1first)
    {
	ajWarn("No atoms for the chain in interface_ContactMapCalc");
	AJFREE(arr1);
	return ajFalse;
    } 

    res1last=arr1[idx1last]->Idx;
    


    /* Find index in <arr2> of first atom belonging to correct model for <chn2> */
    for(done=ajFalse, idx2first=0; idx2first<siz2; idx2first++) 
	/* Find the correct model */
	if(arr2[idx2first]->Mod==mod && arr2[idx2first]->Chn==chn2 && arr2[idx2first]->Type=='P')
	{
	    done=ajTrue;
	    res2first=arr2[idx2first]->Idx;
	    break;
	}

    if(!done)
    {
	ajWarn("Model not found in interface_ContactMapCalc");
	AJFREE(arr2);
	return ajFalse;
    } 
    
    

    /* Find index in <arr2> of last atom belonging to correct model for <chn2> */
    for(idx2last=idx2first; idx2last<siz2; idx2last++)
    {
	if(arr2[idx2last]->Mod!=mod  || arr2[idx2last]->Chn!=chn2 || arr2[idx2last]->Type!='P')
	{
	    idx2last--;
	    break;	
	}
    }
    
    if(idx2last==siz2)
	idx2last--;
    if(idx2last == idx2first)
    {
	ajWarn("No atoms for the chain in interface_ContactMapCalc");
	AJFREE(arr2);
	return ajFalse;
    } 
    res2last=arr2[idx2last]->Idx;



    /*Loop for first residue */
    for(res1=res1first,  idx1=idx1first; res1<res1last; res1++)
    {
	/*Assign position of first atom of res1 */
	for(done=ajFalse, idx1firstres=idx1; idx1firstres<idx1last; idx1firstres++)
	    if(arr1[idx1firstres]->Idx == res1)
	    {
		done=ajTrue;
		break;
	    }
	if(!done)
	    continue;
		

	/*Loop for second residue */
	for(res2=res2first, idx2=idx2first; res2<=res2last; res2++)
	{
	    /*Assign position of first atom of res2 */
	    for(done=ajFalse, idx2firstres=idx2; idx2firstres<idx2last; idx2firstres++)
		if(arr2[idx2firstres]->Idx == res2)
		{
		    done=ajTrue;
		    break;
		}
	    if(!done)
		continue;
	
	    /*Loop for atoms in first residue */
	    for(done=ajFalse, idx1=idx1firstres; idx1<siz1; idx1++)
	    {
		if(arr1[idx1]->Idx != res1)
		    break;
		
		/*Loop for atoms in second residue */
		for(idx2=idx2firstres; idx2<siz2; idx2++)
		{
		    if(arr2[idx2]->Idx != res2)
			break;
		    
		    /*ajFmtPrintF(xxxtemp, "Trying%5d%5d (res %5d-%-5d)...", idx1, idx2,arr1[idx1]->Idx, arr2[idx2]->Idx);
		    fflush(xxxtemp->fp); */
		    
		    
/*		    if(ajXyzInContact(arr[idx1], arr[idx2], thresh, vdw))		 */
		    if((dis = ajXyzAtomDistance(arr1[idx1], arr2[idx2], vdw))<=thresh)
		    {
/*			ajFmtPrintF(xxxtemp, "CONTACT\n");
			fflush(xxxtemp->fp);  */

			/* Increment no. contacts and write contact map */
			(*ncon)++;
  			ajInt2dPut(mat, arr1[idx1]->Idx-1, arr2[idx2]->Idx-1, (ajint) 1);
/*			ajInt2dPut(mat, arr[idx2]->Idx-1, arr[idx1]->Idx-1, (ajint) 1); */

			done=ajTrue;
			break;
		    }
		    else if(dis >= ignore)
			{
			    /*The atoms are so far apart we should not bother finding
			      any more distances for this pair of atoms */
			    done=ajTrue;
			    break;
			}
		    
		    /* ajFmtPrintF(xxxtemp, "NO CONTACT\n");  */

		}	
		if(done)
		    break;
	    }
	}
    }

    
    /* Tidy up */
    AJFREE(arr1);
    AJFREE(arr2);
    

    /* Return */
    return ajTrue;
}

    
    
    
