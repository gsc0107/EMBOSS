#ifndef ajjava_h
#define ajjava_h

#ifdef HAVE_JAVA

#include <jni.h>


#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jboolean JNICALL Java_org_emboss_jemboss_parser_Ajax_seqType 
  (JNIEnv *, jobject, jstring);

#ifdef __cplusplus
}
#endif
#endif
#endif
