#include "phylip.h"
#include "seq.h"

/* version 3.6 (c) Copyright 1993-2002 by the University of Washington.
   Written by Joseph Felsenstein, Akiko Fuseki, Sean Lamont, and Andrew Keeffe.
   Permission is granted to copy and use this program provided no fee is
   charged for it and provided that this copyright notice is not removed. */

#define MAXNUMTREES     1000000  /* bigger than number of user trees can be */

extern sequence y;

AjPSeqset* seqsets = NULL;
AjPPhyloProp phyloweights = NULL;
AjPPhyloTree* phylotrees;

ajint numseqs;
ajint numwts;


#ifndef OLDC
/* function prototypes */
//void   getoptions(void);
void emboss_getoptions(char *pgm, int argc, char *argv[]);
void   allocrest(void);
void   reallocchars(void);
void   doinit(void);
void   makeweights(void);
void   doinput(void);
void   initdnaparsnode(node **, node **, node *, long, long, long *,
        long *, initops, pointarray, pointarray, Char *, Char *, char **);
void   evaluate(node *);
void   tryadd(node *, node *, node *);
void   addpreorder(node *, node *, node *);
void   trydescendants(node *, node *, node *, node *, boolean);

void   trylocal(node *, node *);
void   trylocal2(node *, node *, node *);
void   tryrearr(node *, boolean *);
void   repreorder(node *, boolean *);
void   rearrange(node **);
void   describe(void);
void   dnapars_coordinates(node *, double, long *, double *);
void   dnapars_printree(void);
void   globrearrange(void);
void   grandrearr(void);

void   maketree(void);
void   freerest(void);
void   load_tree(long treei);

/* function prototypes */
#endif


Char infilename[FNMLNGTH], intreename[FNMLNGTH], weightfilename[FNMLNGTH];

const char* outfilename;
const char* outtreename;

char basechar[32]="ACMGRSVTWYHKDBNO???????????????";
node *root;
long chars, col, msets, ith, njumble, jumb, maxtrees, numtrees;
/*   chars = number of sites in actual sequences */
long inseed, inseed0;
double threshold;
boolean jumble, usertree, thresh, weights, thorough, rearrfirst,
          trout, progress, stepbox, ancseq, mulsets, justwts, firstset, mulf,
          multf;
steptr oldweight;
longer seed;
pointarray treenode;            /* pointers to all nodes in tree */
long *enterorder;
long *zeros;

/* local variables for Pascal maketree, propagated globally for C version: */

long minwhich;
double like, minsteps, bestyet, bestlike, bstlike2;
boolean lastrearr, recompute;
double nsteps[maxuser];
long **fsteps;
node *there, *oldnufork;
long *place;
bestelm *bestrees;
long *threshwt;
baseptr nothing;
gbases *garbage;
node *temp, *temp1, *temp2, *tempsum, *temprm, *tempadd, *tempf, *tmp, *tmp1,
       *tmp2, *tmp3, *tmprm, *tmpadd;
boolean *names;
node *grbg;
char *progname;


void emboss_getoptions(char *pgm, int argc, char *argv[])
{
  AjStatus retval;


  jumble = false;
  njumble = 1;
  outgrno = 1;
  outgropt = false;
  thresh = false;
  thorough = true;
  transvp = false;
  rearrfirst = false;
  maxtrees = 10000;
  trout = true;
  usertree = false;
  weights = false;
  mulsets = false;
  printdata = false;
  progress = true;
  treeprint = true;
  stepbox = false;
  ancseq = false;
  dotdiff = true;
  msets = 1;

  retval = embInitP (pgm, argc, argv, "PHYLIPNEW");

  seqsets = ajAcdGetSeqsetall("sequence");
  numseqs = 0;

    while (seqsets[numseqs])
	numseqs++;

    phylotrees = ajAcdGetTree("intreefile");

    if (phylotrees){
	numtrees = 0;
	while (phylotrees[numtrees])
	    numtrees++;
	usertree = true;
    }

    numwts = 0;

    phyloweights = ajAcdGetProperties("weights");
    if (phyloweights)
    {
      weights = true;
      numwts = ajPhyloPropGetSize(phyloweights);
    }

    if (numseqs > 1) {
	mulsets = true;
        msets = numseqs;
    }
    else if (numwts > 1) {
      mulsets = true;
      msets = numwts;
      justwts = true;
    }

  

    progress = ajAcdGetBool("progress");
    printdata = ajAcdGetBool("printdata");
    treeprint = ajAcdGetBool("treeprint");
    trout = ajAcdGetToggle("trout");

    if(!usertree) {
      thorough = ajAcdGetToggle("thorough");
      if(thorough) rearrfirst = ajAcdGetBool("rearrange");
      maxtrees = ajAcdGetInt("maxtrees");
      njumble = ajAcdGetInt("njumble");
      if(njumble >0) {
        inseed = ajAcdGetInt("seed");
        jumble = true; 
        emboss_initseed(inseed, &inseed0, seed);
      }
      else njumble = 1;
    }

    if((mulsets) && (!jumble)) {
      jumble = true;
      inseed = ajAcdGetInt("seed");
      emboss_initseed(inseed, &inseed0, seed);
    }
  
    outgrno = ajAcdGetInt("outgrno");
    if(outgrno != 0) outgropt = true;
    else outgrno = 1;

    thresh = ajAcdGetToggle("thresh");
    if(thresh)  threshold = ajAcdGetFloat("threshold");
    stepbox = ajAcdGetBool("stepbox");
    ancseq = ajAcdGetBool("ancseq");
    transvp = ajAcdGetBool("transversion");
   
    if (ancseq || printdata) ajAcdGetBool("dotdiff");

 
     embossoutfile = ajAcdGetOutfile("outfile");   
     emboss_openfile(embossoutfile, &outfile, &outfilename);
     
    
     if(trout) { 
       embossouttree = ajAcdGetOutfile("outtreefile");
       emboss_openfile(embossouttree, &outtree, &outtreename);
     }

    fprintf(outfile, "\nDNA parsimony algorithm, version %s\n\n",VERSION); 
    if (transvp) fprintf(outfile, "Transversion parsimony\n\n");


}  /* getoptions */


