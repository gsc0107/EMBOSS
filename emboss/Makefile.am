## Makefile.am for EMBOSS
## Process this file with automake to produce Makefile.in in all directories.

ACLOCAL_AMFLAGS = -I m4

AUTOMAKE_OPTIONS = gnits

if ISSHARED
if ISAIXIA64
AIX_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../ajax/pcre/.libs \
-L../ajax/expat/.libs -L../ajax/zlib/.libs -L../ajax/core/.libs \
-L../ajax/graphics/.libs -L../ajax/ensembl/.libs -L../ajax/ajaxdb/.libs \
-L../ajax/acd/.libs \
 -lacd -lajaxdb -lensembl -lajaxg -lajax -leexpat -lepcre -lezlib
endif
endif

AM_CFLAGS =  $(AIX_CFLAGS) $(WARN_CFLAGS) $(DEVWARN_CFLAGS)

check_PROGRAMS = acdlog acdrelations acdxsd ajtest \
complex corbatest edamclean ensembltest \
entrails entrailsbook entrailshtml entrailswiki histogramtest intconv \
origsplitter origunion \
newcoils \
patmattest \
prima primers \
seqinfo seqretall seqretallfeat \
seqretset seqretsingle seqrettype sqltest \
testplot treetypedisplay

# tar to pick up the other directories
# then remove any CVS subdirectories


INCLUDES = -I$(top_srcdir)/nucleus -I$(top_srcdir)/ajax/pcre \
	-I$(top_srcdir)/ajax/expat -I$(top_srcdir)/ajax/zlib \
	-I$(top_srcdir)/ajax/core -I$(top_srcdir)/ajax/graphics \
	-I$(top_srcdir)/ajax/ensembl -I$(top_srcdir)/ajax/ajaxdb \
	-I$(top_srcdir)/ajax/acd -I$(top_srcdir)/plplot


