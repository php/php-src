/*
 * xmlmemory.c:  libxml memory allocator wrapper.
 *
 * daniel@veillard.com
 */

#define IN_LIBXML
#include "libxml.h"

#include <string.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#endif

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

/**
 * MEM_LIST:
 *
 * keep track of all allocated blocks for error reporting 
 * Always build the memory list !
 */
#if DEBUG && !defined(MEMLIST)
#define MEM_LIST /* keep a list of all the allocated memory blocks */
#else
#undef MEM_LIST
#endif

#include <libxml/xmlmemory.h>
#include <libxml/globals.h>
#include <libxml/xmlerror.h>

void xmlMallocBreakpoint(void);
void * xmlMemMalloc(size_t size);
void * xmlMemRealloc(void *ptr,size_t size);
void xmlMemFree(void *ptr);
char * xmlMemoryStrdup(const char *str);

/************************************************************************
 *									*
 * 		Macros, variables and associated types			*
 *									*
 ************************************************************************/


#ifdef xmlMalloc
#undef xmlMalloc
#endif
#ifdef xmlRealloc
#undef xmlRealloc
#endif
#ifdef xmlMemStrdup
#undef xmlMemStrdup
#endif


/*
 * Each of the blocks allocated begin with a header containing informations
 */

#define MEMTAG 0x5aa5

#define MALLOC_TYPE 1
#define REALLOC_TYPE 2
#define STRDUP_TYPE 3

typedef struct memnod {
    unsigned int   mh_tag;
    unsigned int   mh_type;
    unsigned long  mh_number;
    size_t         mh_size;
#ifdef MEM_LIST
   struct memnod *mh_next;
   struct memnod *mh_prev;
#endif
   const char    *mh_file;
   unsigned int   mh_line;
}  MEMHDR;


#ifdef SUN4
#define ALIGN_SIZE  16
#else
#define ALIGN_SIZE  sizeof(double)
#endif
#define HDR_SIZE    sizeof(MEMHDR)
#define RESERVE_SIZE (((HDR_SIZE + (ALIGN_SIZE-1)) \
		      / ALIGN_SIZE ) * ALIGN_SIZE)


#define CLIENT_2_HDR(a) ((MEMHDR *) (((char *) (a)) - RESERVE_SIZE))
#define HDR_2_CLIENT(a)    ((void *) (((char *) (a)) + RESERVE_SIZE))


static unsigned long  debugMemSize = 0;
static unsigned long  debugMaxMemSize = 0;
static int block=0;
static int xmlMemStopAtBlock = 0;
static void *xmlMemTraceBlockAt = NULL;
static int xmlMemInitialized = 0;
#ifdef MEM_LIST
static MEMHDR *memlist = NULL;
#endif

static void debugmem_tag_error(void *addr);
#ifdef MEM_LIST
static void  debugmem_list_add(MEMHDR *);
static void debugmem_list_delete(MEMHDR *);
#endif
#define Mem_Tag_Err(a) debugmem_tag_error(a);

#ifndef TEST_POINT
#define TEST_POINT
#endif

/**
 * xmlMallocBreakpoint:
 *
 * Breakpoint to use in conjunction with xmlMemStopAtBlock. When the block
 * number reaches the specified value this function is called. One need to add a breakpoint
 * to it to get the context in which the given block is allocated.
 */

void
xmlMallocBreakpoint(void) {
    xmlGenericError(xmlGenericErrorContext,
	    "xmlMallocBreakpoint reached on block %d\n", xmlMemStopAtBlock);
}

/**
 * xmlMallocLoc:
 * @size:  an int specifying the size in byte to allocate.
 * @file:  the file name or NULL
 * @line:  the line number
 *
 * a malloc() equivalent, with logging of the allocation info.
 *
 * Returns a pointer to the allocated area or NULL in case of lack of memory.
 */

