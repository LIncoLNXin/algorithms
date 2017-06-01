#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "losertree.h"

void LoserTree_SetCallBack(LoserTree* t, SetResultFunc SetResult, CompareDocFunc CompareDoc)
{
        t->SetResult = SetResult;
        t->CompareDoc = CompareDoc;
}

LoserTree* LoserTree_Create(const int k, const int dimention)
{
        int total_size = 0;
        total_size = sizeof(LoserTree) 
                        + sizeof(int) * k
                        + sizeof(int) * k
                        + sizeof(int) * k
                        + sizeof(void*) * k
                        ;

        LoserTree* t = (LoserTree*)malloc(total_size);
        if(NULL == t) return NULL;
        memset(t, 0, total_size);

        t->road = k;
        t->dimention = dimention;

        t->lr = (int*)(t + 1); 
        t->curidx = t->lr + k; 
        t->roadlen = t->curidx + k; 
        t->kroads = (void**)(t->roadlen + k);

        return t;
}

void LoserTree_Destroy(LoserTree* t)
{
        if(NULL != t)
                free(t);
}


void LoserTree_SetRoad(LoserTree* t, void* road, const int roadidx, const int len)
{
        t->kroads[roadidx] = road;
        t->roadlen[roadidx] = len;
}

static void adjustLoserTreeFirst(LoserTree* t, int road, int maxroad)
{
        int parent = (road + t->road) >> 1;
        int tmp = 0;
        void* roadp = NULL, *roadcur = NULL;
        while(parent > 0)
        {
                roadp = t->kroads[t->lr[parent]];
                roadcur = t->kroads[road];

                if(road != maxroad && (t->lr[parent] == maxroad || t->curidx[road] >= t->roadlen[road]
                        || (t->curidx[t->lr[parent]] < t->roadlen[t->lr[parent]]
                                && t->CompareDoc(t->private, roadp, t->lr[parent], t->curidx[t->lr[parent]], roadcur, road, t->curidx[road])
                                )
                        ))
                {
                        tmp = t->lr[parent];
                        t->lr[parent] = road;
                        road = tmp;
                }
                parent >>= 1;
        }

        t->lr[0] = road;
}
static void adjustLoserTree(LoserTree* t, int road)
{
        int parent = (road + t->road) >> 1;
        int tmp = 0;
        void* roadp = NULL, *roadcur = NULL;
        while(parent > 0)
        {
                roadp = t->kroads[t->lr[parent]];
                roadcur = t->kroads[road];

                if(t->curidx[road] >= t->roadlen[road]
                        || (t->curidx[t->lr[parent]] < t->roadlen[t->lr[parent]]
                                && t->CompareDoc(t->private, roadp, t->lr[parent], t->curidx[t->lr[parent]], roadcur, road, t->curidx[road])
                                )
                        )
                {
                        tmp = t->lr[parent];
                        t->lr[parent] = road;
                        road = tmp;
                }
                parent >>= 1;
        }

        t->lr[0] = road;
}

static void initLoserTree(LoserTree* t)
{
        int i, winner = 0;
        void* roadw = NULL;
        void* road = NULL;
        for(i = 1; i < t->road; ++i)
        {
                road = t->kroads[i];
                roadw = t->kroads[winner];
                if(t->CompareDoc(t->private, road, i, 0, roadw, winner, 0))
                        winner = i;
        }

        for(i = 0; i < t->road; ++i)
                t->lr[i] = winner;


        for(i = t->road - 1; i >= 0; --i)
        {
                adjustLoserTreeFirst(t, i, winner);
        }
}

int LoserTree_Merge(LoserTree* t)
{
        initLoserTree(t);

        int i = 0;
        void* road = NULL;

        while(i++ < t->rcount)
        {
                if(t->curidx[t->lr[0]] >= t->roadlen[t->lr[0]])                         // no data then stop
                        break;

                road = t->kroads[t->lr[0]];
                t->SetResult(t->private, road, t->curidx[t->lr[0]]);                    // return cur max id

                ++ t->curidx[t->lr[0]];                                                 // next doc

                adjustLoserTree(t, t->lr[0]);                                           // adjust next
        }

        return i;
}

void LoserTree_SetPrivate(LoserTree* t, void* private)
{
        t->private = private;
}

void LoserTree_SetResultCount(LoserTree* t, const int rcount)
{
        t->rcount = rcount;
}
