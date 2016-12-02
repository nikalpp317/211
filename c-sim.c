#include "cache.h"  
#include "c-sim.h"
#include <stdbool.h>  


static FILE *traceFile;

int main(int argc, char* argv[])
{
    parseArgs(argc, argv);
    initCache();
    trace();
    printStat();
    fclose(traceFile);
    return 0;
}

void parseArgs(int argc, char** argv)
{
    if (argc < 2) {
        printf("usage:  c-sim [-h] <arguments> <trace file>\n");
        exit(-1);
    }
    
    /* print information about help. */
int i=0;
    for (i = 0; i < argc; i++)
        if (!strcmp(argv[i], "-h")) {
            printf("\tusage: c-sim [-h] <cache size> <associativity> <block size> <write policy> <trace file>\n");
            printf("\tcache size should be a power of 2\n\n");
            printf("\tassociativity should be one of direct(direct mapped cache), assoc(fully associative cache), assoc:n(n−way associative cache and n should be a power of 2)\n");
            printf("\tblock size should be a power of 2\n\n");
            printf("\twrite policy should be one of wt(write through cache), wb(write back cache)\n");
            exit(0);
        }
    
    int C, E, B;
    C = atoi(argv[1]);
    B = atoi(argv[3]);
    
    /* print the informative error message when the size of cache is invalid. */
    if (!powerOf2(C)) {
        printf("Error: the size of cache is invalid. Check the usage by entering -h.\n");
        exit(0);
    }
    /* get the setSize and if the input is invalid, print the informative error message. */
    if (!strcmp(argv[2], "direct"))
        E = 1;
    else if (!strcmp(argv[2], "assoc"))
        E = C / B;
    else if (!strncmp(argv[2], "assoc:", 6))
        E = atoi(strchr(argv[2], ':')+1);
    else {
        printf("Error: the associativity type of cache is invalid. Check the usage by entering -h.\n");
        exit(0);
    }
    if (!powerOf2(E)) {
        printf("Error: the size of set is invalid. Check the usage by entering -h.\n");
        exit(0);
    }
    /* print the informative error message when the size of block is invalid. */
    if (!powerOf2(B)) {
        printf("Error: the size of block is invalid. Check the usage by entering -h.\n");
        exit(0);
    }
    
    /* print the informative error message when the write policy is invalid. */
    if (strcmp(argv[4], "wt") && strcmp(argv[4], "wb")) {
        printf("Error: the write policy is invalid. Check the usage by entering -h.\n");
        exit(0);
    }
    
    /* print the informative error message when the path of trace file is invalid. */
    traceFile = fopen(argv[5], "r");
    if (traceFile == NULL) {
        printf("Error: the trace file doesn't exist. Enter again.");
        exit(0);
    }
    
    setParams(C, E, B, argv[4]);
    return;
}

void trace()
{
    char accessType;
    unsigned ip, memoryAddr;
    while (fscanf(traceFile, "%x: %c %x\n", &ip, &accessType, &memoryAddr) == 3) {
        switch (accessType) {
            case 'W':
            case 'R':
                access(memoryAddr, accessType);
                break;
            default:
                break;
        }
    }
}

bool powerOf2(int n)
{
    if (n==0)
        return false;
    if (n==1)
        return true; 
    else {
        do {
            if (n % 2 == 0) n = n / 2;
            else return false;
        }
        while (n != 1);
        return true;
    }
}