void *
xmlMallocLoc(size_t size, const char * file, int line)
{
    MEMHDR *p;
    void *ret;
    
    if (!xmlMemInitialized) xmlInitMemory();
#ifdef DEBUG_MEMORY
    xmlGenericError(xmlGenericErrorContext,
	    "Malloc(%d)\n",size);
#endif

    TEST_POINT
    
    p = (MEMHDR *) malloc(RESERVE_SIZE+size);

    if (!p) {
	xmlGenericError(xmlGenericErrorContext,
		"xmlMallocLoc : Out of free space\n");
	xmlMemoryDump();
	return(NULL);
    }   
    p->mh_tag = MEMTAG;
    p->mh_number = ++block;
    p->mh_size = size;
    p->mh_type = MALLOC_TYPE;
    p->mh_file = file;
    p->mh_line = line;
    debugMemSize += size;
    if (debugMemSize > debugMaxMemSize) debugMaxMemSize = debugMemSize;
#ifdef MEM_LIST
    debugmem_list_add(p);
#endif

#ifdef DEBUG_MEMORY
    xmlGenericError(xmlGenericErrorContext,
	    "Malloc(%d) Ok\n",size);
#endif
    
    if (xmlMemStopAtBlock == block) xmlMallocBreakpoint();

    ret = HDR_2_CLIENT(p);

    if (xmlMemTraceBlockAt == ret) {
	xmlGenericError(xmlGenericErrorContext,
			"%p : Malloc(%d) Ok\n", xmlMemTraceBlockAt, size);
	xmlMallocBreakpoint();
    }

    TEST_POINT

    return(ret);
}

/**
 * xmlMemMalloc:
 * @size:  an int specifying the size in byte to allocate.
 *
 * a malloc() equivalent, with logging of the allocation info.
 *
 * Returns a pointer to the allocated area or NULL in case of lack of memory.
 */

void *
xmlMemMalloc(size_t size)
{
    return(xmlMallocLoc(size, "none", 0));
}

/**
 * xmlReallocLoc:
 * @ptr:  the initial memory block pointer
 * @size:  an int specifying the size in byte to allocate.
 * @file:  the file name or NULL
 * @line:  the line number
 *
 * a realloc() equivalent, with logging of the allocation info.
 *
 * Returns a pointer to the allocated area or NULL in case of lack of memory.
 */

void *
xmlReallocLoc(void *ptr,size_t size, const char * file, int line)
{
    MEMHDR *p;
    unsigned long number;

    if (!xmlMemInitialized) xmlInitMemory();
    TEST_POINT

    p = CLIENT_2_HDR(ptr);
    number = p->mh_number;
    if (p->mh_tag != MEMTAG) {
       Mem_Tag_Err(p);
	 goto error;
    }
    p->mh_tag = ~MEMTAG;
    debugMemSize -= p->mh_size;
#ifdef MEM_LIST
    debugmem_list_delete(p);
#endif

    p = (MEMHDR *) realloc(p,RESERVE_SIZE+size);
    if (!p) {
	 goto error;
    }
    if (xmlMemTraceBlockAt == ptr) {
	xmlGenericError(xmlGenericErrorContext,
			"%p : Realloced(%d -> %d) Ok\n",
			xmlMemTraceBlockAt, p->mh_size, size);
	xmlMallocBreakpoint();
    }
    p->mh_tag = MEMTAG;
    p->mh_number = number;
    p->mh_type = REALLOC_TYPE;
    p->mh_size = size;
    p->mh_file = file;
    p->mh_line = line;
    debugMemSize += size;
    if (debugMemSize > debugMaxMemSize) debugMaxMemSize = debugMemSize;
#ifdef MEM_LIST
    debugmem_list_add(p);
#endif

    TEST_POINT

    return(HDR_2_CLIENT(p));
    
error:    
    return(NULL);
}

/**
 * xmlMemRealloc:
 * @ptr:  the initial memory block pointer
 * @size:  an int specifying the size in byte to allocate.
 *
 * a realloc() equivalent, with logging of the allocation info.
 *
 * Returns a pointer to the allocated area or NULL in case of lack of memory.
 */

void *
xmlMemRealloc(void *ptr,size_t size) {
    return(xmlReallocLoc(ptr, size, "none", 0));
}

/**
 * xmlMemFree:
 * @ptr:  the memory block pointer
 *
 * a free() equivalent, with error checking.
 */
