/******************************************************************************
 * cache.c                                                                    *  
 *                                                                            *
 * Written by Benjamin Crawford and Andrew Watts                              *
 * Date October 21, 2015                                                      *
 * Version 1.1                                                                *
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
//needed for log2(double) function
#include <math.h>
#include "cache.h"

#define BUFF_SIZE 256

int B,E,F,S;
char * fileName;
FILE * fp;
setType * cache;
bool debugMode;

/******************************************************************************
 * Instructor provided code                                                   *
 ******************************************************************************/

/*  
 *  Name: printResults
 *  Description: This function prints the results of the cache simulation.
 *  Parameters:
 *      accesses - total number of memory accesses
 *      hits - total number of cache hits
 *      misses - total number of cache misses
 *
*/
void printResults(long accesses, long hits, long misses)
{
     printf("Accesses: %d, Hits: %d, Misses: %d, Miss Rate: %.4f\n",
             accesses, hits, misses, ((float)misses/(float)accesses));
}

/*  
 *  Name: printCache
 *  Description: This function prints out the most recent access to memory
 *               and the state of the cache after the most recent access
 *  Parameters:
 *      cache - any array of setType 
 *      NoLines - the number of lines in each set
 *      NoSets - the number of sets in the cache
 *      address - the address of the most recent access
 *      tag - the tag portion of the address
 *      index - the index portion of the address
 *      offset - the offset portion of the address
 *      hit - 1 if the most recent access result in a hit, 0 otherwise
 */
void printCache(setType * cache, int NoLines, int NoSets, int address, int tag, int index, int offset, int hit)
{
    int i, j, empty;
    printf(" Address: %8x, Tag: %x, Index: %x, Offset: %x", address, tag,
             index, offset);
    if (hit)
        printf(" (hit)\n");
    else
        printf(" (miss)\n");
    printf("     Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..\n");
    for (i = 0; i < NoSets; i++)
    {
        empty = 1;
        for (j = 0; j < NoLines; j++)
        {
            if (cache[i].lines[j].valid) empty = 0;
        }
        //don't print an empty set; go back to top of loop
        if (empty) continue;
        printf("%8x: ", i);
        for (j = 0; j < NoLines; j++)
        {
            printf("(%x, %d) ", cache[i].lines[j].tag, 
                                    cache[i].lines[j].valid);
        }
        printf("LRUs: ");
        for (j = 0; j < NoLines; j++)
            printf("%d, ", cache[i].lruQueue[j]);

        printf("\n");
   }
   printf("\n");
}


/*  
 *  Name: initCache
 *  Description: This function creates the structure that represents the cache and
 *               returns a pointer to that structure.  The cache is an array of
 *               setTypes.  Each setType is array of lines and an array of
 *               line indices.  The array of line indices is used for implementing
 *               lru replacement.  Each line contains a valid bit and a tag.
 *
 * Parameters:
 *      NoSets - number of sets in the cace
 *      NoLines - number of lines in each set
 * Returns:
 *      pointer to the cache structure
 */
setType * initCache(int NoSets, int NoLines)
{
    int i, j;
    setType * cache;
    //create the NoSets sets
    cache = malloc(sizeof(setType) * NoSets);
    for (i = 0; i < NoSets; i++)
    {
        //create the NoLines lines and the array of line indices
        cache[i].lines = malloc(sizeof(lineType) * NoLines);
        cache[i].lruQueue = malloc(sizeof(int) * NoLines);
    }
    //initialize all valid bits to 0 and the lruQueue entries to -1
    for (i = 0; i < NoSets; i++)
        for (j = 0; j < NoLines; j++)
        {
            cache[i].lines[j].valid = 0;
            cache[i].lruQueue[j] = -1;
        }
    return cache;
}

/******************************************************************************
 * Student Code                                                                    *
 ******************************************************************************/

/*
 * Name: int main
 * Description: Calls functions for evaluating args, building the cache,
 * and parsing the file.  Also handles the check to see if the right usage
 * was followed in calling the program executable:
 * cacheSim -S s -E e -B b <filename>
 * s = number of sets
 * e = number of lines per set
 * b = number of bytes per block
 * filename
 * optional -D for debug mode
 * ##FOR THIS EXERCISE S,E,B MUST BE POWERS OF TWO##
 * uses commandline args to access these values
 * @params: command line arguments
 * @returns: technically a 0, but nothing of relevance.
 */
int main(int argc, char * argv[]){
    if(argc > 1)
	{
		debugMode = false;
        evaluateArgs(argc, argv);
		cache = initCache(S, E);
		parseFile(cache);
    }
    else{
        printf("usage: cacheSim -S s -E e -B b -F <filename>\n");
		printf("s - number of Sets\n");
		printf("e - number of Lines per Set\n");
		printf("b - number of Bytes per Block\n");
		printf("<filename> - name of file containing address trace\n");
		printf("s, e, b must be powers of two\n");
		exit(0);
    }   
    return 0;
}

