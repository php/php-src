/* Copyright (C) 2000 MySQL AB & MySQL Finland AB & TCX DataKonsult AB
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
   MA 02111-1307, USA */

/*
 * [This posting refers to an article entitled "oops, corrupted memory
 * again!" in net.lang.c.  I am posting it here because it is source.]
 *
 * My tool for approaching this problem is to build another level of data
 * abstraction on top of malloc() and free() that implements some checking.
 * This does a number of things for you:
 *	- Checks for overruns and underruns on allocated data
 *	- Keeps track of where in the program the memory was malloc'ed
 *	- Reports on pieces of memory that were not free'ed
 *	- Records some statistics such as maximum memory used
 *	- Marks newly malloc'ed and newly free'ed memory with special values
 * You can use this scheme to:
 *	- Find bugs such as overrun, underrun, etc because you know where
 *	  a piece of data was malloc'ed and where it was free'ed
 *	- Find bugs where memory was not free'ed
 *	- Find bugs where newly malloc'ed memory is used without initializing
 *	- Find bugs where newly free'ed memory is still used
 *	- Determine how much memory your program really uses
 *	- and other things
 */

/*
 * To implement my scheme you must have a C compiler that has __LINE__ and
 * __FILE__ macros.  If your compiler doesn't have these then (a) buy another:
 * compilers that do are available on UNIX 4.2bsd based systems and the PC,
 * and probably on other machines; or (b) change my scheme somehow.  I have
 * recomendations on both these points if you would like them (e-mail please).
 *
 * There are 4 functions in my package:
 *	char *NEW( uSize )	Allocate memory of uSize bytes
 *				(equivalent to malloc())
 *	char *REA( pPtr, uSize) Allocate memory of uSize bytes, move data and
 *				free pPtr.
 *				(equivalent to realloc())
 *	FREE( pPtr )		Free memory allocated by NEW
 *				(equivalent to free())
 *	TERMINATE(file)		End system, report errors and stats on file
 * I personally use two more functions, but have not included them here:
 *	char *STRSAVE( sPtr )	Save a copy of the string in dynamic memory
 *	char *RENEW( pPtr, uSize )
 *				(equivalent to realloc())
 */

/*
 * Memory sub-system, written by Bjorn Benson
   Fixed to use my_sys scheme by Michael Widenius
 */

#ifndef SAFEMALLOC
#define SAFEMALLOC			/* Get protos from my_sys */
#endif

#include "mysys_priv.h"
#include <m_string.h>
#include "my_static.h"
#include "mysys_err.h"

#define pNext		tInt._pNext
#define pPrev		tInt._pPrev
#define sFileName	tInt._sFileName
#define uLineNum	tInt._uLineNum
#define uDataSize	tInt._uDataSize
#define lSpecialValue	tInt._lSpecialValue

	/* Static functions prototypes */

static int check_ptr(const char *where, byte *ptr, const char *sFile,
		     uint uLine);
static int _checkchunk(struct remember *pRec, const char *sFile, uint uLine);

/*
 *	Note: both these refer to the NEW'ed
 *	data only.  They do not include
 *	malloc() roundoff or the extra
 *	space required by the remember
 *	structures.
 */

#define ALLOC_VAL	(uchar) 0xA5	/* NEW'ed memory is filled with this */
				/* value so that references to it will	 */
				/* end up being very strange.		 */
#define FREE_VAL	(uchar) 0x8F	/* FREE'ed memory is filled with this */
				/* value so that references to it will	 */
				/* also end up being strange.		 */

#define MAGICKEY	0x14235296	/* A magic value for underrun key */
#define MAGICEND0	0x68		/* Magic values for overrun keys  */
#define MAGICEND1	0x34		/*		"		  */
#define MAGICEND2	0x7A		/*		"		  */
#define MAGICEND3	0x15		/*		"		  */

 /* Warning: do not change the MAGICEND? values to */
 /* something with the high bit set.  Various C    */
 /* compilers (like the 4.2bsd one) do not do the  */
 /* sign extension right later on in this code and */
 /* you will get erroneous errors.		  */


