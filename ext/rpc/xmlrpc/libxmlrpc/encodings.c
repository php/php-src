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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef PHP_WIN32
#include <php_config.h>
#else
#include <config.w32.h>
#include <stdlib.h>
#endif

static const char rcsid[] = "#(@) $Id$";

#include <errno.h>

#ifdef HAVE_GICONV_H
#include <giconv.h>
#else
#include <iconv.h>
#endif

#include "encodings.h"

static char* convert(const char* src, int src_len, int *new_len, const char* from_enc, const char* to_enc) {
   char* outbuf = 0;

   if(src && src_len && from_enc && to_enc) {
      int outlenleft = src_len;
      int outlen = src_len;
      int inlenleft = src_len;
      iconv_t ic = iconv_open(to_enc, from_enc);
      char* src_ptr = (char*)src;
      char* out_ptr = 0;

      if(ic != (iconv_t)-1) {
         size_t st;
         outbuf = (char*)malloc(outlen + 1);

         if(outbuf) {
            out_ptr = (char*)outbuf;
            while(inlenleft) {
               st = iconv(ic, &src_ptr, &inlenleft, &out_ptr, &outlenleft);
               if(st == -1) {
                  if(errno == E2BIG) {
                     int diff = out_ptr - outbuf;
                     outlen += inlenleft;
                     outlenleft += inlenleft;
                     outbuf = (char*)realloc(outbuf, outlen + 1);
                     if(!outbuf) {
                        break;
                     }
                     out_ptr = outbuf + diff;
                  }
                  else {
                     free(outbuf);
                     outbuf = 0;
                     break;
                  }
               }
            }
         }
         iconv_close(ic);
      }
      outlen -= outlenleft;

      if(new_len) {
         *new_len = outbuf ? outlen : 0;
      }
      if(outbuf) {
         outbuf[outlen] = 0;
      }
   }
   return outbuf;
}

/* returns a new string that must be freed */
char* utf8_encode(const char *s, int len, int *newlen, const char* encoding)
{
   return convert(s, len, newlen, encoding, "UTF-8");
}

/* returns a new string, possibly decoded */
char* utf8_decode(const char *s, int len, int *newlen, const char* encoding)
{
   return convert(s, len, newlen, "UTF-8", encoding);
}

