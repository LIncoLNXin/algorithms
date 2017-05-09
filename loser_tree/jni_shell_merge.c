#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef DEBUG
#include <sys/time.h>
#endif
#include "jni_shell_merge.h"
#include "losertree.h"

typedef struct _CurScores
{
        int curidx;
        double* scores;
} CurScores;

typedef struct _Private
{
        JNIEnv* env;
        void* result;
        int curidx;
        int offset;
        int dimention;
        CurScores* curscores;
#ifdef DEBUG
        int tc;
        int tct;
        int tc1;
        int tc2;
        int tc3;
        int ts;
#endif
} Private;


static void GetDoc(JNIEnv* env, jobject doc, jstring* id, jdoubleArray* scores)
{
        if(NULL == doc) return;
        if(NULL != id)
        {
                jstring docid = (jstring)(*env)->GetObjectField(env
                                        , doc
                                        , (*env)->GetFieldID(env
                                                , (*env)->GetObjectClass(env, doc)
                                                , "docid"
                                                , "Ljava/lang/String;"
                                                )
                                        );
                *id = docid;
        }

        if(NULL != scores)
        {
                jdoubleArray docscores = (jdoubleArray)(*env)->GetObjectField(env
                                        , doc
                                        , (*env)->GetFieldID(env
                                                , (*env)->GetObjectClass(env, doc)
                                                , "docscores"
                                                , "[D"
                                                )
                                        );
                *scores = docscores;
        }

}

void SetResult(void* private, void* road, const int idx)
{
#ifdef DEBUG
        struct timeval tb, te;
        gettimeofday(&tb, NULL);
#endif
        Private* global = (Private*)private;
        JNIEnv* env = global->env;

        if(global->curidx++ < global->offset) return;

        jobject doc = (*env)->GetObjectArrayElement(env, (jobjectArray)road, idx);

        jstring id = NULL;
        GetDoc(env, doc, &id, NULL);
        
        if(NULL != id)
                (*env)->SetObjectArrayElement(env, (jobjectArray)global->result, global->curidx - global->offset - 1, id);
#ifdef DEBUG
        gettimeofday(&te, NULL);
        int total = (te.tv_sec - tb.tv_sec) * 1000000 + (te.tv_usec - tb.tv_usec);
        global->ts += total;
#endif
}

int CompareDoc(void* private, void* road1, const int roadk1, const int idx1, void* road2, const int roadk2, const int idx2)
{
#ifdef DEBUG
        struct timeval tb, te;
        gettimeofday(&tb, NULL);
        int total = 0;
#endif
        Private* global = (Private*)private;
        JNIEnv* env = global->env;

        double* s1 = NULL, *s2 = NULL;
        double* ss1 = NULL, *ss2 = NULL;
        jdoubleArray scores1 = NULL;
        jdoubleArray scores2 = NULL;

        if(global->curscores[roadk1].curidx != idx1)
        {
                GetDoc(env, (*env)->GetObjectArrayElement(env, (jobjectArray)road1, idx1), NULL, &scores1);

#ifdef DEBUG
                gettimeofday(&te, NULL);
                total = (te.tv_sec - tb.tv_sec) * 1000000 + (te.tv_usec - tb.tv_usec);
                global->tc1 += total;
#endif

                if(NULL != scores1)
                {
                        ss1 = (*env)->GetDoubleArrayElements(env, scores1, NULL);

#ifdef DEBUG
                gettimeofday(&te, NULL);
                total = (te.tv_sec - tb.tv_sec) * 1000000 + (te.tv_usec - tb.tv_usec);
                global->tc2 += total;
#endif
                        if(NULL != ss1)
                        {
                                //memmove(global->curscores[roadk1].scores, s1, sizeof(double) * global->dimention);
                                memcpy(global->curscores[roadk1].scores, ss1, sizeof(double) * (*env)->GetArrayLength(env, scores1));
                                s1 = global->curscores[roadk1].scores;
                        }
                }
                global->curscores[roadk1].curidx = idx1;

        }
        else
        {
                s1 = global->curscores[roadk1].scores;
        }

        if(global->curscores[roadk2].curidx != idx2)
        {
                GetDoc(env, (*env)->GetObjectArrayElement(env, (jobjectArray)road2, idx2), NULL, &scores2);

#ifdef DEBUG
                gettimeofday(&te, NULL);
                total = (te.tv_sec - tb.tv_sec) * 1000000 + (te.tv_usec - tb.tv_usec);
                global->tc1 += total;
#endif

                if(NULL != scores2)
                {
                        ss2 = (*env)->GetDoubleArrayElements(env, scores2, NULL);

#ifdef DEBUG
                gettimeofday(&te, NULL);
                total = (te.tv_sec - tb.tv_sec) * 1000000 + (te.tv_usec - tb.tv_usec);
                global->tc2 += total;
#endif
                        if(NULL ！= ss2)
                        {
                                //memmove(global->curscores[roadk2].scores, s2, sizeof(double) * global->dimention);
                                memcpy(global->curscores[roadk2].scores, ss2, sizeof(double) * (*env)->GetArrayLength(env, scores2));
                                s2 = global->curscores[roadk2].scores;
                        }
                }
                global->curscores[roadk2].curidx = idx2;

        }
        else
        {
                s2 = global->curscores[roadk2].scores;
        }

        if(NULL == s1) goto R_FALSE;
        if(NULL == s2) goto R_TRUE;

        int i;
        for(i = 0; i < global->dimention; ++i)
        {
                if(s1[i] > s2[i]) goto R_TRUE; 
                else if(s1[i] < s2[i]) goto R_FALSE;

                continue;
        }
#ifdef DEBUG
        gettimeofday(&te, NULL);
        total = (te.tv_sec - tb.tv_sec) * 1000000 + (te.tv_usec - tb.tv_usec);
        global->tc3 += total;
#endif

R_FALSE:
        if(NULL != scores1)
                (*env)->ReleaseDoubleArrayElements(env, scores1, ss1, 0);
        if(NULL != scores2)
                (*env)->ReleaseDoubleArrayElements(env, scores2, ss2, 0);
#ifdef DEBUG
        gettimeofday(&te, NULL);
        total = (te.tv_sec - tb.tv_sec) * 1000000 + (te.tv_usec - tb.tv_usec);
        global->tc += total;
        global->tct += 1;
#endif
        return 0;

R_TRUE:
        if(NULL != scores1)
                (*env)->ReleaseDoubleArrayElements(env, scores1, ss1, 0);
        if(NULL != scores2)
                (*env)->ReleaseDoubleArrayElements(env, scores2, ss2, 0);
#ifdef DEBUG
        gettimeofday(&te, NULL);
        total = (te.tv_sec - tb.tv_sec) * 1000000 + (te.tv_usec - tb.tv_usec);
        global->tc += total;
        global->tct += 1;
#endif
        return 1;
}