/*
 * gptr _mymalloc( uint uSize, my_string sFile, uint uLine, MyFlags )
 *	Allocate some memory.
 */

gptr _mymalloc (uint uSize, const char *sFile, uint uLine, myf MyFlags)
{
    struct remember *pTmp;
    DBUG_ENTER("_mymalloc");
    DBUG_PRINT("enter",("Size: %u",uSize));

    if (!sf_malloc_quick)
      (void) _sanity (sFile, uLine);

    /* Allocate the physical memory */
    pTmp = (struct remember *) malloc (
		sizeof (struct irem)			/* remember data  */
		+ sf_malloc_prehunc
		+ uSize					/* size requested */
		+ 4					/* overrun mark   */
		+ sf_malloc_endhunc
		);

    /* Check if there isn't anymore memory avaiable */
    if (pTmp == NULL)
    {
      if (MyFlags & MY_FAE)
	error_handler_hook=fatal_error_handler_hook;
      if (MyFlags & (MY_FAE+MY_WME))
      {
	char buff[SC_MAXWIDTH];
	my_errno=errno;
	sprintf(buff,"Out of memory at line %d, '%s'", uLine, sFile);
	my_message(EE_OUTOFMEMORY,buff,MYF(ME_BELL+ME_WAITTANG));
	sprintf(buff,"needed %d byte (%ldk), memory in use: %ld bytes (%ldk)",
		uSize, (uSize + 1023L) / 1024L,
		lMaxMemory, (lMaxMemory + 1023L) / 1024L);
	my_message(EE_OUTOFMEMORY,buff,MYF(ME_BELL+ME_WAITTANG));
      }
      DBUG_PRINT("error",("Out of memory, in use: %ld at line %d, '%s'",
			  lMaxMemory,uLine, sFile));
      if (MyFlags & MY_FAE)
	exit(1);
      DBUG_RETURN ((gptr) NULL);
    }

    /* Fill up the structure */
    *((long*) ((char*) &pTmp -> lSpecialValue+sf_malloc_prehunc)) = MAGICKEY;
    pTmp -> aData[uSize + sf_malloc_prehunc+0] = MAGICEND0;
    pTmp -> aData[uSize + sf_malloc_prehunc+1] = MAGICEND1;
    pTmp -> aData[uSize + sf_malloc_prehunc+2] = MAGICEND2;
    pTmp -> aData[uSize + sf_malloc_prehunc+3] = MAGICEND3;
    pTmp -> sFileName = (my_string) sFile;
    pTmp -> uLineNum = uLine;
    pTmp -> uDataSize = uSize;
    pTmp -> pPrev = NULL;

    /* Add this remember structure to the linked list */
    pthread_mutex_lock(&THR_LOCK_malloc);
    if ((pTmp->pNext=pRememberRoot))
    {
      pRememberRoot -> pPrev = pTmp;
    }
    pRememberRoot = pTmp;

    /* Keep the statistics */
    lCurMemory += uSize;
    if (lCurMemory > lMaxMemory) {
	lMaxMemory = lCurMemory;
    }
    cNewCount++;
    pthread_mutex_unlock(&THR_LOCK_malloc);

    /* Set the memory to the aribtrary wierd value */
#ifdef HAVE_purify
    if (MyFlags & MY_ZEROFILL)
#endif
      bfill(&pTmp -> aData[sf_malloc_prehunc],uSize,
	    (char) (MyFlags & MY_ZEROFILL ? 0 : ALLOC_VAL));
    /* Return a pointer to the real data */
    DBUG_PRINT("exit",("ptr: %lx",&(pTmp -> aData[sf_malloc_prehunc])));
    if (sf_min_adress > &(pTmp -> aData[sf_malloc_prehunc]))
      sf_min_adress = &(pTmp -> aData[sf_malloc_prehunc]);
    if (sf_max_adress < &(pTmp -> aData[sf_malloc_prehunc]))
      sf_max_adress = &(pTmp -> aData[sf_malloc_prehunc]);
    DBUG_RETURN ((gptr) &(pTmp -> aData[sf_malloc_prehunc]));
}