void
xmlMemFree(void *ptr)
{
    MEMHDR *p;
    char *target;

    if (ptr == (void *) -1) {
	xmlGenericError(xmlGenericErrorContext,
	    "trying to free pointer from freed area\n");
        goto error;
    }

    if (xmlMemTraceBlockAt == ptr) {
	xmlGenericError(xmlGenericErrorContext,
			"%p : Freed()\n", xmlMemTraceBlockAt);
	xmlMallocBreakpoint();
    }

    TEST_POINT

    target = (char *) ptr;

    p = CLIENT_2_HDR(ptr);
    if (p->mh_tag != MEMTAG) {
        Mem_Tag_Err(p);
        goto error;
    }
    p->mh_tag = ~MEMTAG;
    debugMemSize -= p->mh_size;
    memset(target, -1, p->mh_size);

#ifdef MEM_LIST
    debugmem_list_delete(p);
#endif
    free(p);

    TEST_POINT

    return;
    
error:    
    xmlGenericError(xmlGenericErrorContext,
	    "xmlMemFree(%lX) error\n", (unsigned long) ptr);
    xmlMallocBreakpoint();
    return;
}

/**
 * xmlMemStrdupLoc:
 * @str:  the initial string pointer
 * @file:  the file name or NULL
 * @line:  the line number
 *
 * a strdup() equivalent, with logging of the allocation info.
 *
 * Returns a pointer to the new string or NULL if allocation error occurred.
 */

char *
xmlMemStrdupLoc(const char *str, const char *file, int line)
{
    char *s;
    size_t size = strlen(str) + 1;
    MEMHDR *p;

    if (!xmlMemInitialized) xmlInitMemory();
    TEST_POINT

    p = (MEMHDR *) malloc(RESERVE_SIZE+size);
    if (!p) {
      goto error;
    }
    p->mh_tag = MEMTAG;
    p->mh_number = ++block;
    p->mh_size = size;
    p->mh_type = STRDUP_TYPE;
    p->mh_file = file;
    p->mh_line = line;
    debugMemSize += size;
    if (debugMemSize > debugMaxMemSize) debugMaxMemSize = debugMemSize;
#ifdef MEM_LIST
    debugmem_list_add(p);
#endif
    s = (char *) HDR_2_CLIENT(p);
    
    if (xmlMemStopAtBlock == block) xmlMallocBreakpoint();

    if (s != NULL)
      strcpy(s,str);
    else
      goto error;
    
    TEST_POINT

    if (xmlMemTraceBlockAt == s) {
	xmlGenericError(xmlGenericErrorContext,
			"%p : Strdup() Ok\n", xmlMemTraceBlockAt);
	xmlMallocBreakpoint();
    }

    return(s);

error:
    return(NULL);
}

/**
 * xmlMemoryStrdup:
 * @str:  the initial string pointer
 *
 * a strdup() equivalent, with logging of the allocation info.
 *
 * Returns a pointer to the new string or NULL if allocation error occurred.
 */

char *
xmlMemoryStrdup(const char *str) {
    return(xmlMemStrdupLoc(str, "none", 0));
}

/**
 * xmlMemUsed:
 *
 * Provides the amount of memory currently allocated
 *
 * Returns an int representing the amount of memory allocated.
 */

int
xmlMemUsed(void) {
     return(debugMemSize);
}

#ifdef MEM_LIST
/**
 * xmlMemContentShow:
 * @fp:  a FILE descriptor used as the output file
 * @p:  a memory block header
 *
 * tries to show some content from the memory block
 */

static void
xmlMemContentShow(FILE *fp, MEMHDR *p)
{
    int i,j,len = p->mh_size;
    const char *buf = (const char *) HDR_2_CLIENT(p);

    if (p == NULL) {
	fprintf(fp, " NULL");
	return;
    }

    for (i = 0;i < len;i++) {
        if (buf[i] == 0) break;
	if (!isprint((unsigned char) buf[i])) break;
    }
    if ((i < 4) && ((buf[i] != 0) || (i == 0))) {
        if (len >= 4) {
	    MEMHDR *q;
	    void *cur;

            for (j = 0;j < len -3;j += 4) {
		cur = *((void **) &buf[j]);
		q = CLIENT_2_HDR(cur);
		p = memlist;
		while (p != NULL) {
		    if (p == q) break;
		    p = p->mh_next;
		}
		if ((p != NULL) && (p == q)) {
		    fprintf(fp, " pointer to #%lu at index %d",
		            p->mh_number, j);
		    return;
		}
	    }
	}
    } else if ((i == 0) && (buf[i] == 0)) {
        fprintf(fp," null");
    } else {
        if (buf[i] == 0) fprintf(fp," \"%.25s\"", buf); 
	else {
            fprintf(fp," [");
	    for (j = 0;j < i;j++)
                fprintf(fp,"%c", buf[j]);
            fprintf(fp,"]");
	}
    }
}
#endif

