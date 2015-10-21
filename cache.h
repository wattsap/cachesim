#include <stdbool.h>

/******************************************************************************
 * cache.h                                                                    *  
 *                                                                            *
 * Written by Benjamin Crawford and Andrew Watts                              *
 * Date October 21, 2015                                                      *
 * Version 1.1                                                                *
 ******************************************************************************/
#ifndef CACHE_H_
#define CACHE_H_

/******************************************************************************
 * Struct Declarations                                                        *
 ******************************************************************************/

//each line consists of a valid bit and
//a tag (note we aren't keeping track of
//the data in the cache)
typedef struct 
{

int valid;
int tag;
} lineType;

// A cache is an array of these (sets)
typedef struct 
{

//Each set is an array of lines and an
//array of line indices (lruQueue) used for
//replacement
lineType * lines; 
int * lruQueue;
} setType;

/******************************************************************************
 * function Declarations                                                      *
 ******************************************************************************/

void printResults(long accesses, long hits, long misses);

void printCache(setType * cache, int NoLines, int NoSets, int address, int tag, int index, int offset, int hit);

setType * initCache(int NoSets, int NoLines);

void evaluateArgs(int argc, char * argv[]);

bool isValidFileType(char * fName);

void parseFile();

void enqueue(int *lruQueue, int item);

void dequeue(int *lru_queue, int item);

int isPowerOfTwo(unsigned int n);

unsigned long int getBlockOffset(unsigned long int rawAddressLine);

unsigned long int getSetIndex(unsigned long int rawAddressLine);

unsigned long int getTag(unsigned long int rawAddressLine);

#endif
