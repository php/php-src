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

#include <php.h>


#define SIMPLESTRING_INCR 32

/****h* ABOUT/simplestring
 * NAME
 *   simplestring
 * AUTHOR
 *   Dan Libby, aka danda  (dan@libby.com)
 * CREATION DATE
 *   06/2000
 * HISTORY
 *   $Log$
 *   Revision 1.3  2002/08/22 01:25:50  sniper
 *   kill some compile warnings
 *
 *   Revision 1.2  2002/07/05 04:43:53  danda
 *   merged in updates from SF project.  bring php repository up to date with xmlrpc-epi version 0.51
 *
 *   Revision 1.4  2002/02/13 20:58:50  danda
 *   patch to make source more windows friendly, contributed by Jeff Lawson
 *
 *   Revision 1.3  2001/09/29 21:58:05  danda
 *   adding cvs log to history section
 *
 *   10/15/2000 -- danda -- adding robodoc documentation
 * PORTABILITY
 *   Coded on RedHat Linux 6.2.  Builds on Solaris x86.  Should build on just
 *   about anything with minor mods.
 * NOTES
 *   This code was written primarily for xmlrpc, but has found some other uses.
 *
 *   simplestring is, as the name implies, a simple API for dealing with C strings.
 *   Why would I write yet another string API?  Because I couldn't find any that were
 *   a) free / GPL, b) simple/lightweight, c) fast, not doing unnecessary strlens all
 *   over the place.  So.  It is simple, and it seems to work, and it is pretty fast.
 *
 *   Oh, and it is also binary safe, ie it can handle strings with embedded NULLs,
 *   so long as the real length is passed in.
 *
 *   And the masses rejoiced.
 *
 * BUGS
 *   there must be some.
 ******/

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "simplestring.h"

#define my_free(thing)  if(thing) {efree(thing); thing = 0;}

/*----------------------**
* Begin String Functions *
*-----------------------*/

/****f* FUNC/simplestring_init
 * NAME
 *   simplestring_init
 * SYNOPSIS
 *   void simplestring_init(simplestring* string)
 * FUNCTION
 *   initialize string
 * INPUTS
 *   string - pointer to a simplestring struct that will be initialized
 * RESULT
 *   void
 * NOTES
 * SEE ALSO
 *   simplestring_free ()
 *   simplestring_clear ()
 * SOURCE
 */
void simplestring_init(simplestring* string) {
   memset(string, 0, sizeof(simplestring));
}
/******/

static void simplestring_init_str(simplestring* string) {
   string->str = (char*)emalloc(SIMPLESTRING_INCR);
   if(string->str) {
      string->str[0] = 0;
      string->len = 0;
      string->size = SIMPLESTRING_INCR;
   }
   else {
      string->size = 0;
   }
}

/****f* FUNC/simplestring_clear
 * NAME
 *   simplestring_clear
 * SYNOPSIS
 *   void simplestring_clear(simplestring* string)
 * FUNCTION
 *   clears contents of a string
 * INPUTS
 *   string - the string value to clear
 * RESULT
 *   void
 * NOTES
 *   This function is very fast as it does not de-allocate any memory.
 * SEE ALSO
 *
 * SOURCE
 */
void simplestring_clear(simplestring* string) {
   if(string->str) {
      string->str[0] = 0;
   }
   string->len = 0;
}
/******/

/****f* FUNC/simplestring_free
 * NAME
 *   simplestring_free
 * SYNOPSIS
 *   void simplestring_free(simplestring* string)
 * FUNCTION
 *   frees contents of a string, if any. Does *not* free the simplestring struct itself.
 * INPUTS
 *   string - value containing string to be free'd
 * RESULT
 *   void
 * NOTES
 *   caller is responsible for allocating and freeing simplestring* struct itself.
 * SEE ALSO
 *   simplestring_init ()
 * SOURCE
 */
void simplestring_free(simplestring* string) {
   if(string && string->str) {
      my_free(string->str);
      string->len = 0;
   }
}
/******/

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)-1)
#endif
/****f* FUNC/simplestring_addn
 * NAME
 *   simplestring_addn
 * SYNOPSIS
 *   void simplestring_addn(simplestring* string, const char* add, int add_len)
 * FUNCTION
 *   copies n characters from source to target string
 * INPUTS
 *   target  - target string
 *   source  - source string
 *   add_len - number of characters to copy
 * RESULT
 *   void
 * NOTES
 * SEE ALSO
 *   simplestring_add ()
 * SOURCE
 */
void simplestring_addn(simplestring* target, const char* source, size_t add_len) {
   size_t newsize = target->size, incr = 0;
   if(target && source) {
      if(!target->str) {
         simplestring_init_str(target);
      }

      if((SIZE_MAX - add_len) < target->len || (SIZE_MAX - add_len - 1) < target->len) {
    	  /* check for overflows, if there's a potential overflow do nothing */
    	  return;
      }

      if(target->len + add_len + 1 > target->size) {
         /* newsize is current length + new length */
         newsize = target->len + add_len + 1;
         incr = target->size * 2;

         /* align to SIMPLESTRING_INCR increments */
         if (incr) {
            newsize = newsize - (newsize % incr) + incr;
         }
         if(newsize < (target->len + add_len + 1)) {
        	 /* some kind of overflow happened */
        	 return;
         }
         target->str = (char*)erealloc(target->str, newsize);

         target->size = target->str ? newsize : 0;
      }

      if(target->str) {
         if(add_len) {
            memcpy(target->str + target->len, source, add_len);
         }
         target->len += add_len;
         target->str[target->len] = 0; /* null terminate */
      }
   }
}
/******/

/****f* FUNC/simplestring_add
 * NAME
 *   simplestring_add
 * SYNOPSIS
 *   void simplestring_add(simplestring* string, const char* add)
 * FUNCTION
 *   appends a string of unknown length from source to target
 * INPUTS
 *   target - the target string to append to
 *   source - the source string of unknown length
 * RESULT
 *   void
 * NOTES
 * SEE ALSO
 *   simplestring_addn ()
 * SOURCE
 */
void simplestring_add(simplestring* target, const char* source) {
   if(target && source) {
      simplestring_addn(target, source, strlen(source));
   }
}
/******/


/*----------------------
* End String Functions *
*--------------------**/
