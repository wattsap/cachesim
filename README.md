Cache Simulation Assignment:


For this programming assignment, you'll write a simple cache simulator that takes as input a trace of addresses and cache configuration parameters (number of sets, number of lines and the block size) and outputs the number of accesses, number of hits, number of misses and the miss rate.

The first section of the assignment gives an overview of how the cache simulator works. More details are provided in later sections. Be sure to read the entire assignment before starting.

I. The general idea

The simulator will takes as input the command line arguments that specify the input file of addresses and the cache configuration. It will then open the file, create the cache and loop until eof. During each iteration of the loop, the simulator will:

1. read the type of the address and the actual address

2. if the type is greater than 7, then continue to next iteration of loop (1-7 indicates a load or store operation)

3. parse the address into tag, index, and offset

4. look at every line in set indicated by index for the given tag

5. if match then hit

6. if no match then miss; do replacement to insert the tag into the set

7. For both hits and misses, update the lru queue used for replacement

After the loop is exited, print the number of accesses, hits, misses, and miss rate.

Note the simulation handles loads and stores the same way. This indicates that the simulator is assuming a write-miss allocate policy (when a write-miss occurs, allocate space in the cache for the block containing the data to be modified).

II. The input file

Your program will accept as input dinero trace files. These files contain information, such as memory accesses, collected while a program is running. Some of the information in the file will be ignored by our program since the trace file contains more information than is needed for our purposes.

The file of traces contains three comment lines that are to be read and discarded by your program (don't forget to do this!) followed by some number of lines where each line represents a memory access. The memory access lines are of the form:

<type> <address in hex> <value to be stored or 0> <optional comment>

For example, the first memory access line in the file simple.din contains:

0 440010ab 0 #miss, set 0

The type indicates that this access is a load (read from memory). The 440010ab is the hex value of the memory location to be accessed. Since the type of access is a load, the third value is 0. The rest of the line contains a comment.

Your program needs to read the first two values of each memory access line and read and discard the rest of the line. The first value needs to be between 0 and 7 (inclusive). A value that is greater than 7 indicates a line that is to be ignored. A type between 0 and 7 indicates an access that is a load word, load byte, load halfword, store word, etc. These are the lines that we are interested in.

III. The cache configuration

The cache will be configured according to the user's command line arguments. In addition to the name of the trace file, the user will provide the number of sets in the cache, the number of lines per set and the number of bytes per block. Each line in a set will hold a valid "bit" (is there data in this line?) and the tag (the portion of the address of the data in the cache line). The line will not actually be used to hold the data from memory. We don't need to know the actual data that is being read or written to in order to calculate hits and misses. We only need to keep track of the addresses. 

Your cache simulator should be invoked something like this:

cachesim -E 8 -S 16 -B 256 -F filename

The value after the -E specifies the number of lines per set. The value after the -S specifies the number of sets. The value after the -B specifies the size of the block. These values are used to create the cache structure as well as determine what portions of the address are the tag, index (set number) and offset. For example, if the cache simulator is invoked like above, then given the address in the line below:

0 440010ab 0 #miss, set 0

the tag is 44001, the index is 0 and the offset is ab. (The number of bytes in a block is 256 thus the block offset is the rightmost 8 bits. The number of sets in the cache is 16 thus the index is the four bits to the left of the offset. The tag consists of the bits leftover.)

Write your code so that the command line arguments can be in any order. For example, you should also be able to invoke the cache simulator like this:

cachesim -S 16 -F filename -B 256 -E 8

The number of sets, the number of lines in the set and the number of bits in a block must be numbers that are powers of two. Otherwise, the simulator should output a usage message and stop execution, similar to this example:

% cacheSim -B 256 -S 16 -E 5 -F simple.din 

usage: cacheSim -S s -E e -B b -F <filename>
s - number of Sets
e - number of Lines per Set
b - number of Bytes per Block
<filename> - name of file containing address trace
s, e, b must be powers of two

Your program should check for errors in the command line arguments (each of those options must be given, the numeric values must be numbers and powers of two, the file open should succeed). You do not need to test for errors in the dinero trace file. This makes sense because the trace file was generated by a program and is more likely to be error free. The command line arguments are provided by error-prone humans.

IV. Testing and Output

The output of the simulator is the number of accesses, number of hits, number of misses, and the miss rate. In order to automate the testing, I've provided a printResults routine and a run.pl perl script. The printResults routine takes as input the number of accesses, hits, and misses, and outputs those value to standard output. The run.pl perl script will run your cache simulator and the instructor's cache simulator on three different input files and and cache configurations and use diff to compare their outputs. Your output should match the output of the instructor's simulator. The perl script doesn't keep around the output produced by either simulator, but does indicate the tests upon which your simulator fails so you can re-run the tests by hand and compare their outputs. (Each produces only one line of output.)

In addition, I have provided a printCache routine that takes as input a pointer to the cache structure, the number of sets in the cache, the number of lines in each set, the address of the most recent memory access, the tag, index and offset of that address and whether that access resulted in a hit or a miss. Information about the memory access is displayed and the nonempty sets of the cache are displayed. This function is useful for debugging.

In addition to the command line arguments mentioned earlier, your instructor's cache simulator can also be given a -D (debug) argument. This argument causes the simulator to call printCache after each access. For example, if the instructor's cache simulator is invoked like this:

cacheSim -B 256 -S 16 -E 4 -F simple.din -D

then cacheSim outputs the following:

Address: 440010ab, Tag: 44001, Index: 0, Offset: ab (miss)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44001, 1) (0, 0) (0, 0) (0, 0) LRUs: 0, -1, -1, -1, 