aaindexextract_SOURCES = aaindexextract.c
abiview_SOURCES = abiview.c
acdc_SOURCES = acdc.c
acdpretty_SOURCES = acdpretty.c
acdtable_SOURCES = acdtable.c
acdtrace_SOURCES = acdtrace.c
acdvalid_SOURCES = acdvalid.c
acdrelations_SOURCES = acdrelations.c
acdxsd_SOURCES = acdxsd.c
aligncopy_SOURCES = aligncopy.c
aligncopypair_SOURCES = aligncopypair.c
antigenic_SOURCES = antigenic.c
backtranambig_SOURCES = backtranambig.c
backtranseq_SOURCES = backtranseq.c
banana_SOURCES = banana.c
biosed_SOURCES = biosed.c
btwisted_SOURCES = btwisted.c
chaos_SOURCES = chaos.c
cai_SOURCES = cai.c
charge_SOURCES = charge.c
checktrans_SOURCES = checktrans.c
chips_SOURCES = chips.c
cirdna_SOURCES = cirdna.c
codcmp_SOURCES = codcmp.c
coderet_SOURCES = coderet.c
compseq_SOURCES = compseq.c
cons_SOURCES = cons.c
consambig_SOURCES = consambig.c
cpgplot_SOURCES = cpgplot.c
cpgreport_SOURCES = cpgreport.c
cusp_SOURCES = cusp.c
cutgextract_SOURCES = cutgextract.c
cutseq_SOURCES = cutseq.c
dan_SOURCES = dan.c
dbiblast_SOURCES = dbiblast.c
dbifasta_SOURCES = dbifasta.c
dbiflat_SOURCES = dbiflat.c
dbigcg_SOURCES = dbigcg.c
dbxfasta_SOURCES = dbxfasta.c
dbxflat_SOURCES = dbxflat.c
dbxgcg_SOURCES = dbxgcg.c
degapseq_SOURCES = degapseq.c
density_SOURCES = density.c
descseq_SOURCES = descseq.c
diffseq_SOURCES = diffseq.c
digest_SOURCES = digest.c
distmat_SOURCES = distmat.c
dotmatcher_SOURCES = dotmatcher.c
dotpath_SOURCES = dotpath.c
dottup_SOURCES = dottup.c
dreg_SOURCES = dreg.c
edialign_SOURCES = edialign.c
einverted_SOURCES = einverted.c
embossdata_SOURCES = embossdata.c
emma_SOURCES = emma.c
emowse_SOURCES = emowse.c
entret_SOURCES = entret.c
epestfind_SOURCES = epestfind.c
eprimer3_SOURCES = eprimer3.c
equicktandem_SOURCES = equicktandem.c
est2genome_SOURCES = est2genome.c
etandem_SOURCES = etandem.c
embossversion_SOURCES = embossversion.c
extractalign_SOURCES = extractalign.c
extractfeat_SOURCES = extractfeat.c
extractseq_SOURCES = extractseq.c
featcopy_SOURCES = featcopy.c
featreport_SOURCES = featreport.c
findkm_SOURCES = findkm.c
freak_SOURCES = freak.c
fuzznuc_SOURCES = fuzznuc.c
fuzzpro_SOURCES = fuzzpro.c
fuzztran_SOURCES = fuzztran.c
garnier_SOURCES = garnier.c
geecee_SOURCES = geecee.c
getorf_SOURCES = getorf.c
helixturnhelix_SOURCES = helixturnhelix.c
hmoment_SOURCES = hmoment.c
iep_SOURCES = iep.c
infoalign_SOURCES = infoalign.c
infobase_SOURCES = infobase.c
inforesidue_SOURCES = inforesidue.c
infoseq_SOURCES = infoseq.c
isochore_SOURCES = isochore.c
jaspextract_SOURCES = jaspextract.c
jaspscan_SOURCES = jaspscan.c
jembossctl_SOURCES = jembossctl.c
lindna_SOURCES = lindna.c
listor_SOURCES = listor.c
marscan_SOURCES = marscan.c
maskambignuc_SOURCES = maskambignuc.c
maskambigprot_SOURCES = maskambigprot.c
maskfeat_SOURCES = maskfeat.c
maskseq_SOURCES = maskseq.c
matcher_SOURCES = matcher.c
msbar_SOURCES = msbar.c
megamerger_SOURCES = megamerger.c
merger_SOURCES = merger.c
mwcontam_SOURCES = mwcontam.c
mwfilter_SOURCES = mwfilter.c
needle_SOURCES = needle.c
needleall_SOURCES = needleall.c
newcpgreport_SOURCES = newcpgreport.c
newcpgseek_SOURCES = newcpgseek.c
newseq_SOURCES = newseq.c
nohtml_SOURCES = nohtml.c
noreturn_SOURCES = noreturn.c
nospace_SOURCES = nospace.c
notab_SOURCES = notab.c
notseq_SOURCES = notseq.c
nthseq_SOURCES = nthseq.c
nthseqset_SOURCES = nthseqset.c
octanol_SOURCES = octanol.c
oddcomp_SOURCES = oddcomp.c
palindrome_SOURCES = palindrome.c
pasteseq_SOURCES = pasteseq.c
patmatdb_SOURCES = patmatdb.c
patmatmotifs_SOURCES = patmatmotifs.c
pepcoil_SOURCES = pepcoil.c
pepinfo_SOURCES = pepinfo.c
pepnet_SOURCES = pepnet.c
pepstats_SOURCES = pepstats.c
pepwheel_SOURCES = pepwheel.c
pepwindow_SOURCES = pepwindow.c
pepwindowall_SOURCES = pepwindowall.c
plotcon_SOURCES = plotcon.c
plotorf_SOURCES = plotorf.c
polydot_SOURCES = polydot.c
preg_SOURCES = preg.c
prettyplot_SOURCES = prettyplot.c
prettyseq_SOURCES = prettyseq.c
primersearch_SOURCES = primersearch.c
printsextract_SOURCES = printsextract.c
profit_SOURCES = profit.c
prophecy_SOURCES = prophecy.c
prophet_SOURCES = prophet.c
prosextract_SOURCES = prosextract.c
pscan_SOURCES = pscan.c
psiphi_SOURCES = psiphi.c
rebaseextract_SOURCES = rebaseextract.c
recoder_SOURCES = recoder.c
redata_SOURCES = redata.c
remap_SOURCES = remap.c
restover_SOURCES = restover.c
restrict_SOURCES = restrict.c
revseq_SOURCES = revseq.c
seealso_SOURCES = seealso.c
seqmatchall_SOURCES = seqmatchall.c
seqret_SOURCES = seqret.c
seqretsetall_SOURCES = seqretsetall.c
seqretsplit_SOURCES = seqretsplit.c
showalign_SOURCES = showalign.c
showdb_SOURCES = showdb.c
showfeat_SOURCES = showfeat.c
showorf_SOURCES = showorf.c
showpep_SOURCES = showpep.c
showseq_SOURCES = showseq.c
shuffleseq_SOURCES = shuffleseq.c
sigcleave_SOURCES = sigcleave.c
silent_SOURCES = silent.c
sirna_SOURCES = sirna.c
sixpack_SOURCES = sixpack.c
sizeseq_SOURCES = sizeseq.c
skipredundant_SOURCES = skipredundant.c
skipseq_SOURCES = skipseq.c
splitter_SOURCES = splitter.c
stretcher_SOURCES = stretcher.c
stssearch_SOURCES = stssearch.c
supermatcher_SOURCES = supermatcher.c
syco_SOURCES = syco.c
tcode_SOURCES = tcode.c
textsearch_SOURCES = textsearch.c
tfextract_SOURCES = tfextract.c
tfm_SOURCES = tfm.c
tfscan_SOURCES = tfscan.c
tmap_SOURCES = tmap.c
tranalign_SOURCES = tranalign.c
transeq_SOURCES = transeq.c
trimest_SOURCES = trimest.c
trimseq_SOURCES = trimseq.c
trimspace_SOURCES = trimspace.c
twofeat_SOURCES = twofeat.c
union_SOURCES = union.c
vectorstrip_SOURCES = vectorstrip.c
water_SOURCES = water.c
whichdb_SOURCES = whichdb.c
wobble_SOURCES = wobble.c
wordcount_SOURCES = wordcount.c
wordfinder_SOURCES = wordfinder.c
wordmatch_SOURCES = wordmatch.c
wossname_SOURCES = wossname.c
yank_SOURCES = yank.c