/**
 * xmlMemShow:
 * @fp:  a FILE descriptor used as the output file
 * @nr:  number of entries to dump
 *
 * show a show display of the memory allocated, and dump
 * the @nr last allocated areas which were not freed
 */

void
xmlMemShow(FILE *fp, int nr)
{
#ifdef MEM_LIST
    MEMHDR *p;
#endif

    if (fp != NULL)
	fprintf(fp,"      MEMORY ALLOCATED : %lu, MAX was %lu\n",
		debugMemSize, debugMaxMemSize);
#ifdef MEM_LIST
    if (nr > 0) {
	fprintf(fp,"NUMBER   SIZE  TYPE   WHERE\n");
	p = memlist;
	while ((p) && nr > 0) {
	      fprintf(fp,"%6lu %6lu ",p->mh_number,(unsigned long)p->mh_size);
	    switch (p->mh_type) {
	       case STRDUP_TYPE:fprintf(fp,"strdup()  in ");break;
	       case MALLOC_TYPE:fprintf(fp,"malloc()  in ");break;
	      case REALLOC_TYPE:fprintf(fp,"realloc() in ");break;
			default:fprintf(fp,"   ???    in ");break;
	    }
	    if (p->mh_file != NULL)
	        fprintf(fp,"%s(%d)", p->mh_file, p->mh_line);
	    if (p->mh_tag != MEMTAG)
		fprintf(fp,"  INVALID");
	    xmlMemContentShow(fp, p);
	    fprintf(fp,"\n");
	    nr--;
	    p = p->mh_next;
	}
    }
#endif /* MEM_LIST */    
}

/**
 * xmlMemDisplay:
 * @fp:  a FILE descriptor used as the output file, if NULL, the result is
 *       written to the file .memorylist
 *
 * show in-extenso the memory blocks allocated
 */

void
xmlMemDisplay(FILE *fp)
{
#ifdef MEM_LIST
    MEMHDR *p;
    unsigned idx;
    int     nb = 0;
#if defined(HAVE_LOCALTIME) && defined(HAVE_STRFTIME)
    time_t currentTime;
    char buf[500];
    struct tm * tstruct;

    currentTime = time(NULL);
    tstruct = localtime(&currentTime);
    strftime(buf, sizeof(buf) - 1, "%I:%M:%S %p", tstruct);
    fprintf(fp,"      %s\n\n", buf);
#endif

    
    fprintf(fp,"      MEMORY ALLOCATED : %lu, MAX was %lu\n",
            debugMemSize, debugMaxMemSize);
    fprintf(fp,"BLOCK  NUMBER   SIZE  TYPE\n");
    idx = 0;
    p = memlist;
    while (p) {
	  fprintf(fp,"%-5u  %6lu %6lu ",idx++,p->mh_number,
		  (unsigned long)p->mh_size);
        switch (p->mh_type) {
           case STRDUP_TYPE:fprintf(fp,"strdup()  in ");break;
           case MALLOC_TYPE:fprintf(fp,"malloc()  in ");break;
          case REALLOC_TYPE:fprintf(fp,"realloc() in ");break;
                    default:fprintf(fp,"   ???    in ");break;
        }
	  if (p->mh_file != NULL) fprintf(fp,"%s(%d)", p->mh_file, p->mh_line);
        if (p->mh_tag != MEMTAG)
	      fprintf(fp,"  INVALID");
        nb++;
	if (nb < 100)
	    xmlMemContentShow(fp, p);
	else
	    fprintf(fp," skip");

        fprintf(fp,"\n");
        p = p->mh_next;
    }
#else
    fprintf(fp,"Memory list not compiled (MEM_LIST not defined !)\n");
#endif
}

#ifdef MEM_LIST

static void debugmem_list_add(MEMHDR *p)
{
     p->mh_next = memlist;
     p->mh_prev = NULL;
     if (memlist) memlist->mh_prev = p;
     memlist = p;
#ifdef MEM_LIST_DEBUG
     if (stderr)
     Mem_Display(stderr);
#endif
}