Address: 440010cd, Tag: 44001, Index: 0, Offset: cd (hit)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44001, 1) (0, 0) (0, 0) (0, 0) LRUs: 0, -1, -1, -1, 

Address: 441010ab, Tag: 44101, Index: 0, Offset: ab (miss)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44001, 1) (44101, 1) (0, 0) (0, 0) LRUs: 0, 1, -1, -1, 

Address: 440010ab, Tag: 44001, Index: 0, Offset: ab (hit)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44001, 1) (44101, 1) (0, 0) (0, 0) LRUs: 1, 0, -1, -1, 

Address: 442010ab, Tag: 44201, Index: 0, Offset: ab (miss)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44001, 1) (44101, 1) (44201, 1) (0, 0) LRUs: 1, 0, 2, -1, 

Address: 442010ab, Tag: 44201, Index: 0, Offset: ab (hit)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44001, 1) (44101, 1) (44201, 1) (0, 0) LRUs: 1, 0, 2, -1, 

Address: 443010ab, Tag: 44301, Index: 0, Offset: ab (miss)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44001, 1) (44101, 1) (44201, 1) (44301, 1) LRUs: 1, 0, 2, 3, 

Address: 444010ab, Tag: 44401, Index: 0, Offset: ab (miss)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44001, 1) (44401, 1) (44201, 1) (44301, 1) LRUs: 0, 2, 3, 1, 

Address: 445010ab, Tag: 44501, Index: 0, Offset: ab (miss)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44501, 1) (44401, 1) (44201, 1) (44301, 1) LRUs: 2, 3, 1, 0, 

Address: 442010ab, Tag: 44201, Index: 0, Offset: ab (hit)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44501, 1) (44401, 1) (44201, 1) (44301, 1) LRUs: 3, 1, 0, 2, 

Address: 446010ab, Tag: 44601, Index: 0, Offset: ab (miss)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44501, 1) (44401, 1) (44201, 1) (44601, 1) LRUs: 1, 0, 2, 3, 

Address: 440011ab, Tag: 44001, Index: 1, Offset: ab (miss)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44501, 1) (44401, 1) (44201, 1) (44601, 1) LRUs: 1, 0, 2, 3, 
1: (44001, 1) (0, 0) (0, 0) (0, 0) LRUs: 0, -1, -1, -1, 

