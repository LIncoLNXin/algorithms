#ifndef __JNI_SHELL_MERGE_H__
#define __JNI_SHELL_MERGE_H__

#include "jni.h"

#ifdef __cplusplus
extern "C" {
#endif


	JNIEXPORT jobjectArray JNICALL Java_org_apache_solr_handler_component_jni_JniMerge_Merge(JNIEnv* env, jobject arg, jobjectArray shardDocs, jint offset, jint count, jint dimention);


	JNIEXPORT jobjectArray JNICALL Java_JniMerge_Merge(JNIEnv* env, jobject arg, jobjectArray shardDocs, jint offset, jint count, jint dimention);

#ifdef __cpluscplus
}
#endif

#endif