static void debugmem_list_delete(MEMHDR *p)
{
     if (p->mh_next)
     p->mh_next->mh_prev = p->mh_prev;
     if (p->mh_prev)
     p->mh_prev->mh_next = p->mh_next;
     else memlist = p->mh_next;
#ifdef MEM_LIST_DEBUG
     if (stderr)
     Mem_Display(stderr);
#endif
}

#endif

/*
 * debugmem_tag_error:
 *
 * internal error function.
 */
 
static void debugmem_tag_error(void *p)
{
     xmlGenericError(xmlGenericErrorContext,
	     "Memory tag error occurs :%p \n\t bye\n", p);
#ifdef MEM_LIST
     if (stderr)
     xmlMemDisplay(stderr);
#endif
}

static FILE *xmlMemoryDumpFile = NULL;


/**
 * xmlMemoryDump:
 *
 * Dump in-extenso the memory blocks allocated to the file .memorylist
 */

void
xmlMemoryDump(void)
{
    FILE *dump;

    if (debugMaxMemSize == 0)
	return;
    dump = fopen(".memdump", "w");
    if (dump == NULL)
	xmlMemoryDumpFile = stderr;
    else xmlMemoryDumpFile = dump;

    xmlMemDisplay(xmlMemoryDumpFile);

    if (dump != NULL) fclose(dump);
}


/****************************************************************
 *								*
 *		Initialization Routines				*
 *								*
 ****************************************************************/

static int xmlInitMemoryDone = 0;

/**
 * xmlInitMemory:
 *
 * Initialize the memory layer.
 *
 * Returns 0 on success
 */
int
xmlInitMemory(void)
{
     int ret;
     
#ifdef HAVE_STDLIB_H
     char *breakpoint;
#endif     

     if (xmlInitMemoryDone) return(-1);

#ifdef HAVE_STDLIB_H
     breakpoint = getenv("XML_MEM_BREAKPOINT");
     if (breakpoint != NULL) {
         sscanf(breakpoint, "%d", &xmlMemStopAtBlock);
     }
#endif     
#ifdef HAVE_STDLIB_H
     breakpoint = getenv("XML_MEM_TRACE");
     if (breakpoint != NULL) {
         sscanf(breakpoint, "%p", &xmlMemTraceBlockAt);
     }
#endif     
    
#ifdef DEBUG_MEMORY
     xmlGenericError(xmlGenericErrorContext,
	     "xmlInitMemory() Ok\n");
#endif     
     xmlMemInitialized = 1;
     xmlInitMemoryDone = 1;

     ret = 0;
     return(ret);
}

/**
 * xmlMemSetup:
 * @freeFunc: the free() function to use
 * @mallocFunc: the malloc() function to use
 * @reallocFunc: the realloc() function to use
 * @strdupFunc: the strdup() function to use
 *
 * Override the default memory access functions with a new set
 * This has to be called before any other libxml routines !
 *
 * Should this be blocked if there was already some allocations
 * done ?
 *
 * Returns 0 on success
 */
int
xmlMemSetup(xmlFreeFunc freeFunc, xmlMallocFunc mallocFunc,
            xmlReallocFunc reallocFunc, xmlStrdupFunc strdupFunc) {
    if (freeFunc == NULL)
	return(-1);
    if (mallocFunc == NULL)
	return(-1);
    if (reallocFunc == NULL)
	return(-1);
    if (strdupFunc == NULL)
	return(-1);
    xmlFree = freeFunc;
    xmlMalloc = mallocFunc;
    xmlRealloc = reallocFunc;
    xmlMemStrdup = strdupFunc;
    return(0);
}

/**
 * xmlMemGet:
 * @freeFunc: place to save the free() function in use
 * @mallocFunc: place to save the malloc() function in use
 * @reallocFunc: place to save the realloc() function in use
 * @strdupFunc: place to save the strdup() function in use
 *
 * Provides the memory access functions set currently in use
 *
 * Returns 0 on success
 */
int
xmlMemGet(xmlFreeFunc *freeFunc, xmlMallocFunc *mallocFunc,
	  xmlReallocFunc *reallocFunc, xmlStrdupFunc *strdupFunc) {
    if (freeFunc != NULL) *freeFunc = xmlFree;
    if (mallocFunc != NULL) *mallocFunc = xmlMalloc;
    if (reallocFunc != NULL) *reallocFunc = xmlRealloc;
    if (strdupFunc != NULL) *strdupFunc = xmlMemStrdup;
    return(0);
}

