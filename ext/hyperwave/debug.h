/****************************************************************************
*
*          Copyright (C) 1991 Kendall Bennett.
*              All rights reserved.
*
* Filename:    $RCSfile$
* Version:    $Revision$
*
* Language:    ANSI C
* Environment:  any
*
* Description:  General header file for portable code.
*
* $Id$
*
* Revision History:
* -----------------
*
* $Log$
* Revision 1.1  1999/04/21 23:37:47  ssb
* moved db
*
* Revision 1.1.1.1  1999/04/07 21:03:29  zeev
* PHP 4.0
*
* Revision 1.1.1.1  1999/03/17 04:29:10  andi
* PHP4
*
* Revision 1.1.1.1  1998/12/21 07:56:22  andi
* Trying to start the zend CVS tree
*
* Revision 1.1  1998/08/12 09:29:16  steinm
* First version of Hyperwave module.
*
* Revision 1.6  92/03/15  12:51:48  kjb
* Added MK_FP macro and ushort typedef.
* 
* Revision 1.5  91/10/28  03:17:33  kjb
* Ported to the Iris.
* 
* Revision 1.4  91/09/26  15:29:02  kjb
* Added stuff for the SGI Iris 4D.
* 
* Revision 1.3  91/09/26  10:07:04  kjb
* Added general typedef stuff.
* 
* Revision 1.2  91/09/03  18:19:14  ROOT_DOS
* Added a few defines that are supplied by <dir.h> for UNIX compatibility.
*
* Revision 1.1  91/08/16  13:19:06  ROOT_DOS
* Initial revision
*
****************************************************************************/

#ifndef  DEBUG_H
#define  DEBUG_H

#ifdef  DEBUG
#    define D(x) x
#else
#    define D(x)
#endif

#define  PRIVATE  static
#define  PUBLIC

#ifdef   __MSDOS__        /* Compiling for MSDOS          */
#    define MS(x) x
#    define UX(x)
#    define IR(x)
#    define _8086    /* We know we have an 8086 type processor  */
#if defined(__COMPACT__) || defined(__LARGE__) || defined(__HUGE__)
#    define LDATA
#    define NULL  0L
#else
#    define NULL  0
#endif
#else  __MSDOS__
#ifdef  __IRIS4D__        /* Compiling for the SGI Iris 4D    */
#    define MS(x)
#    define UX(x) x      /* The Iris is a UNIX machine      */
#    define IR(x) x
#    define O_BINARY 0    /* no binary input mode in UNIX open()  */
#    define MAXFILE  255    /* These are defined in <dir.h>, but  */
#    define MAXDIR  255    /* on UNIX machines, we just define    */
#    define MAXPATH  255    /* them all to be the same size      */
#    define far        /* Near and far do not exist under    */
#    define near        /* UNIX or the Iris.          */
#    define NULL ((void *)0)
#else  __IRIS4D__        /* Assume UNIX compilation        */
#    define MS(x)
#    define UX(x) x
#    define IR(x)
#    define O_BINARY 0    /* no binary input mode in UNIX open()  */
#    define MAXFILE  255    /* These are defined in <dir.h>, but  */
#    define MAXDIR  255    /* on UNIX machines, we just define    */
#    define MAXPATH  255    /* them all to be the same size      */
#    define far        /* Near and far do not exist under    */
#    define near        /* UNIX or the Iris.          */
#    ifndef NULL
#      define NULL ((void *)0)
#    endif
#endif  __IRIS4D__
#endif  __MSDOS__

/****************************************************************************
*
*  SEG(p)    Evaluates to the segment portion of an 8086 address.
*  OFF(p)    Evaluates to the offset portion of an 8086 address.
*  FP(s,o)    Creates a far pointer given a segment offset pair.
*  PHYS(p)    Evaluates to a long holding a physical address
*
****************************************************************************/

#ifdef  _8086
#    define SEG(p)  ( ((unsigned *)&(void far *)(p))[1] )
#    define OFF(p)  ( (unsigned)(p) )
#    define FP(s,o)  ( (void far *)( ((unsigned long)s << 16) +  \
              (unsigned long)o ))
#    define PHYS(p)  ( (unsigned long)OFF(p) +            \
              ((unsigned long)SEG(p) << 4))
#else
#    define PHYS(p)   (p)
#endif  _8086

/****************************************************************************
*
*  NUMELE(array)    Evaluates to the array size in elements
*  LASTELE(array)    Evaluates to a pointer to the last element
*  INBOUNDS(array,p)  Evaluates to true if p points into the array
*  RANGE(a,b,c)    Evaluates to true if a <= b <= c
*  max(a,b)      Evaluates to a or b, whichever is larger
*  min(a,b)      Evaluates to a or b, whichever is smaller
*  ABS(a)        Evaluates to the absolute value of a
*  NBITS(type)      Returns the number of bits in a variable of the
*            indicated type
*  MAXINT        Evaluates to the value of the largest signed integer
*
****************************************************************************/

#define  NUMELE(a)    (sizeof(a)/sizeof(*(a)))
#define  LASTELE(a)    ((a) + (NUMELE(a)-1))
#ifdef  LDATA
#define  TOOHIGH(a,p)  ((long)PHYS(p) - (long)PHYS(a) > (long)(NUMELE(a)-1))
#define  TOOLOW(a,p)    ((long)PHYS(p) - (long)PHYS(a) < 0)
#else
#define  TOOHIGH(a,p)  ((long)(p) - (long)(a) > (long)(NUMELE(a)-1))
#define  TOOLOW(a,p)    ((long)(p) - (long)(a) < 0)
#endif
#define  INBOUNDS(a,p)  ( ! (TOOHIGH(a,p) || TOOLOW(a,p)) )

#define  _IS(t,x) (((t)1 << (x)) != 0)  /* Evaluates true if the width of */
                    /* variable of type t is < x.    */
                    /* The != 0 assures that the      */
                    /* answer is 1 or 0          */

#define  NBITS(t) (4 * (1 + _IS(t,4) + _IS(t,8) + _IS(t,12) + _IS(t,16) \
             + _IS(t,20) + _IS(t,24) + _IS(t,28) + _IS(t,32)))

#define  MAXINT      (((unsigned)~0) >> 1)

#ifndef  MAX
#    define MAX(a,b)  ( ((a) > (b)) ? (a) : (b))
#endif
#ifndef  MIN
#    define MIN(a,b) ( ((a) < (b)) ? (a) : (b))
#endif
#ifndef  ABS
#    define ABS(a)  ((a) >= 0 ? (a) : -(a))
#endif

#define  RANGE(a,b,c)  ( (a) <= (b) && (b) <= (c) )

/* General typedefs  */

#ifndef  __GENDEFS
#define  __GENDEFS
typedef  void      *ptr;
typedef void near    *nearptr;
typedef  void far    *farptr;
/*typedef  unsigned char  uchar;
typedef  unsigned short  ushort;
typedef  unsigned int  uint;
typedef  unsigned long  ulong;*/
typedef  int        bool;
#endif  __GENDEFS

/* Boolean truth values */

#define  false    0
#define  true    1
#define  FALSE    0
#define  TRUE    1
#define  NO      0
#define  YES      1

#endif  /* DEBUG_H */
