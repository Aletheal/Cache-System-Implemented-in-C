#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <math.h>   //Needed for computing S and B
#include <strings.h>  //Needed for processing strings
//Inclusion of generally useful header files
#include "cachelab.h"

//We were told to use assume a 64 bit memory system. Therefore, use 64-bit variable.
typedef unsigned long long int memory_address;

typedef struct{
  int access_no;
  memory_address iD;
  int validtest;
  char *block;
} mem_line;
//Cache and cache-set objects
typedef mem_line* set_lines;
typedef set_lines* cache;

int verbosity = 0; /* Will print the trace if set. Global variable */

//Global counters used to record cache statistics
int hit = 0;
int miss = 0;
int evict = 0;
//Important computational parameters
int s = 0;
int b = 0;
int E = 0;

// Number of sets, lines, and overall block size used to generate cache. COMPLETE
cache initCache (long long set_total, int line_total) {

    cache myCache;
    set_lines myset;
    mem_line myline;

    int i;
    int j;

    myCache = (set_lines *) malloc(sizeof(set_lines) * set_total);

    for (i = 0; i < set_total; i++)
    {
        myset =  (mem_line *) malloc(sizeof(mem_line) * line_total);
        myCache[i] = myset;
        for (j = 0; j < line_total; j ++)
        {
            myline.access_no = 0;
            myline.validtest = 0;
            myline.iD = 0;
            myset[j] = myline;
        }
    }

    return myCache;
}

//Call to free the cache. Iterates through sets, then frees cache itself. COMPLETE
void freeCache(cache your_cache, long long set_total)
{
    int i;

    for (i = 0; i < set_total; i ++) {
        if (your_cache[i] != NULL) {
            free(your_cache[i]);
        }
    }

    if (your_cache != NULL) {
        free(your_cache);
    }
}
//Scan the cache to check for hits
int scan_cache (cache this_cache, memory_address address) {

        int j;
        int cache_full = 1;

        unsigned long long temp = address << ((64 - (s + b)));
        unsigned long long i = temp >> ((64 - (s + b)) + b);

        memory_address input_tag = address >> (s + b);

        set_lines this_set = this_cache[i];

        for (j = 0; j < E; j ++) {

            mem_line line = this_set[j];

            if (line.validtest) {
                if (line.iD == input_tag) { /* found the right tag - cache hit */
                    hit++;
                    line.access_no++;
                    this_set[j] = line;
                    return 1; //Return case if hit
                }

            } else if (!(line.validtest) && (cache_full)) {
                cache_full = 0;
            }
        }

        miss++;

        int min_used = this_set[0].access_no;
        int max_used = this_set[0].access_no;
        int min_index = 0;
        int empty_index = 0;
        int q;
        mem_line line;

        //Iterate through lines in set. Compare for minimum used and maximum used cases
        for (q = 1; q < E; q ++) {

            line = this_set[q];

            if (min_used > line.access_no) {
                min_index = q;
                min_used = line.access_no;
            }

            if (max_used < line.access_no) {
                max_used = line.access_no;
            }
            if (line.validtest == 0) { //Track empty lines
                empty_index = q;
            }
        }
        if (cache_full) {
            //No empty lines? Evict!
            evict++;
            this_set[min_index].iD = input_tag;
            this_set[min_index].access_no = max_used + 1;
        } else {
            // Update bits at end
            this_set[empty_index].iD = input_tag;
            this_set[empty_index].validtest = 1;
            this_set[empty_index].access_no = max_used + 1;
        }
        return 1; //Successful termination

}
//printUsage function as requested in notes. COMPLETE
void printUsage(char* argv[])
{
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}

int main(int argc, char **argv)
{
  cache this_cache;
  FILE *trace_reader;

  memory_address address;
  int size;

  char cmd; //Used to store command line inputs
  char c;
  char *tracer;

  while( (c=getopt(argc,argv,"s:E:b:t:vh")) != -1){
      switch(c){
      case 's':
          s = atoi(optarg);
          break;
      case 'E':
          E = atoi(optarg);
          break;
      case 'b':
          b = atoi(optarg);
          break;
      case 't':
          tracer = optarg;
          break;
      case 'v':
          verbosity = 1;
          break;
      case 'h':
          printUsage(argv);
          exit(0);
      default:
          printUsage(argv);
          exit(1);
      }
  }
  if (s == 0 || E == 0 || b == 0 || tracer == NULL) {
      printf("%s: Missing required command line argument\n", argv[0]);
      printUsage(argv);
      exit(1);
  }
  this_cache = initCache(pow(2.0, s), E);
  trace_reader = fopen(tracer,"r");

  //Scanner reads in cache based off of case of trace_reader
  if (trace_reader != NULL) {
      while (fscanf(trace_reader, " %c %llx,%d", &cmd, &address, &size) == 3) {
          switch(cmd) {
              case 'I':
              break;
              case 'L':
                  scan_cache(this_cache, address);
              break;
              case 'S':
                  scan_cache(this_cache, address);
              break;
              case 'M':
                  scan_cache(this_cache, address);
                  scan_cache(this_cache, address);
              break;
              default:
              break;
          }
      }
  }
  //Final result print out
  printSummary(hit, miss, evict);

  freeCache(this_cache, pow(2.0, s));
  fclose(trace_reader);
  return 0;
}
