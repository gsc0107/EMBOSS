#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajnam_h
#define ajnam_h

AjBool ajNamDatabase(AjPStr name);
AjBool ajNamDbAccess (AjPStr dbname, AjPSeqQuery qry, AjPStr* method);
AjBool ajNamDbDetails (AjPStr name, AjPStr* type, AjBool* id,
	       AjBool* qry, AjBool* all, AjPStr* comment, AjPStr* release);
AjBool ajNamDbGetAlias (AjPStr dbname, AjPStr* dbalias);
AjBool ajNamDbGetDbalias (AjPStr dbname, AjPStr* dbalias);
AjBool ajNamDbGetUrl (AjPStr dbname, AjPStr* url);
AjBool ajNamDbData (AjPSeqQuery qry);
AjBool ajNamDbQuery (AjPSeqQuery qry);
AjBool ajNamDbTest (AjPStr dbname);
void   ajNamDebugOrigin (void);
void   ajNamDebugDatabases (void);
void   ajNamDebugEnvironmentals (void);
void   ajNamEnvironmentals(void); /* "" "" may want to delete later */
void   ajNamExit (void);
AjBool ajNamGetenv (AjPStr name, AjPStr* value);
AjBool ajNamGetValue (AjPStr name, AjPStr* value);
AjBool ajNamGetValueC (char *name, AjPStr* value);
void   ajNamInit(char* prefix);
void   ajNamListDatabases(void); /* test routine */
void   ajNamListListDatabases (AjPList dbnames);
void   ajNamListOrigin (void);
void   ajNamPrintDbAttr (AjPFile outf, AjBool full);
AjBool ajNamResolve (AjPStr* name);
AjBool ajNamRoot (AjPStr* root);
AjBool ajNamRootBase (AjPStr* rootbase);
AjBool ajNamRootInstall (AjPStr* root);
AjBool ajNamRootPack (AjPStr* pack);
AjBool ajNamRootVersion (AjPStr* version);
#endif

#ifdef __cplusplus
}
#endif