Address: 441011ab, Tag: 44101, Index: 1, Offset: ab (miss)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44501, 1) (44401, 1) (44201, 1) (44601, 1) LRUs: 1, 0, 2, 3, 
1: (44001, 1) (44101, 1) (0, 0) (0, 0) LRUs: 0, 1, -1, -1, 

Address: 442011ab, Tag: 44201, Index: 1, Offset: ab (miss)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44501, 1) (44401, 1) (44201, 1) (44601, 1) LRUs: 1, 0, 2, 3, 
1: (44001, 1) (44101, 1) (44201, 1) (0, 0) LRUs: 0, 1, 2, -1, 

Address: 443011ab, Tag: 44301, Index: 1, Offset: ab (miss)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44501, 1) (44401, 1) (44201, 1) (44601, 1) LRUs: 1, 0, 2, 3, 
1: (44001, 1) (44101, 1) (44201, 1) (44301, 1) LRUs: 0, 1, 2, 3, 

Address: 440011ab, Tag: 44001, Index: 1, Offset: ab (hit)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44501, 1) (44401, 1) (44201, 1) (44601, 1) LRUs: 1, 0, 2, 3, 
1: (44001, 1) (44101, 1) (44201, 1) (44301, 1) LRUs: 1, 2, 3, 0, 

Address: 441011ab, Tag: 44101, Index: 1, Offset: ab (hit)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44501, 1) (44401, 1) (44201, 1) (44601, 1) LRUs: 1, 0, 2, 3, 
1: (44001, 1) (44101, 1) (44201, 1) (44301, 1) LRUs: 2, 3, 0, 1, 

Address: 442011ab, Tag: 44201, Index: 1, Offset: ab (hit)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44501, 1) (44401, 1) (44201, 1) (44601, 1) LRUs: 1, 0, 2, 3, 
1: (44001, 1) (44101, 1) (44201, 1) (44301, 1) LRUs: 3, 0, 1, 2, 

Address: 443011ab, Tag: 44301, Index: 1, Offset: ab (hit)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44501, 1) (44401, 1) (44201, 1) (44601, 1) LRUs: 1, 0, 2, 3, 
1: (44001, 1) (44101, 1) (44201, 1) (44301, 1) LRUs: 0, 1, 2, 3, 

Address: 442011ab, Tag: 44201, Index: 1, Offset: ab (hit)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44501, 1) (44401, 1) (44201, 1) (44601, 1) LRUs: 1, 0, 2, 3, 
1: (44001, 1) (44101, 1) (44201, 1) (44301, 1) LRUs: 0, 1, 3, 2, 

Address: 442011ab, Tag: 44201, Index: 1, Offset: ab (hit)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44501, 1) (44401, 1) (44201, 1) (44601, 1) LRUs: 1, 0, 2, 3, 
1: (44001, 1) (44101, 1) (44201, 1) (44301, 1) LRUs: 0, 1, 3, 2, 

Address: 440011ab, Tag: 44001, Index: 1, Offset: ab (hit)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44501, 1) (44401, 1) (44201, 1) (44601, 1) LRUs: 1, 0, 2, 3, 
1: (44001, 1) (44101, 1) (44201, 1) (44301, 1) LRUs: 1, 3, 2, 0, 

Address: 444011ab, Tag: 44401, Index: 1, Offset: ab (miss)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44501, 1) (44401, 1) (44201, 1) (44601, 1) LRUs: 1, 0, 2, 3, 
1: (44001, 1) (44401, 1) (44201, 1) (44301, 1) LRUs: 3, 2, 0, 1, 

Accesses: 23, Hits: 11, Misses: 12, Miss Rate: 0.5217

You may also choose to implement a -D option this way or simply have a call to printCache that you use only for debugging and delete later.

Note that one of the dinero trace files (simple.din - the one used in the examples in this assignment) contains comments that indicate a specific cache configuration and more comments that indicate whether a access will result in a hit or a miss if that configuration is used. This is a good first test for your simulator.

V. Cache Structure

Your instructor has provided the typedefs needed for the cache structure and a function to create and initialize the cache. These are the typedefs from the instructor provided cache.h file:

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

The cache is declared as follows:
setType * cache;