acdlog_SOURCES = acdlog.c
ajtest_SOURCES = ajtest.c
complex_SOURCES = complex.c
corbatest_SOURCES = corbatest.c
edamclean_SOURCES = edamclean.c
ensembltest_SOURCES = ensembltest.c
entrails_SOURCES = entrails.c
entrailsbook_SOURCES = entrailsbook.c
entrailshtml_SOURCES = entrailshtml.c
entrailswiki_SOURCES = entrailswiki.c
histogramtest_SOURCES = histogramtest.c
intconv_SOURCES = intconv.c
origsplitter_SOURCES = origsplitter.c
origunion_SOURCES = origunion.c
newcoils_SOURCES = newcoils.c
patmattest_SOURCES = patmattest.c
prima_SOURCES = prima.c
primers_SOURCES = primers.c
seqinfo_SOURCES = seqinfo.c
seqretall_SOURCES = seqretall.c
seqretallfeat_SOURCES = seqretallfeat.c
seqretset_SOURCES = seqretset.c
seqretsingle_SOURCES = seqretsingle.c
seqrettype_SOURCES = seqrettype.c
sqltest_SOURCES = sqltest.c
testplot_SOURCES = testplot.c
treetypedisplay_SOURCES = treetypedisplay.c

LDADD = ../nucleus/libnucleus.la ../ajax/acd/libacd.la \
	../ajax/ajaxdb/libajaxdb.la ../ajax/ensembl/libensembl.la \
	../ajax/graphics/libajaxg.la ../ajax/core/libajax.la \
	../ajax/zlib/libezlib.la ../ajax/expat/libeexpat.la \
	../ajax/pcre/libepcre.la ../plplot/libeplplot.la \
	$(XLIB)

pkgdata_DATA = emboss.default.template
pkgdatadir=$(prefix)/share/$(PACKAGE)

#if PURIFY
#  LINK = purify $(LIBTOOL) --mode=link $(CC) $(CFLAGS) $(LDFLAGS) -o $@
#else
#
#endif

