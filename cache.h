#ifndef _cache_h_
#define _cache_h_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>  

/* structure definitions of line*/  
typedef struct cacheLine {
    unsigned tag;
    int dirty;
    struct cacheLine *next;
    struct cacheLine *prev;
} line;

/* structure definition of set */
typedef struct cacheSet {
    int setValids;      /* number of valid entries in set */
    line* head;     /* head of set, the most recently used */
    line* tail;     /* tail of set, the least recently used */
}set;

/* structure definition of cache */
typedef struct cache_ {
    int C;		/* capacity */
    int E;      /* associativity */
    int B;      /* block size (bytes) */
    int S;		/* number of sets */
    int t;      /* number of tag bits */
    int s;      /* number of set index bits */
    int b;      /* number of block offset bits */
    int indexMaskOffset;
    unsigned indexMask;
    int tagMaskOffset;
    unsigned tagMask;
    char* writePolicy;      /* cache write policy */
    set *sets;      /* array of sets */
} cache;

/* statistics for the trace */
typedef struct cacheStat {
    int memoryReads;			/* number of memory read */
    int memoryWrites;			/* number of memory write */
    int cacheHits;		/* number of cache hit */
    int cacheMisses;		/* number of cache miss */
} cacheStat;

/* set parameters according to the arguments of main function */
void setParams(int, int, int, char*);
/* initiate cache */
void initCache();
/* simulate the access to cache */
void access(unsigned, char);
/* insert line in set */
void insert(set *, line*);
/*delete line in set */
void delete(set *, line*);
/* print out statistics of the trace */
void printStat();
/* compute log2(x) */
int Log2(int x);
#endif