In other words, a cache is an array of sets. Each set consists of an array of lines and an array of ints used to implement the lru policy. Each line has a valid field and a tag. The initCache function in the instructor provided cache.c file can be used to dynamically allocate the cache.

VI. LRU Replacement policy

When there is a miss in the cache, space must be allocated in the appropriate set (indicated by the index) for the tag of the missed access. If the set is not full (one of the lines has a valid field of 0) then the tag is stored in a line with a valid field of 0. At that point, the valid field is set to 1. If the set is full, then a least recently used replacement policy would be used to replace the tag whose last access is the furthest in the past. The lruQueue array is used to implement that policy.

The lruQueue array is the same size as the number of lines in the set. It is used to implement a queue of indices. When a memory location is accessed whose tag is in line j of set i (cache[i].line[j].tag), then j is moved to the end of the queue. When a set is full, cache[i].lruQueue[0] (where i is the address index/set number) contains the index of the line that is to be replaced next. For example, consider these accesses taken from the -D output above.

The first access (seen below) results in a miss. The tag is stored in cache[0].line[0].tag (set 0, line 0) and the line index (0) is added to the lruQueue. Initially the lruQueue is empty, caused by setting each element in the queue to -1.

Address: 440010ab, Tag: 44001, Index: 0, Offset: ab (miss)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44001, 1) (0, 0) (0, 0) (0, 0) LRUs: 0, -1, -1, -1, 

The second access is a hit. 

Address: 440010cd, Tag: 44001, Index: 0, Offset: cd (hit)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44001, 1) (0, 0) (0, 0) (0, 0) LRUs: 0, -1, -1, -1, 

The third access results in a miss. The tag is stored in cache[0].line[1].tag (set 0, line 1).

Address: 441010ab, Tag: 44101, Index: 0, Offset: ab (miss)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44001, 1) (44101, 1) (0, 0) (0, 0) LRUs: 0, 1, -1, -1, 

The fourth access is a hit on the tag in cache[0].line[0].tag (set 0, line 0). Notice how the index 0 is removed from the front of a queue and placed at the end of the queue. The next index in the queue (1) is now the least recently used.

Address: 440010ab, Tag: 44001, Index: 0, Offset: ab (hit)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44001, 1) (44101, 1) (0, 0) (0, 0) LRUs: 1, 0, -1, -1, 

The fifth access is a miss causing the tag to be stored in cache[0].line[2].tag (set 0, line 2). The line index 2 is placed at the end of the lru queue.

Address: 442010ab, Tag: 44201, Index: 0, Offset: ab (miss)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44001, 1) (44101, 1) (44201, 1) (0, 0) LRUs: 1, 0, 2, -1, 

The sixth access causes the index 2 to be removed from the queue and placed back in the queue at the end. This is the same spot so the lru queue output is the same.

Address: 442010ab, Tag: 44201, Index: 0, Offset: ab (hit)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44001, 1) (44101, 1) (44201, 1) (0, 0) LRUs: 1, 0, 2, -1, 

The seventh access is a miss causing the tag to be stored in cache[0].line[3].tag. The line index 3 is put at the end of the lru queue.

Address: 443010ab, Tag: 44301, Index: 0, Offset: ab (miss)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44001, 1) (44101, 1) (44201, 1) (44301, 1) LRUs: 1, 0, 2, 3, 

The eighth access is a miss causing the tag at cache[0].line[1].tag to be replaced. Why that one? Because the front of the queue contains index 1. This also causes the index 1 to be removed from the queue and placed back in the queue at the end. 

Address: 444010ab, Tag: 44401, Index: 0, Offset: ab (miss)
Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..
0: (44001, 1) (44401, 1) (44201, 1) (44301, 1) LRUs: 0, 2, 3, 1, 

Note this lru policy is implemented using two queue operations. One is the standard enqueue operation that puts a value at the end of the queue. The second is a non-standard dequeue operation. The dequeue operation removes from the queue a specific value. For example, if there is a match between the tag in the address and cache[i].line[j].tag then j will be dequeued (removed from its current place in the queue) and enqueued (added to the end).