/*
 *  Allocate some new memory and move old memoryblock there.
 *  Free then old memoryblock
 */

gptr _myrealloc (register gptr pPtr, register uint uSize,
		 const char *sFile, uint uLine, myf MyFlags)
{
  struct remember *pRec;
  gptr ptr;
  DBUG_ENTER("_myrealloc");

  if (!pPtr && (MyFlags & MY_ALLOW_ZERO_PTR))
    DBUG_RETURN(_mymalloc(uSize,sFile,uLine,MyFlags));

  if (!sf_malloc_quick)
    (void) _sanity (sFile, uLine);

  if (check_ptr("Reallocating",(byte*) pPtr,sFile,uLine))
    DBUG_RETURN((gptr) NULL);

  pRec = (struct remember *) ((char*) pPtr - sizeof (struct irem)-
			      sf_malloc_prehunc);
  if (*((long*) ((char*) &pRec -> lSpecialValue+sf_malloc_prehunc))
      != MAGICKEY)
  {
    fprintf (stderr, "Reallocating unallocated data at line %d, '%s'\n",
	     uLine, sFile);
    DBUG_PRINT("safe",("Reallocating unallocated data at line %d, '%s'",
		       uLine, sFile));
    (void) fflush(stderr);
    DBUG_RETURN((gptr) NULL);
  }

  if ((ptr=_mymalloc(uSize,sFile,uLine,MyFlags)))	/* Allocate new area */
  {
    uSize=min(uSize,pRec-> uDataSize);		/* Move as much as possibly */
    memcpy((byte*) ptr,pPtr,(size_t) uSize);	/* Copy old data */
    _myfree(pPtr,sFile,uLine,0);		/* Free not needed area */
  }
  else
  {
    if (MyFlags & MY_HOLD_ON_ERROR)
      DBUG_RETURN(pPtr);
    if (MyFlags & MY_FREE_ON_ERROR)
      _myfree(pPtr,sFile,uLine,0);
  }
  DBUG_RETURN(ptr);
} /* _myrealloc */


/*
 * void _myfree( my_string pPtr, my_string sFile, uint uLine, myf myflags)
 *	Deallocate some memory.
 */

void _myfree (gptr pPtr, const char *sFile, uint uLine, myf myflags)
{
  struct remember *pRec;
  DBUG_ENTER("_myfree");
  DBUG_PRINT("enter",("ptr: %lx",pPtr));

  if (!sf_malloc_quick)
    (void) _sanity (sFile, uLine);

  if ((!pPtr && (myflags & MY_ALLOW_ZERO_PTR)) ||
      check_ptr("Freeing",(byte*) pPtr,sFile,uLine))
    DBUG_VOID_RETURN;

  /* Calculate the address of the remember structure */
  pRec = (struct remember *) ((byte*) pPtr-sizeof(struct irem)-
			      sf_malloc_prehunc);

  /* Check to make sure that we have a real remember structure	*/
  /* Note: this test could fail for four reasons:		*/
  /*	(1) The memory was already free'ed			*/
  /*	(2) The memory was never new'ed				*/
  /*	(3) There was an underrun				*/
  /*	(4) A stray pointer hit this location			*/

  if (*((long*) ((char*) &pRec -> lSpecialValue+sf_malloc_prehunc))
      != MAGICKEY)
  {
    fprintf (stderr, "Freeing unallocated data at line %d, '%s'\n",
	     uLine, sFile);
    DBUG_PRINT("safe",("Unallocated data at line %d, '%s'",uLine,sFile));
    (void) fflush(stderr);
    DBUG_VOID_RETURN;
  }

  /* Remove this structure from the linked list */
  pthread_mutex_lock(&THR_LOCK_malloc);
  if (pRec -> pPrev) {
    pRec -> pPrev -> pNext = pRec -> pNext;
  } else {
    pRememberRoot = pRec -> pNext;
  }
  if (pRec -> pNext) {
    pRec -> pNext -> pPrev = pRec -> pPrev;
  }
  /* Handle the statistics */
  lCurMemory -= pRec -> uDataSize;
  cNewCount--;
  pthread_mutex_unlock(&THR_LOCK_malloc);

#ifndef HAVE_purify
  /* Mark this data as free'ed */
  bfill(&pRec->aData[sf_malloc_prehunc],pRec->uDataSize,(pchar) FREE_VAL);
#endif
  *((long*) ((char*) &pRec -> lSpecialValue+sf_malloc_prehunc)) = ~MAGICKEY;

  /* Actually free the memory */
  free ((my_string ) pRec);
  DBUG_VOID_RETURN;
}

	/* Check if we have a wrong  pointer */