void allocrest()
{
  long i;

  y = (Char **)Malloc(spp*sizeof(Char *));
  for (i = 0; i < spp; i++)
    y[i] = (Char *)Malloc(chars*sizeof(Char));
  bestrees = (bestelm *)Malloc(maxtrees*sizeof(bestelm));
  for (i = 1; i <= maxtrees; i++)
    bestrees[i - 1].btree = (long *)Malloc(nonodes*sizeof(long));
  nayme = (naym *)Malloc(spp*sizeof(naym));
  enterorder = (long *)Malloc(spp*sizeof(long));
  place = (long *)Malloc(nonodes*sizeof(long));
  weight = (long *)Malloc(chars*sizeof(long));
  oldweight = (long *)Malloc(chars*sizeof(long));
  alias = (long *)Malloc(chars*sizeof(long));
  ally = (long *)Malloc(chars*sizeof(long));
  location = (long *)Malloc(chars*sizeof(long));
}  /* allocrest */


void doinit()
{
  /* initializes variables */

  inputnumbersseq(seqsets[0], &spp, &chars, &nonodes, 1);
  if (printdata)
    fprintf(outfile, "%2ld species, %3ld  sites\n\n", spp, chars);
  alloctree(&treenode, nonodes, usertree);
}  /* doinit */


void makeweights()
{
  /* make up weights vector to avoid duplicate computations */
  long i;

  for (i = 1; i <= chars; i++) {
    alias[i - 1] = i;
    oldweight[i - 1] = weight[i - 1];
    ally[i - 1] = i;
  }
  sitesort(chars, weight);
  sitecombine(chars);
  sitescrunch(chars);
  endsite = 0;
  for (i = 1; i <= chars; i++) {
    if (ally[i - 1] == i)
      endsite++;
  }
  for (i = 1; i <= endsite; i++)
    location[alias[i - 1] - 1] = i;
  if (!thresh)
    threshold = spp;
  threshwt = (long *)Malloc(endsite*sizeof(long));
  for (i = 0; i < endsite; i++) {
    weight[i] *= 10;
    threshwt[i] = (long)(threshold * weight[i] + 0.5);
  }
  zeros = (long *)Malloc(endsite*sizeof(long));
  for (i = 0; i < endsite; i++)
    zeros[i] = 0;
}  /* makeweights */


void doinput()
{
  /* reads the input data */
  long i;

  if (justwts) {
    if (firstset)
      seq_inputdata(seqsets[0],chars);
    for (i = 0; i < chars; i++)
      weight[i] = 1;
    inputweightsstr(phyloweights->Str[0], chars, weight, &weights);
    if (justwts) {
      fprintf(outfile, "\n\nWeights set # %ld:\n\n", ith);
      if (progress)
        printf("\nWeights set # %ld:\n\n", ith);
    }
    if (printdata)
      printweights(outfile, 0, chars, weight, "Sites");
  } else {
    if (!firstset){
      samenumspseq(seqsets[ith-1], &chars, ith);
      reallocchars();
    }
    seq_inputdata(seqsets[ith-1], chars);
    for (i = 0; i < chars; i++)
      weight[i] = 1;
    if (weights) {
      inputweightsstr(phyloweights->Str[0], chars, weight, &weights);
      if (printdata)
        printweights(outfile, 0, chars, weight, "Sites");
    }
  }
  makeweights();
  makevalues(treenode, zeros, usertree);
  if (!usertree) {
    allocnode(&temp, zeros, endsite);
    allocnode(&temp1, zeros, endsite);
    allocnode(&temp2, zeros, endsite);
    allocnode(&tempsum, zeros, endsite);
    allocnode(&temprm, zeros, endsite);
    allocnode(&tempadd, zeros, endsite);
    allocnode(&tempf, zeros, endsite);
    allocnode(&tmp, zeros, endsite);
    allocnode(&tmp1, zeros, endsite);
    allocnode(&tmp2, zeros, endsite);
    allocnode(&tmp3, zeros, endsite);
    allocnode(&tmprm, zeros, endsite);
    allocnode(&tmpadd, zeros, endsite);
  }
}  /* doinput */


void initdnaparsnode(node **p, node **grbg, node *q, long len, long nodei,
                        long *ntips, long *parens, initops whichinit,
                        pointarray treenode, pointarray nodep, Char *str,
                        Char *ch, char** treestr)
{
  /* initializes a node */
  boolean minusread;
  double valyew, divisor;

  switch (whichinit) {
  case bottom:
    gnutreenode(grbg, p, nodei, endsite, zeros);
    treenode[nodei - 1] = *p;
    break;
  case nonbottom:
    gnutreenode(grbg, p, nodei, endsite, zeros);
    break;
  case tip:
    match_names_to_data (str, treenode, p, spp);
    break;
  case length:         /* if there is a length, read it and discard value */
    processlength(&valyew, &divisor, ch, &minusread, treestr, parens);
    break;
  default:            /*cases hslength,hsnolength,treewt,unittrwt,iter,*/
    break;
  }
} /* initdnaparsnode */


void evaluate(node *r)
{
  /* determines the number of steps needed for a tree. this is
     the minimum number of steps needed to evolve sequences on
     this tree */
  long i, steps;
  long term;
  double sum;

  sum = 0.0;
  for (i = 0; i < endsite; i++) {
    steps = r->numsteps[i];
    if ((long)steps <= threshwt[i])
      term = steps;
    else
      term = threshwt[i];
    sum += (double)term;
    if (usertree && which <= maxuser)
      fsteps[which - 1][i] = term;
  }
  if (usertree && which <= maxuser) {
    nsteps[which - 1] = sum;
    if (which == 1) {
      minwhich = 1;
      minsteps = sum;
    } else if (sum < minsteps) {
      minwhich = which;
      minsteps = sum;
    }
  }
  like = -sum;
}  /* evaluate */


