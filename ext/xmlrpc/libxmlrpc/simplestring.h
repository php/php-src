/*
  This file is part of libXMLRPC - a C library for xml-encoded function calls.

  Author: Dan Libby (dan@libby.com)
  Epinions.com may be contacted at feedback@epinions-inc.com
*/

/*
  Copyright 2000 Epinions, Inc.

  Subject to the following 3 conditions, Epinions, Inc.  permits you, free
  of charge, to (a) use, copy, distribute, modify, perform and display this
  software and associated documentation files (the "Software"), and (b)
  permit others to whom the Software is furnished to do so as well.

  1) The above copyright notice and this permission notice shall be included
  without modification in all copies or substantial portions of the
  Software.

  2) THE SOFTWARE IS PROVIDED "AS IS", WITHOUT ANY WARRANTY OR CONDITION OF
  ANY KIND, EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION ANY
  IMPLIED WARRANTIES OF ACCURACY, MERCHANTABILITY, FITNESS FOR A PARTICULAR
  PURPOSE OR NONINFRINGEMENT.

  3) IN NO EVENT SHALL EPINIONS, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
  SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES OR LOST PROFITS ARISING OUT
  OF OR IN CONNECTION WITH THE SOFTWARE (HOWEVER ARISING, INCLUDING
  NEGLIGENCE), EVEN IF EPINIONS, INC.  IS AWARE OF THE POSSIBILITY OF SUCH
  DAMAGES.

*/

#ifndef __SIMPLESTRING_H__
 #define __SIMPLESTRING_H__

/*-********************************
* begin simplestring header stuff *
**********************************/

#ifdef __cplusplus
extern "C" {
#endif

   /****s* struct/simplestring
 * NAME
 *  simplestring
 * NOTES
 *   represents a string efficiently for fast appending, etc.
 * SOURCE
 */
typedef struct _simplestring {
   char* str;         /* string buf               */
   size_t len;           /* length of string/buf     */
   size_t size;          /* size of allocated buffer */
} simplestring;
/******/

#ifndef NULL
 #define NULL 0
#endif

void simplestring_init(simplestring* string);
void simplestring_clear(simplestring* string);
void simplestring_free(simplestring* string);
void simplestring_add(simplestring* string, const char* add);
void simplestring_addn(simplestring* string, const char* add, size_t add_len);

#ifdef __cplusplus
}
#endif

/*-******************************
* end simplestring header stuff *
********************************/

#endif /* __SIMPLESTRING_H__ */
