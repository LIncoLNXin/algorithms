#ifndef __LOSERTREE_H__
#define __LOSERTREE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*SetResultFunc)(void*, void*, const int);                         // set result (private, road, idx)

typedef int (*CompareDocFunc)(void*, void*, const int, const int, void*, const int, const int); // compare doc (private, road, roadidx, offset, road, roadidx, offset)

typedef struct _LoserTree
{
        int*                    lr;             // relation
        int*                    curidx;         // curidx - road offset
        int*                    roadlen;        // roadlen
        void**                  kroads;         // roads array 

        int                     road;           // k-road
        int                     dimention;      // dimention
        int                     rcount;         // result count
        void*                   private;        // private data

        SetResultFunc           SetResult;
        CompareDocFunc          CompareDoc;
} LoserTree;

LoserTree* LoserTree_Create(const int k, const int dimention);

void LoserTree_SetCallBack(LoserTree* t, SetResultFunc SetResult, CompareDocFunc CompareDoc);

void LoserTree_SetRoad(LoserTree* t, void* road, const int roadidx, const int len);

void LoserTree_SetPrivate(LoserTree* t, void* private);

void LoserTree_Destroy(LoserTree* t);

int LoserTree_Merge(LoserTree* t);

void LoserTree_SetResultCount(LoserTree* t, const int rcount);

#ifdef __cplusplus
}
#endif

#endif
