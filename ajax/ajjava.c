#ifdef HAVE_JAVA

#include <jni.h>
#include "ajax.h"

#define AJNOTFOUND -999

static AjBool ajJavaGetSeqFromUsa (AjPStr thys, AjPSeq *seq);


JNIEXPORT jboolean JNICALL Java_org_emboss_jemboss_parser_Ajax_seqType
           (JNIEnv *env, jobject obj, jstring usa)
{
    AjPStr name=NULL;
    char *javaname=NULL;
    AjBool ok=ajFalse;
    AjPSeq seq=NULL;
    AjBool nuc=ajFalse;
    ajint  len=0;
    float  weight=0.;
    jclass jvc = (*env)->GetObjectClass(env,obj);
    jfieldID field;
    
    name = ajStrNew();
    seq  = ajSeqNew();
    
    javaname = (char *) (*env)->GetStringUTFChars(env,usa,0);
    ajStrAssC(&name,javaname);
    (*env)->ReleaseStringUTFChars(env,usa,javaname);

    ok = ajJavaGetSeqFromUsa(name,&seq);
    if(ok)
    {
	len = ajSeqLen(seq);
	ajSeqType(seq);	
	nuc = ajSeqIsNuc(seq);
	weight = seq->Weight;
	
	field = (*env)->GetStaticFieldID(env,jvc,"length","I");
	(*env)->SetStaticIntField(env,jvc,field,len);

	field = (*env)->GetStaticFieldID(env,jvc,"protein","Z");
	(*env)->SetStaticBooleanField(env,jvc,field,(unsigned char)!nuc);

	field = (*env)->GetStaticFieldID(env,jvc,"weight","F");
	(*env)->SetStaticFloatField(env,jvc,field,weight);
    }
    
    ajStrDel(&name);
    ajSeqDel(&seq);

    return (unsigned char) ok;
}





static AjBool ajJavaGetSeqFromUsa (AjPStr thys, AjPSeq *seq)
{
    AjPSeqin seqin;
    AjBool ok;
  
    ajNamInit("emboss");
  
    seqin = ajSeqinNew();
    seqin->multi = ajFalse;
    seqin->Text  = ajFalse;
  
    ajSeqinUsa (&seqin, thys);
    ok = ajSeqRead(*seq, seqin);
    ajSeqinDel (&seqin);

    if(!ok)
	return ajFalse;

    return ajTrue;
}


#endif