void tryadd(node *p, node *item, node *nufork)
{
  /* temporarily adds one fork and one tip to the tree.
     if the location where they are added yields greater
     "likelihood" than other locations tested up to that
     time, then keeps that location as there */
  long pos;
  double belowsum, parentsum;
  boolean found, collapse, changethere, trysave;

  if (!p->tip) {
    memcpy(temp->base, p->base, endsite*sizeof(long));
    memcpy(temp->numsteps, p->numsteps, endsite*sizeof(long));
    memcpy(temp->numnuc, p->numnuc, endsite*sizeof(nucarray));
    temp->numdesc = p->numdesc + 1;
    if (p->back) {
      multifillin(temp, tempadd, 1);
      sumnsteps2(tempsum, temp, p->back, 0, endsite, threshwt);
    } else {
      multisumnsteps(temp, tempadd, 0, endsite, threshwt);
      tempsum->sumsteps = temp->sumsteps;
    }
    if (tempsum->sumsteps <= -bestyet) {
      if (p->back)
        sumnsteps2(tempsum, temp, p->back, endsite+1, endsite, threshwt);
      else {
        multisumnsteps(temp, temp1, endsite+1, endsite, threshwt);
        tempsum->sumsteps = temp->sumsteps;
      }
    }
    p->sumsteps = tempsum->sumsteps;
  }
  if (p == root)
    sumnsteps2(temp, item, p, 0, endsite, threshwt);
  else {
    sumnsteps(temp1, item, p, 0, endsite);
    sumnsteps2(temp, temp1, p->back, 0, endsite, threshwt);
  }
  if (temp->sumsteps <= -bestyet) {
    if (p == root)
      sumnsteps2(temp, item, p, endsite+1, endsite, threshwt);
    else {
      sumnsteps(temp1, item, p, endsite+1, endsite);
      sumnsteps2(temp, temp1, p->back, endsite+1, endsite, threshwt);
    }
  }
  belowsum = temp->sumsteps;
  multf = false;
  like = -belowsum;
  if (!p->tip && belowsum >= p->sumsteps) {
    multf = true;
    like = -p->sumsteps;
  }
  trysave = true;
  if (!multf && p != root) {
    parentsum = treenode[p->back->index - 1]->sumsteps;
    if (belowsum >= parentsum)
      trysave = false;
  }
  if (lastrearr) {
    changethere = true;
    if (like >= bstlike2 && trysave) {
      if (like > bstlike2)
        found = false;
      else {
        addnsave(p, item, nufork, &root, &grbg, multf, treenode, place, zeros);
        pos = 0;
        findtree(&found, &pos, nextree, place, bestrees);
      }
      if (!found) {
        collapse = collapsible(item, p, temp, temp1, temp2, tempsum, temprm,
                     tmpadd, multf, root, zeros, treenode);
        if (!thorough)
          changethere = !collapse;
        if (thorough || !collapse || like > bstlike2 || (nextree == 1)) {
          if (like > bstlike2) {
            addnsave(p, item, nufork, &root, &grbg, multf, treenode, 
                       place, zeros);
            bestlike = bstlike2 = like;
            addbestever(&pos, &nextree, maxtrees, collapse, place, bestrees);
          } else
            addtiedtree(pos, &nextree, maxtrees, collapse, place, bestrees);
        }
      }
    }
    if (like >= bestyet) {
      if (like > bstlike2)
        bstlike2 = like;
      if (changethere && trysave) {
        bestyet = like;
        there = p;
        mulf = multf;
      }
    }
  } else if ((like > bestyet) || (like >= bestyet && trysave)) {
    bestyet = like;
    there = p;
    mulf = multf;
  }
}  /* tryadd */


void addpreorder(node *p, node *item, node *nufork)
{
  /* traverses a n-ary tree, calling function tryadd
     at a node before calling tryadd at its descendants */
  node *q;

  if (p == NULL)
    return;
  tryadd(p, item, nufork);
  if (!p->tip) {
    q = p->next;
    while (q != p) {
      addpreorder(q->back, item, nufork);
      q = q->next;
    }
  }
}  /* addpreorder */