/* 
 * Name: void evaluateArgs
 * Description: uses command line args to evaluate
 * the specifications used when calling the executable C file.
 * Checks to be sure each required part of usage is present,
 * prints error messages if not. If it is present, the usage 
 * spec is saved globally for further use.
 * @params: command line arguments like those required in 
 * main methods.  
 * @returns: none
 */
void evaluateArgs(int argc, char * argv[])
{
    char * pc;
	bool hasB, hasE, hasF, hasS;
	for(int i = 1; i < argc; i++)
	{
		if(strcmp(argv[i],"-B") == 0)
		{
			i++;
			if(i < argc)
			{
				int tmp = strtol(argv[i], &pc, 10);
				if(tmp <= 0)
				{
					printf("Error: invalid value for -B\n");
					exit(0);
				}
				else{
					if(isPowerOfTwo(tmp))
					{
						B = tmp;
						hasB = true;
					}
					else
					{
						printf("Error: B must be a power of 2");
						exit(0);
					}
				}
			}
			else{
				printf("Error: invalid value for -B\n");
				exit(0);
			}
		}
		else if(strcmp(argv[i],"-E") == 0)
		{
			i++;
			if(i < argc)
			{
				int tmp = strtol(argv[i], &pc, 10);
				if(tmp <= 0)
				{
					printf("Error: invalid value for -E\n");
					exit(0);
				}
				else
				{
					E = tmp;
					hasE = true;
				}
			}
			else
			{
				printf("Error: invalid value for -E\n");
				exit(0);
			}
		}
		else if(strcmp(argv[i],"-F") == 0)
		{
			i++;
			if(i < argc)
			{
				if(isValidFileType(argv[i]))
				{
					fileName = argv[i];
					hasF = true;
				}
				else
				{
					printf("Error: invalid file type given");
					exit(0);
				}
			}
			else{
				printf("Error: invalid value for -F\n");
				exit(0);
			}
		}
		else if(strcmp(argv[i],"-S") ==  0)
		{
			i++;
			if(i < argc)
			{
				int tmp = strtol(argv[i], &pc, 10);
				if(tmp <= 0)
				{
					printf("Error: invalid value for -S\n");
					exit(0);
				}
				else
				{
					if(isPowerOfTwo(tmp))
					{
						S = tmp;
						hasS = true;
					}
					else
					{
						printf("Error: S must be a power of 2");
						exit(0);
					}
				}
			}
			else
			{
				printf("Error: invalid value for -S\n");
				exit(0);
			}
		}
		else if(strcmp(argv[i],"-D") ==  0)
		{
			debugMode = true;
		}
		else
		{
			printf("Error: invalid argument tag given {%s}", argv[i]);
			exit(0);
		}
    }
	if(!hasB || !hasE || !hasF || !hasS)
	{
		printf("Error: missing an argument\n");
		exit(0);
	}
}


/* 
 * name: bool isValidFileType
 * Description: Checks to see if the file is a valid type of file.
 * This program is designed to only accept Dinero files, or files of 
 * type ".din".
 * @param char *fName: pointer to the beginning of the filename. 
 * @returns: boolean flag indicating if the file is valid or not.
 *
 */
