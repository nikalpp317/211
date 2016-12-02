#include "cache.h"

static cache Cache;
static cacheStat Stat;      

void setParams(int C, int E, int B, char* writePolicy)   
{
    Cache.C = C;
    Cache.E = E;
    Cache.B = B;
    Cache.S = C / E / B;
    Cache.writePolicy = writePolicy;
    Cache.s = Log2(Cache.S);
    Cache.b = Log2(B);
    Cache.t = 32 - Cache.s - Cache.b;
    Cache.tagMask = 0xFFFFFFFF >> (Cache.s + Cache.b);
    Cache.tagMaskOffset = Cache.s + Cache.b;
    Cache.indexMask = 0xFFFFFFFF >> (Cache.t + Cache.b);
    if (Cache.S == 1) Cache.indexMask = 0;      /* to fully associativity cache, the indexMask should be 0. the last line do not realize this because (a >> 32)this is not defined in C/C++. */
    Cache.indexMaskOffset = Cache.b;
}

void initCache()
{
    Cache.sets = (set *)malloc(sizeof(set)*Cache.S);    /* allocate space for sets */
    /* set initial values */
	int i=0;
    for ( i=0; i!=Cache.S; ++i) {
        Cache.sets[i].head = NULL;
        Cache.sets[i].tail = NULL;
        Cache.sets[i].setValids = 0;
    }
    Stat.cacheHits = 0;
    Stat.cacheMisses = 0;
    Stat.memoryReads = 0;
    Stat.memoryWrites = 0;
}

void access(unsigned memoryAddr, char accessType)
{
    int tag = (memoryAddr >> Cache.tagMaskOffset) & Cache.tagMask;      /* compute tag */
    int index = (memoryAddr >> Cache.indexMaskOffset) & Cache.indexMask;        /* compute index */
    /* read */
    if (accessType == 'R') {
        line* item = (line *)malloc(sizeof(line));
        if (Cache.sets[index].setValids == 0) {     /* cold miss */
            Stat.cacheMisses++;
            Stat.memoryReads++;
            item->tag = tag;
            item->dirty = 0;
            insert(&Cache.sets[index], item);       /* load block */
        }
        else {
            line *itr = Cache.sets[index].head;
            //traverse the whole set to find the target
            while (itr != NULL) {
                if (itr->tag == tag)
                    break;
                itr = itr->next;
            }
            /* if not find, it is a miss */
            if (itr == NULL) {
                Stat.cacheMisses++;
                Stat.memoryReads++;
                if (Cache.sets[index].setValids==Cache.E) {    /* capacity miss */
                    if (!strcmp(Cache.writePolicy, "wb") && Cache.sets[index].tail->dirty)  /* wb && modified, need to write back to memory */
                        Stat.memoryWrites++;
                    delete(&Cache.sets[index], Cache.sets[index].tail);     /* delete the least recently used block */
                }
                item->tag = tag;
                item->dirty = 0;
                insert(&Cache.sets[index], item);    /* load block */
            }
            /* if find, it is a hit */
            else {
                Stat.cacheHits++;
                item->tag = itr->tag;
                item->dirty = itr->dirty;
                insert(&Cache.sets[index], item);    /* insert to the head (because it is referenced, change its position to the head of this list) */
                delete(&Cache.sets[index], itr);    /* delete original hit block */
            }
        }
    }
    
    /* write */
    else {
        if (!strcmp(Cache.writePolicy, "wt"))       /* immediately write to memory */
            Stat.memoryWrites++;
        line* item = (line *)malloc(sizeof(line));
        if (Cache.sets[index].setValids == 0) {     /* cold miss */
            Stat.cacheMisses++;
            Stat.memoryReads++;
            item->tag = tag;
            item->dirty = 1;
            insert(&Cache.sets[index], item);       /* load block */
        }
        else {
            line *itr = Cache.sets[index].head;
            //traverse the whole set to find the target
            while (itr != NULL) {
                if (itr->tag == tag)
                    break;
                itr = itr->next;
            }
            /* if not find, it is a miss */
            if (itr == NULL) {
                Stat.cacheMisses++;
                Stat.memoryReads++;
                if (Cache.sets[index].setValids==Cache.E) {   /* capacity miss */
                    if (!strcmp(Cache.writePolicy, "wb") && Cache.sets[index].tail->dirty)  /* wb && modified, need to write back to memory */
                        Stat.memoryWrites++;
                    delete(&Cache.sets[index], Cache.sets[index].tail);
                }
                item->tag = tag;
                item->dirty = 1;        /* modified */
                insert(&Cache.sets[index], item);       /* load block */
            }
            /* if find, it is a hit */
            else {
                Stat.cacheHits++;
                item->tag = itr->tag;
                item->dirty = 1;        /* modified */
                insert(&Cache.sets[index], item);    /* modified, change it position to the head of this list */
                delete(&Cache.sets[index], itr);    /* delete the line at original position */
            }
        }
    }
}

void insert(set *s, line* line)
{
    s->setValids++;
    line->next = s->head;
    line->prev = NULL;
    if (line->next != NULL) {
        line->next->prev = line;
    }
    else
        s->tail = line;
    s->head = line;
}

void delete(set *s, line* line)
{
    s->setValids--;
    if (line->prev) {
        line->prev->next = line->next;
    } else {
        s->head = line->next;
    }
    if (line->next) {
        line->next->prev = line->prev;
    } else {
        s->tail = line->prev;
    }
    free(line);
}

void printStat()
{
    printf("Memory reads: %d\n", Stat.memoryReads);
    printf("Memory writes: %d\n", Stat.memoryWrites);
    printf("Cache hits: %d\n", Stat.cacheHits); 
    printf("Cache misses: %d\n", Stat.cacheMisses);
}

int Log2(int x)
{
    int y = -1;
    while (x > 0) {
        x >>= 1;
        ++y;
    }
    return y;
}