void trydescendants(node *item, node *forknode, node *parent,
                        node *parentback, boolean trybelow)
{
  /* tries rearrangements at parent and below parent's descendants */
  node *q, *tempblw;
  boolean bestever=0, belowbetter, multf=0, saved, trysave;
  double parentsum=0, belowsum;

  memcpy(temp->base, parent->base, endsite*sizeof(long));
  memcpy(temp->numsteps, parent->numsteps, endsite*sizeof(long));
  memcpy(temp->numnuc, parent->numnuc, endsite*sizeof(nucarray));
  temp->numdesc = parent->numdesc + 1;
  multifillin(temp, tempadd, 1);
  sumnsteps2(tempsum, parentback, temp, 0, endsite, threshwt);
  belowbetter = true;
  if (lastrearr) {
    parentsum = tempsum->sumsteps;
    if (-tempsum->sumsteps >= bstlike2) {
      belowbetter = false;
      bestever = false;
      multf = true;
      if (-tempsum->sumsteps > bstlike2)
        bestever = true;
      savelocrearr(item, forknode, parent, tmp, tmp1, tmp2, tmp3, tmprm,
                    tmpadd, &root, maxtrees, &nextree, multf, bestever,
                    &saved, place, bestrees, treenode, &grbg, zeros);
      if (saved) {
        like = bstlike2 = -tempsum->sumsteps;
        there = parent;
        mulf = true;
      }
    }
  } else if (-tempsum->sumsteps >= like) {
    there = parent;
    mulf = true;
    like = -tempsum->sumsteps;
  }
  if (trybelow) {
    sumnsteps(temp, parent, tempadd, 0, endsite);
    sumnsteps2(tempsum, temp, parentback, 0, endsite, threshwt);
    if (lastrearr) {
      belowsum = tempsum->sumsteps;
      if (-tempsum->sumsteps >= bstlike2 && belowbetter && 
            (forknode->numdesc > 2 ||
               (forknode->numdesc == 2 && 
                 parent->back->index != forknode->index))) {
        trysave = false;
        memcpy(temp->base, parentback->base, endsite*sizeof(long));
        memcpy(temp->numsteps, parentback->numsteps, endsite*sizeof(long));
        memcpy(temp->numnuc, parentback->numnuc, endsite*sizeof(nucarray));
        temp->numdesc = parentback->numdesc + 1;
        multifillin(temp, tempadd, 1);
        sumnsteps2(tempsum, parent, temp, 0, endsite, threshwt);
        if (-tempsum->sumsteps < bstlike2) {
          multf = false;
          bestever = false;
          trysave = true;
        }
        if (-belowsum > bstlike2) {
          multf = false;
          bestever = true;
          trysave = true;
        }
        if (trysave) {
          if (treenode[parent->index - 1] != parent)
            tempblw = parent->back;
          else
            tempblw = parent;
          savelocrearr(item, forknode, tempblw, tmp, tmp1, tmp2, tmp3, tmprm,
                         tmpadd, &root, maxtrees, &nextree, multf, bestever,
                         &saved, place, bestrees, treenode, &grbg, zeros);
          if (saved) {
            like = bstlike2 = -belowsum;
            there = tempblw;
            mulf = false;
          }
        }
      }
    } else if (-tempsum->sumsteps > like) {
      like = -tempsum->sumsteps;
      if (-tempsum->sumsteps > bestyet) {
        if (treenode[parent->index - 1] != parent)
          tempblw = parent->back;
        else
          tempblw = parent;
        there = tempblw;
        mulf = false;
      }
    }
  }
  q = parent->next;
  while (q != parent) {
    if (q->back && q->back != item) {
      memcpy(temp1->base, q->base, endsite*sizeof(long));
      memcpy(temp1->numsteps, q->numsteps, endsite*sizeof(long));
      memcpy(temp1->numnuc, q->numnuc, endsite*sizeof(nucarray));
      temp1->numdesc = q->numdesc;
      multifillin(temp1, parentback, 0);
      if (lastrearr)
        belowbetter = (-parentsum < bstlike2);
      if (!q->back->tip) {
        memcpy(temp->base, q->back->base, endsite*sizeof(long));
        memcpy(temp->numsteps, q->back->numsteps, endsite*sizeof(long));
        memcpy(temp->numnuc, q->back->numnuc, endsite*sizeof(nucarray));
        temp->numdesc = q->back->numdesc + 1;
        multifillin(temp, tempadd, 1);
        sumnsteps2(tempsum, temp1, temp, 0, endsite, threshwt);
        if (lastrearr) {
          if (-tempsum->sumsteps >= bstlike2) {
            belowbetter = false;
            bestever = false;
            multf = true;
            if (-tempsum->sumsteps > bstlike2)
              bestever = true;
            savelocrearr(item, forknode, q->back, tmp, tmp1, tmp2, tmp3, tmprm,
                          tmpadd, &root, maxtrees, &nextree, multf, bestever,
                          &saved, place, bestrees, treenode, &grbg, zeros);
            if (saved) {
              like = bstlike2 = -tempsum->sumsteps;
              there = q->back;
              mulf = true;
            }
          }
        } else if (-tempsum->sumsteps >= like) {
          like = -tempsum->sumsteps;
          there = q->back;
          mulf = true;
        }
      }
      sumnsteps(temp, q->back, tempadd, 0, endsite);
      sumnsteps2(tempsum, temp, temp1, 0, endsite, threshwt);
      if (lastrearr) {
        if (-tempsum->sumsteps >= bstlike2) {
          trysave = false;
          multf = false;
          if (belowbetter) {
            bestever = false;
            trysave = true;
          }
          if (-tempsum->sumsteps > bstlike2) {
            bestever = true;
            trysave = true;
          }
          if (trysave) {
            if (treenode[q->back->index - 1] != q->back)
              tempblw = q;
            else
              tempblw = q->back;
            savelocrearr(item, forknode, tempblw, tmp, tmp1, tmp2, tmp3, tmprm,
                        tmpadd, &root, maxtrees, &nextree, multf, bestever,
                        &saved, place, bestrees, treenode, &grbg, zeros);
            if (saved) {
              like = bstlike2 = -tempsum->sumsteps;
              there = tempblw;
              mulf = false;
            }
          }
        }
      } else if (-tempsum->sumsteps > like) {
        like = -tempsum->sumsteps;
        if (-tempsum->sumsteps > bestyet) {
          if (treenode[q->back->index - 1] != q->back)
            tempblw = q;
          else
            tempblw = q->back;
          there = tempblw;
          mulf = false;
        }
      }
    }
    q = q->next;
  }
} /* trydescendants */


void trylocal(node *item, node *forknode)
{
  /* rearranges below forknode, below descendants of forknode when
     there are more than 2 descendants, then unroots the back of
     forknode and rearranges on its descendants */
  node *q;
  boolean bestever, multf, saved;

  memcpy(temprm->base, zeros, endsite*sizeof(long));
  memcpy(temprm->numsteps, zeros, endsite*sizeof(long));
  memcpy(temprm->oldbase, item->base, endsite*sizeof(long));
  memcpy(temprm->oldnumsteps, item->numsteps, endsite*sizeof(long));
  memcpy(tempf->base, forknode->base, endsite*sizeof(long));
  memcpy(tempf->numsteps, forknode->numsteps, endsite*sizeof(long));
  memcpy(tempf->numnuc, forknode->numnuc, endsite*sizeof(nucarray));
  tempf->numdesc = forknode->numdesc - 1;
  multifillin(tempf, temprm, -1);
  if (!forknode->back) {
    sumnsteps2(tempsum, tempf, tempadd, 0, endsite, threshwt);
    if (lastrearr) {
      if (-tempsum->sumsteps > bstlike2) {
        bestever = true;
        multf = false;
        savelocrearr(item, forknode, forknode, tmp, tmp1, tmp2, tmp3, tmprm,
                       tmpadd, &root, maxtrees, &nextree, multf, bestever,
                       &saved, place, bestrees, treenode, &grbg, zeros);
        if (saved) {
          like = bstlike2 = -tempsum->sumsteps;
          there = forknode;
          mulf = false;
        }
      }
    } else if (-tempsum->sumsteps > like) {
      like = -tempsum->sumsteps;
      if (-tempsum->sumsteps > bestyet) {
        there = forknode;
        mulf = false;
      }
    }
  } else {
    sumnsteps(temp, tempf, tempadd, 0, endsite);
    sumnsteps2(tempsum, temp, forknode->back, 0, endsite, threshwt);
    if (lastrearr) {
      if (-tempsum->sumsteps > bstlike2) {
        bestever = true;
        multf = false;
        savelocrearr(item, forknode, forknode, tmp, tmp1, tmp2, tmp3, tmprm,
                       tmpadd, &root, maxtrees, &nextree, multf, bestever,
                       &saved, place, bestrees, treenode, &grbg, zeros);
        if (saved) {
          like = bstlike2 = -tempsum->sumsteps;
          there = forknode;
          mulf = false;
        }
      }
    } else if (-tempsum->sumsteps > like) {
      like = -tempsum->sumsteps;
      if (-tempsum->sumsteps > bestyet) {
        there = forknode;
        mulf = false;
      }
    }
    trydescendants(item, forknode, forknode->back, tempf, false);
  }
  q = forknode->next;
  while (q != forknode) {
    if (q->back != item) {
      memcpy(temp2->base, q->base, endsite*sizeof(long));
      memcpy(temp2->numsteps, q->numsteps, endsite*sizeof(long));
      memcpy(temp2->numnuc, q->numnuc, endsite*sizeof(nucarray));
      temp2->numdesc = q->numdesc - 1;
      multifillin(temp2, temprm, -1);
      if (!q->back->tip) {
        trydescendants(item, forknode, q->back, temp2, true);
      } else {
        sumnsteps(temp1, q->back, tempadd, 0, endsite);
        sumnsteps2(tempsum, temp1, temp2, 0, endsite, threshwt);
        if (lastrearr) {
          if (-tempsum->sumsteps > bstlike2) {
            multf = false;
            bestever = true;
            savelocrearr(item, forknode, q->back, tmp, tmp1, tmp2, tmp3,
                         tmprm, tmpadd, &root, maxtrees, &nextree, multf,
                         bestever, &saved, place, bestrees, treenode,
                         &grbg, zeros);
            if (saved) {
              like = bstlike2 = -tempsum->sumsteps;
              there = q->back;
              mulf = false;
            }
          }
        } else if ((-tempsum->sumsteps) > like) {
          like = -tempsum->sumsteps;
          if (-tempsum->sumsteps > bestyet) {
            there = q->back;
            mulf = false;
          }
        }
      }
    }
    q = q->next;
  }
} /* trylocal */