JNIEXPORT jobjectArray JNICALL Java_org_apache_solr_handler_component_jni_JniMerge_Merge(JNIEnv* env, jobject arg, jobjectArray shardDocs, jint offset, jint count, jint dimention)
{
#ifdef DEBUG
        struct timeval tb, te;
        gettimeofday(&tb, NULL);
#endif

        jsize shardlen = (*env)->GetArrayLength(env, shardDocs);
        if(0 == shardlen) return (*env)->NewObjectArray(env,  0,  (*env)->FindClass(env, "Ljava/lang/String;"), NULL);

        LoserTree* t = LoserTree_Create(shardlen, dimention);           // need destroy
        if(NULL == t) return (*env)->NewObjectArray(env,  0,  (*env)->FindClass(env, "Ljava/lang/String;"), NULL);
        LoserTree_SetCallBack(t, SetResult, CompareDoc);

        int i;
        jsize docslen, totallen = 0, resultlen = 0;
        for(i = 0; i < shardlen; ++i)
        {
                jobjectArray docs = (*env)->GetObjectArrayElement(env, shardDocs, i);   // no need release
                if(NULL != docs)
                {
                        docslen = (*env)->GetArrayLength(env, docs);
                        totallen += docslen;

                        LoserTree_SetRoad(t, docs, i, (int)docslen);
                }
        }

        if(totallen >= (offset + count))
        {
                resultlen = count;
                totallen = offset + count;
        }
        else
        {
                resultlen = totallen > offset ? totallen - offset : 0;
                if(resultlen == 0)
                {
                        LoserTree_Destroy(t);
                        return (*env)->NewObjectArray(env,  0,  (*env)->FindClass(env, "Ljava/lang/String;"), NULL);
                }
        }
        LoserTree_SetResultCount(t, (int)totallen);

        Private private;
        private.env = env;
        private.curidx = 0;
        private.offset = (int)offset;
        private.result = (*env)->NewObjectArray(env,  resultlen,  (*env)->FindClass(env, "Ljava/lang/String;"), NULL);
        private.dimention = dimention;

        // cache cur scores
        private.curscores = (CurScores*)malloc(sizeof(double) * shardlen * dimention + sizeof(CurScores) * shardlen);
        if(private.curscores) 
        ｛
                memset(private.curscores, 0, sizeof(double) * shardlen * dimention + sizeof(CurScores) * shardlen);
        ｝
        else
        {
                LoserTree_Destroy(t);
                return (*env)->NewObjectArray(env,  0,  (*env)->FindClass(env, "Ljava/lang/String;"), NULL);
        }

        int j;
        for(j = 0; j < shardlen; ++j)
        {
                private.curscores[j].curidx = -1;
                private.curscores[j].scores = (double*)(private.curscores + shardlen) + j * dimention;
        }


#ifdef DEBUG
        private.tc = 0;
        private.tct = 0;
        private.tc1 = 0;
        private.tc2 = 0;
        private.tc3 = 0;
        private.ts = 0;
#endif

        LoserTree_SetPrivate(t, &private);

        LoserTree_Merge(t);
        LoserTree_Destroy(t);

        if(private.curscores) free(private.curscores);

#ifdef DEBUG
        gettimeofday(&te, NULL);
        int total = (te.tv_sec - tb.tv_sec) * 1000000 + (te.tv_usec - tb.tv_usec);
        printf("total time use: %d \t%d\t%d\t%d [us]\n", total, private.tc, private.ts, total - private.tc - private.ts);
        printf("tc: %d %d %d %d\n", private.tct, private.tc1, private.tc2, private.tc3);
#endif

        return private.result;
}

JNIEXPORT jobjectArray JNICALL Java_JniMerge_Merge(JNIEnv* env, jobject arg, jobjectArray shardDocs, jint offset, jint count, jint dimention)
{
        return Java_org_apache_solr_handler_component_jni_JniMerge_Merge(env, arg, shardDocs, offset, count, dimention);
}