bool isValidFileType(char * fName)
{
	int len = strlen(fName);
	const char *last_four = &fName[len-4];
	if(strcmp(last_four,".din") == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/* 
 * Name: void parseFile
 * Description: method assumes file has been accepted and all required
 * usage specs have been fulfilled. This method opens the file, skips 
 * past the top file comment lines(apparently dinero files always have 
 * three comment lines at the top, denoted by the '#' symbol). It then
 * loops every line of code and parses from it the block offset, set 
 * index, and tag with the help of helper methods. It calculates the 
 * misses and hits, and stores repeated line uses in an array-based 
 * queue to keep track of them. Handles conflict miss, cold miss, and
 * cache hit.
 * @params: none
 * @returns: none  
 */
void parseFile()
{
	fp = fopen(fileName, "r");
	char * pc;
	char current[BUFF_SIZE];
	int accesses = 0;
	int hits = 0;
	int misses = 0;
	int lineNum = 0;
	while(fgets( current, sizeof(current), fp ) != NULL)
	{
		long int addressNum;
		unsigned long int blockOffset;
		unsigned long int setIndex;
		unsigned long int tag;
		int type;
		bool didHit = false;
		if(lineNum > 2)
		{
			char * pch;
			pch = strtok (current," ");
			int index = 0;
			accesses++;
			while (pch != NULL)
			{	
				if(index == 0)
				{
					type = strtol(pch, &pc, 16);
				}
				if(type >= 0 && type <= 7)
				{	
					if(index == 1)
					{
						addressNum = strtol(pch, &pc, 16);
						if(addressNum != 0)
						{
							blockOffset = getBlockOffset(addressNum);
							setIndex = getSetIndex(addressNum);
							tag = getTag(addressNum);
							
							setType cacheSet = cache[setIndex];
							
							int linesFilled = 0;
							for (int lineIndex = 0; lineIndex < E; lineIndex++) 
							{
								lineType *pLine = &cacheSet.lines[lineIndex];
								if ((*pLine).valid == 1) 
								{
									linesFilled++;
									if ((*pLine).tag == tag) 
									{
										// CACHE HIT
										dequeue(cacheSet.lruQueue, lineIndex);
										enqueue(cacheSet.lruQueue, lineIndex);
										hits++;
										didHit = true;
										break;
									}
								}
							}
							if (!didHit) 
							{
								misses++;
								if (linesFilled == E) 
								{
									// CONFLICT MISS
									int lruIndex = cacheSet.lruQueue[0];
									dequeue(cacheSet.lruQueue, lruIndex);
									enqueue(cacheSet.lruQueue, lruIndex);
									lineType *resetLine = &cacheSet.lines[lruIndex];
									(*resetLine).tag = tag;

								} 
								else 
								{
									// COLD MISS
									for (int lineIndex = 0; lineIndex < E; lineIndex++) 
									{
										lineType *pLine = &cacheSet.lines[lineIndex];
										if ((*pLine).valid == 0) 
										{
											// enqueue the line index onto the lru policy queue
											enqueue(cacheSet.lruQueue, lineIndex);
											// bit to the given cache line
											(*pLine).tag = tag;
											(*pLine).valid = 1;
											break;
										}
									}
								}
							}
							if(debugMode)
							{
								printCache(cache, E, S, addressNum, tag, setIndex, blockOffset, didHit);
							}
						}
					}
				}
				index++;
				pch = strtok (NULL, " ");
			}
		}
		lineNum++;
	}
	fclose( fp );
	printResults(accesses, hits, misses);
}

/*
 * Name: int isPowerOfTwo
 * Description: returns if an int is a power of two
 * @param unsigned int n: number to be determined as being power of 2 or not
 * @returns: int showing if the number used as param is a power of two or not
 */
int isPowerOfTwo(unsigned int n) 
{
    return ((n != 0) && !(n & (n - 1)));
}

/*
 * Name: void enqueue
 * Description: Enqueue an item into the queue.
 * @param lruQueue:A pointer to the start of the lru queue
 * @param item: The item to be added to the queue
 * @returns: none
 */
void enqueue(int *lruQueue, int item) 
{
    int queueOffset;
    for (queueOffset = 0; queueOffset < E; queueOffset++) 
	{
        if (lruQueue[queueOffset] == -1) 
		{
            break;
        }
    }
    if (queueOffset == E) 
	{
        queueOffset--;
    }
    lruQueue[queueOffset] = item;
}


/*
 * Name: void dequeue
 * Description: dequeues a given item from the given lruQueue.
 * @param lruQueue: A pointer to the start of the lru queue
 * @param item: The item to be dequeued from the queue
 * @returns: none
 */
void dequeue(int *lruQueue, int item) 
{
    int queueOffset;
    for (queueOffset = 0; queueOffset < E; queueOffset++) 
	{
        if (lruQueue[queueOffset] == item) 
		{
            break;
        }
    }
    if (queueOffset == E) 
	{
        queueOffset--;
    }
    for (int i = queueOffset; i < (E - 1); i++) 
	{
        lruQueue[i] = lruQueue[i + 1];
    }
    lruQueue[E - 1] = -1;
}


/*
 * Name: unsigned long int getBlockOffset
 * Description: gets the block offset. 
 * @param unsigned long int rawAddressLine: line stripped from .din file 
 * that contains address, tag, block offset and set index.
 * @returns: the block offset 
 */
unsigned long int getBlockOffset(unsigned long int rawAddressLine)
{
	unsigned long int blockOffset = rawAddressLine % B;
    return blockOffset;
}

/*
 * Name: unsigned long int getSetIndex
 * Description: gets the set index
 * @param unsigned long int rawAddressLine: line stripped from .din file
 * that contains address, tag, block offset and a set index.
 * @returns: the set index 
 */
unsigned long int getSetIndex(unsigned long int rawAddressLine)
{
	
	unsigned long int setIndex  = rawAddressLine / B;
	setIndex = setIndex % S;
	return setIndex;
}

/*
 * Name: unsigned long int getTag
 * Description: gets the tag from an address.
 * @param unsinged long int rawAddressLine: line stripped from .din file
 * that contains the address, tag, block offset and set index.
 * @returns: the tag
 */
unsigned long int getTag(unsigned long int rawAddressLine)
{
	unsigned long int tag = rawAddressLine >> (int)(log2f(S) + log2f(B));
	return tag;
}