void trylocal2(node *item, node *forknode, node *other)
{
  /* rearranges below forknode, below descendants of forknode when
     there are more than 2 descendants, then unroots the back of
     forknode and rearranges on its descendants.  Used if forknode
     has binary descendants */
  node *q;
  boolean bestever=0, multf, saved, belowbetter, trysave;

  memcpy(tempf->base, other->base, endsite*sizeof(long));
  memcpy(tempf->numsteps, other->numsteps, endsite*sizeof(long));
  memcpy(tempf->oldbase, forknode->base, endsite*sizeof(long));
  memcpy(tempf->oldnumsteps, forknode->numsteps, endsite*sizeof(long));
  tempf->numdesc = other->numdesc;
  if (forknode->back)
    trydescendants(item, forknode, forknode->back, tempf, false);
  if (!other->tip) {
    memcpy(temp->base, other->base, endsite*sizeof(long));
    memcpy(temp->numsteps, other->numsteps, endsite*sizeof(long));
    memcpy(temp->numnuc, other->numnuc, endsite*sizeof(nucarray));
    temp->numdesc = other->numdesc + 1;
    multifillin(temp, tempadd, 1);
    if (forknode->back)
      sumnsteps2(tempsum, forknode->back, temp, 0, endsite, threshwt);
    else
      sumnsteps2(tempsum, NULL, temp, 0, endsite, threshwt);
    belowbetter = true;
    if (lastrearr) {
      if (-tempsum->sumsteps >= bstlike2) {
        belowbetter = false;
        bestever = false;
        multf = true;
        if (-tempsum->sumsteps > bstlike2)
          bestever = true;
        savelocrearr(item, forknode, other, tmp, tmp1, tmp2, tmp3, tmprm,
                       tmpadd, &root, maxtrees, &nextree, multf, bestever,
                       &saved, place, bestrees, treenode, &grbg, zeros);
        if (saved) {
          like = bstlike2 = -tempsum->sumsteps;
          there = other;
          mulf = true;
        }
      }
    } else if (-tempsum->sumsteps >= like) {
      there = other;
      mulf = true;
      like = -tempsum->sumsteps;
    }
    if (forknode->back) {
      memcpy(temprm->base, forknode->back->base, endsite*sizeof(long));
      memcpy(temprm->numsteps, forknode->back->numsteps, endsite*sizeof(long));
    } else {
      memcpy(temprm->base, zeros, endsite*sizeof(long));
      memcpy(temprm->numsteps, zeros, endsite*sizeof(long));
    }
    memcpy(temprm->oldbase, other->back->base, endsite*sizeof(long));
    memcpy(temprm->oldnumsteps, other->back->numsteps, endsite*sizeof(long));
    q = other->next;
    while (q != other) {
      memcpy(temp2->base, q->base, endsite*sizeof(long));
      memcpy(temp2->numsteps, q->numsteps, endsite*sizeof(long));
      memcpy(temp2->numnuc, q->numnuc, endsite*sizeof(nucarray));
      if (forknode->back) {
        temp2->numdesc = q->numdesc;
        multifillin(temp2, temprm, 0);
      } else {
        temp2->numdesc = q->numdesc - 1;
        multifillin(temp2, temprm, -1);
      }
      if (!q->back->tip)
        trydescendants(item, forknode, q->back, temp2, true);
      else {
        sumnsteps(temp1, q->back, tempadd, 0, endsite);
        sumnsteps2(tempsum, temp1, temp2, 0, endsite, threshwt);
        if (lastrearr) {
          if (-tempsum->sumsteps >= bstlike2) {
            trysave = false;
            multf = false;
            if (belowbetter) {
              bestever = false;
              trysave = true;
            }
            if (-tempsum->sumsteps > bstlike2) {
              bestever = true;
              trysave = true;
            }
            if (trysave) {
              savelocrearr(item, forknode, q->back, tmp, tmp1, tmp2, tmp3,
                           tmprm, tmpadd, &root, maxtrees, &nextree, multf,
                           bestever, &saved, place, bestrees, treenode,
                           &grbg, zeros);
              if (saved) {
                like = bstlike2 = -tempsum->sumsteps;
                there = q->back;
                mulf = false;
              }
            }
          }
        } else if (-tempsum->sumsteps > like) {
          like = -tempsum->sumsteps;
          if (-tempsum->sumsteps > bestyet) {
            there = q->back;
            mulf = false;
          }
        }
      }
      q = q->next;
    }
  }
} /* trylocal2 */