static int check_ptr(const char *where, byte *ptr, const char *sFile,
		     uint uLine)
{
  if (!ptr)
  {
    fprintf (stderr, "%s NULL pointer at line %d, '%s'\n",
	     where,uLine, sFile);
    DBUG_PRINT("safe",("Null pointer at line %d '%s'", uLine, sFile));
    (void) fflush(stderr);
    return 1;
  }
#ifndef _MSC_VER
  if ((long) ptr & (MY_ALIGN(1,sizeof(char *))-1))
  {
    fprintf (stderr, "%s wrong aligned pointer at line %d, '%s'\n",
	     where,uLine, sFile);
    DBUG_PRINT("safe",("Wrong aligned pointer at line %d, '%s'",
		       uLine,sFile));
    (void) fflush(stderr);
    return 1;
  }
#endif
  if (ptr < sf_min_adress || ptr > sf_max_adress)
  {
    fprintf (stderr, "%s pointer out of range at line %d, '%s'\n",
	     where,uLine, sFile);
    DBUG_PRINT("safe",("Pointer out of range at line %d '%s'",
		       uLine,sFile));
    (void) fflush(stderr);
    return 1;
  }
  return 0;
}


/*
 * TERMINATE(FILE *file)
 *	Report on all the memory pieces that have not been
 *	free'ed as well as the statistics.
 */

void TERMINATE (FILE *file)
{
  struct remember *pPtr;
  DBUG_ENTER("TERMINATE");
  pthread_mutex_lock(&THR_LOCK_malloc);

  /* Report the difference between number of calls to  */
  /* NEW and the number of calls to FREE.  >0 means more	 */
  /* NEWs than FREEs.  <0, etc.				 */

  if (cNewCount)
  {
    if (file)
    {
      fprintf (file, "cNewCount: %d\n", cNewCount);
      (void) fflush(file);
    }
    DBUG_PRINT("safe",("cNewCount: %d",cNewCount));
  }

  /* Report on all the memory that was allocated with NEW	 */
  /* but not free'ed with FREE.				 */

  if ((pPtr=pRememberRoot))
  {
    if (file)
    {
      fprintf(file, "Memory that was not free'ed (%ld bytes):\n",lCurMemory);
      (void) fflush(file);
    }
    DBUG_PRINT("safe",("Memory that was not free'ed (%ld bytes):",lCurMemory));
    while (pPtr)
    {
      if (file)
      {
	fprintf (file,
		 "\t%6u bytes at 0x%09lx, allocated at line %4u in '%s'\n",
		 pPtr -> uDataSize,
		 (ulong) &(pPtr -> aData[sf_malloc_prehunc]),
		 pPtr -> uLineNum, pPtr -> sFileName);
	(void) fflush(file);
      }
      DBUG_PRINT("safe",
		 ("%6u bytes at 0x%09lx, allocated at line %4d in '%s'",
		  pPtr -> uDataSize, &(pPtr -> aData[sf_malloc_prehunc]),
		  pPtr -> uLineNum, pPtr -> sFileName));
      pPtr = pPtr -> pNext;
    }
  }
  /* Report the memory usage statistics */
  if (file)
  {
    fprintf (file, "Maximum memory usage: %ld bytes (%ldk)\n",
	     lMaxMemory, (lMaxMemory + 1023L) / 1024L);
    (void) fflush(file);
  }
  DBUG_PRINT("safe",("Maximum memory usage: %ld bytes (%ldk)",
		     lMaxMemory, (lMaxMemory + 1023L) / 1024L));
  pthread_mutex_unlock(&THR_LOCK_malloc);
  DBUG_VOID_RETURN;
}


	/* Returns 0 if chunk is ok */