void tryrearr(node *p, boolean *success)
{
  /* evaluates one rearrangement of the tree.
     if the new tree has greater "likelihood" than the old
     one sets success = TRUE and keeps the new tree.
     otherwise, restores the old tree */
  node *forknode, *newfork, *other, *oldthere;
  double oldlike;
  boolean oldmulf;

  if (p->back == NULL)
    return;
  forknode = treenode[p->back->index - 1]; 
  if (!forknode->back && forknode->numdesc <= 2 && alltips(forknode, p))
    return;
  oldlike = bestyet;
  like = -10.0 * spp * chars;
  memcpy(tempadd->base, p->base, endsite*sizeof(long));
  memcpy(tempadd->numsteps, p->numsteps, endsite*sizeof(long));
  memcpy(tempadd->oldbase, zeros, endsite*sizeof(long));
  memcpy(tempadd->oldnumsteps, zeros, endsite*sizeof(long));
  if (forknode->numdesc > 2) {
    oldthere = there = forknode;
    oldmulf = mulf = true;
    trylocal(p, forknode);
  } else {
    findbelow(&other, p, forknode);
    oldthere = there = other;
    oldmulf = mulf = false;
    trylocal2(p, forknode, other);
  }
  if ((like <= oldlike) || (there == oldthere && mulf == oldmulf))
    return;
  recompute = true;
  re_move(p, &forknode, &root, recompute, treenode, &grbg, zeros);
  if (mulf)
    add(there, p, NULL, &root, recompute, treenode, &grbg, zeros);
  else {
    if (forknode->numdesc > 0)
      getnufork(&newfork, &grbg, treenode, zeros);
    else
      newfork = forknode;
    add(there, p, newfork, &root, recompute, treenode, &grbg, zeros);
  } 
  if (like > oldlike) {
    *success = true;
    bestyet = like;
  }
}  /* tryrearr */


void repreorder(node *p, boolean *success)
{
  /* traverses a binary tree, calling PROCEDURE tryrearr
     at a node before calling tryrearr at its descendants */
  node *q, *this;

  if (p == NULL)
    return;
  if (!p->visited) {
    tryrearr(p, success);
    p->visited = true;
  }
  if (!p->tip) {
    q = p;
    while (q->next != p) {
      this = q->next->back;
      repreorder(q->next->back,success);
      if (q->next->back == this)
        q = q->next;
    }
  }
}  /* repreorder */


void rearrange(node **r)
{
  /* traverses the tree (preorder), finding any local
     rearrangement which decreases the number of steps.
     if traversal succeeds in increasing the tree's
     "likelihood", PROCEDURE rearrange runs traversal again */
  boolean success=true;

  while (success) {
    success = false;
    clearvisited(treenode);
    repreorder(*r, &success);
  }
}  /* rearrange */


void describe()
{
  /* prints ancestors, steps and table of numbers of steps in
     each site */

  if (treeprint) {
    fprintf(outfile, "\nrequires a total of %10.3f\n", like / -10.0);
    fprintf(outfile, "\n  between      and       length\n");
    fprintf(outfile, "  -------      ---       ------\n");
    printbranchlengths(root);
  }
  if (stepbox)
    writesteps(chars, weights, oldweight, root);
  if (ancseq) {
    hypstates(chars, root, treenode, &garbage, basechar);
    putc('\n', outfile);
  }
  putc('\n', outfile);
  if (trout) {
    col = 0;
    treeout3(root, nextree, &col, root);
  }
}  /* describe */


void dnapars_coordinates(node *p, double lengthsum, long *tipy,
                        double *tipmax)
{
  /* establishes coordinates of nodes */
  node *q, *first, *last;
  double xx;

  if (p == NULL)
    return;
  if (p->tip) {
    p->xcoord = (long)(over * lengthsum + 0.5);
    p->ycoord = (*tipy);
    p->ymin = (*tipy);
    p->ymax = (*tipy);
    (*tipy) += down;
    if (lengthsum > (*tipmax))
      (*tipmax) = lengthsum;
    return;
  }
  q = p->next;
  do {
    xx = q->v;
    if (xx > 100.0)
      xx = 100.0;
    dnapars_coordinates(q->back, lengthsum + xx, tipy,tipmax);
    q = q->next;
  } while (p != q);
  first = p->next->back;
  q = p;
  while (q->next != p)
    q = q->next;
  last = q->back;
  p->xcoord = (long)(over * lengthsum + 0.5);
  if ((p == root) || count_sibs(p) > 2)
    p->ycoord = p->next->next->back->ycoord;
  else
    p->ycoord = (first->ycoord + last->ycoord) / 2;
  p->ymin = first->ymin;
  p->ymax = last->ymax;
}  /* dnapars_coordinates */


void dnapars_printree()
{
  /* prints out diagram of the tree2 */
  long tipy;
  double scale, tipmax;
  long i;
 
  if (!treeprint)
    return;
  putc('\n', outfile);
  tipy = 1;
  tipmax = 0.0;
  dnapars_coordinates(root, 0.0, &tipy, &tipmax);
  scale = 1.0 / (long)(tipmax + 1.000);
  for (i = 1; i <= (tipy - down); i++)
    drawline3(i, scale, root);
  putc('\n', outfile);
}  /* dnapars_printree */


void globrearrange()
{
  /* does global rearrangements */
  long j;
  double gotlike;
  boolean frommulti;
  node *item, *nufork;

  recompute = true;
  do {
    printf("   ");
    gotlike = bstlike2 = bestlike;
    for (j = 0; j < nonodes; j++) {
      bestyet = -10.0 * spp * chars;
      if (j < spp)
        item = treenode[enterorder[j] -1];
      else 
        item = treenode[j];

      if ((item != root) && 
           ((j < spp) || ((j >= spp) && (item->numdesc > 0))) &&
           !((item->back->index == root->index) && (root->numdesc == 2)
              && alltips(root, item))) {
        re_move(item, &nufork, &root, recompute, treenode, &grbg, zeros);
        frommulti = (nufork->numdesc > 0);
        clearcollapse(treenode);
        there = root;
        memcpy(tempadd->base, item->base, endsite*sizeof(long));
        memcpy(tempadd->numsteps, item->numsteps, endsite*sizeof(long));
        memcpy(tempadd->oldbase, zeros, endsite*sizeof(long));
        memcpy(tempadd->oldnumsteps, zeros, endsite*sizeof(long));
        if (frommulti){
          oldnufork = nufork;
          getnufork(&nufork, &grbg, treenode, zeros);
        }
        addpreorder(root, item, nufork);
        if (frommulti)
          oldnufork = NULL;
        if (!mulf)
          add(there, item, nufork, &root, recompute, treenode, &grbg, zeros);
        else
          add(there, item, NULL, &root, recompute, treenode, &grbg, zeros);
      }
      if (progress) {
        if (j % ((nonodes / 72) + 1) == 0)
          putchar('.');
        fflush(stdout);
      }
    }
    if (progress) {
      putchar('\n');
#ifdef WIN32
      phyFillScreenColor();
#endif
    }
  } while (bestlike > gotlike);
} /* globrearrange */


void load_tree(long treei)
{ /* restores a tree from bestrees */
  long j, nextnode;
  boolean recompute = false;
  node *dummy;

  for (j = spp - 1; j >= 1; j--)
    re_move(treenode[j], &dummy, &root, recompute, treenode, &grbg,
              zeros);
  root = treenode[0];
  recompute = true;
  add(treenode[0], treenode[1], treenode[spp], &root, recompute,
    treenode, &grbg, zeros);
  nextnode = spp + 2;
  for (j = 3; j <= spp; j++) {
    if (bestrees[treei].btree[j - 1] > 0)
      add(treenode[bestrees[treei].btree[j - 1] - 1], treenode[j - 1],
            treenode[nextnode++ - 1], &root, recompute, treenode, &grbg,
            zeros);
    else
      add(treenode[treenode[-bestrees[treei].btree[j-1]-1]->back->index-1],
            treenode[j - 1], NULL, &root, recompute, treenode, &grbg,
            zeros);
  }
}


void grandrearr()
{
  /* calls global rearrangement on best trees */
  long treei; 
  boolean done;

  done = false;
  do {
    treei = findunrearranged(bestrees, nextree, true);
    if (treei < 0)
      done = true;
    else 
      bestrees[treei].gloreange = true;
    
    if (!done) {
      load_tree(treei);
      globrearrange();
      done = rearrfirst;
    }
  } while (!done);
} /* grandrearr */


void maketree()
{
  /* constructs a binary tree from the pointers in treenode.
     adds each node at location which yields highest "likelihood"
  then rearranges the tree for greatest "likelihood" */
  long i, j, nextnode;
  boolean done, firsttree, goteof, haslengths;
  node *item, *nufork, *dummy;
  pointarray nodep;
  char* treestr;

  if (!usertree) {
    for (i = 1; i <= spp; i++)
      enterorder[i - 1] = i;
    if (jumble)
      randumize(seed, enterorder);
    recompute = true;
    root = treenode[enterorder[0] - 1];
    add(treenode[enterorder[0] - 1], treenode[enterorder[1] - 1],
        treenode[spp], &root, recompute, treenode, &grbg, zeros);
    if (progress) {
      printf("Adding species:\n");
      writename(0, 2, enterorder);
#ifdef WIN32
      phyFillScreenColor();
#endif
    }
    lastrearr = false;
    oldnufork = NULL;
    for (i = 3; i <= spp; i++) {
      bestyet = -10.0 * spp * chars;
      item = treenode[enterorder[i - 1] - 1];
      getnufork(&nufork, &grbg, treenode, zeros);
      there = root;
      memcpy(tempadd->base, item->base, endsite*sizeof(long));
      memcpy(tempadd->numsteps, item->numsteps, endsite*sizeof(long));
      memcpy(tempadd->oldbase, zeros, endsite*sizeof(long));
      memcpy(tempadd->oldnumsteps, zeros, endsite*sizeof(long));
      addpreorder(root, item, nufork);
      if (!mulf)
        add(there, item, nufork, &root, recompute, treenode, &grbg, zeros);
      else
        add(there, item, NULL, &root, recompute, treenode, &grbg, zeros);
      like = bestyet;
      rearrange(&root);
      if (progress) {
        writename(i - 1, 1, enterorder);
#ifdef WIN32
        phyFillScreenColor();
#endif
      }
      lastrearr = (i == spp);
      if (lastrearr) {
        bestlike = bestyet;
        if (jumb == 1) {
          bstlike2 = bestlike;
          nextree = 1;
          initbestrees(bestrees, maxtrees, true);
          initbestrees(bestrees, maxtrees, false);
        }
        if (progress) {
          printf("\nDoing global rearrangements");
          if (rearrfirst)
            printf(" on the first of the trees tied for best\n");
          else
            printf(" on all trees tied for best\n");
          printf("  !");
          for (j = 0; j < nonodes; j++)
            if (j % ((nonodes / 72) + 1) == 0)
              putchar('-');
          printf("!\n");
#ifdef WIN32
          phyFillScreenColor();
#endif
        }
        globrearrange();
      }
    }
    done = false;
    while (!done && findunrearranged(bestrees, nextree, true) >= 0) {
      grandrearr();
      done = rearrfirst;
    }
    if (progress)
      putchar('\n'); 
    recompute = false;
    for (i = spp - 1; i >= 1; i--)
      re_move(treenode[i], &dummy, &root, recompute, treenode, &grbg, zeros);
    if (jumb == njumble) {
      collapsebestrees(&root, &grbg, treenode, bestrees, place, zeros,
                        chars, recompute, progress);
      if (treeprint) {
        putc('\n', outfile);
        if (nextree == 2)
          fprintf(outfile, "One most parsimonious tree found:\n");
        else
          fprintf(outfile, "%6ld trees in all found\n", nextree - 1);
      }
      if (nextree > maxtrees + 1) {
        if (treeprint)
          fprintf(outfile, "here are the first %4ld of them\n", (long)maxtrees);
        nextree = maxtrees + 1;
      }
      if (treeprint)
        putc('\n', outfile);
      for (i = 0; i <= (nextree - 2); i++) {
        root = treenode[0];
        add(treenode[0], treenode[1], treenode[spp], &root, recompute,
          treenode, &grbg, zeros);
        nextnode = spp + 2;
        for (j = 3; j <= spp; j++) {
          if (bestrees[i].btree[j - 1] > 0)
            add(treenode[bestrees[i].btree[j - 1] - 1], treenode[j - 1],
              treenode[nextnode++ - 1], &root, recompute, treenode, &grbg,
              zeros);
          else
            add(treenode[treenode[-bestrees[i].btree[j - 1]-1]->back->index-1],
              treenode[j - 1], NULL, &root, recompute, treenode, &grbg, zeros);
        }
        reroot(treenode[outgrno - 1], root);
        postorder(root);
        evaluate(root);
        treelength(root, chars, treenode);
        dnapars_printree();
        describe();
        for (j = 1; j < spp; j++)
          re_move(treenode[j], &dummy, &root, recompute, treenode,
                    &grbg, zeros);
      }
    }
  } else {
    
    if (numtrees > 2)
      emboss_initseed(inseed, &inseed0, seed);
    if (numtrees > MAXNUMTREES) {
      printf("\nERROR: number of input trees is read incorrectly from %s\n",
        intreename);
      exxit(-1);
    }
    if (treeprint) {
      fprintf(outfile, "User-defined tree");
      if (numtrees > 1)
        putc('s', outfile);
      fprintf(outfile, ":\n");
    }
    fsteps = (long **)Malloc(maxuser*sizeof(long *));
    for (j = 1; j <= maxuser; j++)
      fsteps[j - 1] = (long *)Malloc(endsite*sizeof(long));
    if (trout)
      fprintf(outtree, "%ld\n", numtrees);
    nodep = NULL;
    which = 1;
    while (which <= numtrees) {
      firsttree = true;
      nextnode = 0;
      haslengths = true;
     treestr = ajStrStrMod(&phylotrees[which-1]->Tree);
      treeread(&treestr, &root, treenode, &goteof, &firsttree,
                 nodep, &nextnode, &haslengths,
                 &grbg, initdnaparsnode);
      if (treeprint)
        fprintf(outfile, "\n\n");
      if (outgropt)
        reroot(treenode[outgrno - 1], root);
      postorder(root);
      evaluate(root);
      treelength(root, chars, treenode);
      dnapars_printree();
      describe();
      if (which < numtrees)
        gdispose(root, &grbg, treenode);
      which++;
    }
    FClose(intree);
    putc('\n', outfile);
    if (numtrees > 1 && chars > 1 )
      standev(chars, numtrees, minwhich, minsteps, nsteps, fsteps, seed);
    for (j = 1; j <= maxuser; j++)
      free(fsteps[j - 1]);
    free(fsteps);
  }
  if (jumb == njumble) {
    if (progress) {
      printf("\nOutput written to file \"%s\"\n\n", outfilename);
      if (trout) {
        printf("Tree");
        if (numtrees > 1)
          printf("s");
        printf(" also written onto file \"%s\"\n\n", outtreename);
      }
    }
  }
}  /* maketree */