static int _checkchunk (register struct remember *pRec, const char *sFile,
			uint uLine)
{
  reg1 uint uSize;
  reg2 my_string magicp;
  reg3 int flag=0;

  /* Check for a possible underrun */
  if (*((long*) ((char*) &pRec -> lSpecialValue+sf_malloc_prehunc))
      != MAGICKEY)
  {
    fprintf (stderr, "Memory allocated at %s:%d was underrun,",
	     pRec -> sFileName, pRec -> uLineNum);
    fprintf (stderr, " discovered at %s:%d\n", sFile, uLine);
    (void) fflush(stderr);
    DBUG_PRINT("safe",("Underrun at %lx, allocated at %s:%d",
		       &(pRec -> aData[sf_malloc_prehunc]),
		       pRec -> sFileName,
		       pRec -> uLineNum));
    flag=1;
  }

  /* Check for a possible overrun */
  uSize = pRec -> uDataSize;
  magicp = &(pRec -> aData[uSize+sf_malloc_prehunc]);
  if (*magicp++ != MAGICEND0 ||
      *magicp++ != MAGICEND1 ||
      *magicp++ != MAGICEND2 ||
      *magicp++ != MAGICEND3)
  {
    fprintf (stderr, "Memory allocated at %s:%d was overrun,",
	     pRec -> sFileName, pRec -> uLineNum);
    fprintf (stderr, " discovered at '%s:%d'\n", sFile, uLine);
    (void) fflush(stderr);
    DBUG_PRINT("safe",("Overrun at %lx, allocated at %s:%d",
		       &(pRec -> aData[sf_malloc_prehunc]),
		       pRec -> sFileName,
		       pRec -> uLineNum));
    flag=1;
  }
  return(flag);
}


	/* Returns how many wrong chunks */

int _sanity (const char *sFile, uint uLine)
{
  reg1 struct remember *pTmp;
  reg2 int flag=0;
  uint count=0;

  pthread_mutex_lock(&THR_LOCK_malloc);
  count=cNewCount;
  for (pTmp = pRememberRoot; pTmp != NULL && count-- ; pTmp = pTmp -> pNext)
    flag+=_checkchunk (pTmp, sFile, uLine);
  pthread_mutex_unlock(&THR_LOCK_malloc);
  if (count || pTmp)
  {
    const char *format="Safemalloc link list destroyed, discovered at '%s:%d'";
    fprintf (stderr, format, sFile, uLine); fputc('\n',stderr);
    (void) fflush(stderr);
    DBUG_PRINT("safe",(format, sFile, uLine));
    flag=1;
  }
  return flag;
} /* _sanity */


	/* malloc and copy */

gptr _my_memdup(const byte *from, uint length, const char *sFile, uint uLine,
		myf MyFlags)
{
  gptr ptr;
  if ((ptr=_mymalloc(length,sFile,uLine,MyFlags)) != 0)
    memcpy((byte*) ptr, (byte*) from,(size_t) length);
  return(ptr);
} /*_my_memdup */


my_string _my_strdup(const char *from, const char *sFile, uint uLine,
		     myf MyFlags)
{
  gptr ptr;
  uint length=(uint) strlen(from)+1;
  if ((ptr=_mymalloc(length,sFile,uLine,MyFlags)) != 0)
    memcpy((byte*) ptr, (byte*) from,(size_t) length);
  return((my_string) ptr);
} /* _my_strdup */