void reallocchars() 
{ /* The amount of chars can change between runs 
     this function reallocates all the variables 
     whose size depends on the amount of chars */
  long i;

  for (i=0; i < spp; i++){
    free(y[i]);
    y[i] = (Char *)Malloc(chars*sizeof(Char));
  }
  free(weight);
  free(oldweight); 
  free(alias);
  free(ally);
  free(location);

  weight = (long *)Malloc(chars*sizeof(long));
  oldweight = (long *)Malloc(chars*sizeof(long));
  alias = (long *)Malloc(chars*sizeof(long));
  ally = (long *)Malloc(chars*sizeof(long));
  location = (long *)Malloc(chars*sizeof(long));
}


void freerest()
{ /* free variables that are allocated each data set */
  long i;

  if (!usertree) {
    freenode(&temp);
    freenode(&temp1);
    freenode(&temp2);
    freenode(&tempsum);
    freenode(&temprm);
    freenode(&tempadd);
    freenode(&tempf);
    freenode(&tmp);
    freenode(&tmp1);
    freenode(&tmp2);
    freenode(&tmp3);
    freenode(&tmprm);
    freenode(&tmpadd);
  }
  for (i = 0; i < spp; i++)
    free(y[i]);
  free(y);
  for (i = 1; i <= maxtrees; i++)
    free(bestrees[i - 1].btree);
  free(bestrees);
  free(nayme);
  free(enterorder);
  free(place);
  free(weight);
  free(oldweight);
  free(alias);
  free(ally);
  free(location);
  freegrbg(&grbg);
  if (ancseq)
    freegarbage(&garbage);
  free(threshwt);
  free(zeros);
  freenodes(nonodes, treenode);
}  /* freerest */


int main(int argc, Char *argv[])
{  /* DNA parsimony by uphill search */

  /* reads in spp, chars, and the data. Then calls maketree to
     construct the tree */
#ifdef MAC
   argc = 1;        /* macsetup("Dnapars","");        */
   argv[0] = "Dnapars";
#endif
  init(argc, argv);
  emboss_getoptions("fdnapars",argc,argv);
  progname = argv[0];


  ibmpc = IBMCRT;
  ansi = ANSICRT;

  firstset = true;
  garbage = NULL;
  grbg = NULL;
  doinit();

  for (ith = 1; ith <= msets; ith++) {
    if (!(justwts && !firstset))
      allocrest();
    if (msets > 1 && !justwts) {
    fprintf(outfile, "\nData set # %ld:\n\n", ith);
      if (progress)
        printf("\nData set # %ld:\n\n", ith);
    }
    doinput();
    if (ith == 1)
      firstset = false;
    for (jumb = 1; jumb <= njumble; jumb++)
      maketree();
    if (!justwts)
      freerest();
  }
  freetree(nonodes, treenode);
  FClose(infile);
  FClose(outfile);
  if (weights || justwts)
    FClose(weightfile);
  if (trout)
    FClose(outtree);
  if (usertree)
    FClose(intree);
#ifdef MAC
  fixmacfile(outfilename);
  fixmacfile(outtreename);
#endif
  if (progress)
    printf("Done.\n\n");
#ifdef WIN32
  phyRestoreConsoleAttributes();
#endif
  return 0;
}  /* DNA parsimony by uphill search */

