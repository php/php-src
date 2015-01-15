/*
  zip_source_error.c -- get last error from zip_source
  Copyright (C) 2009-2015 Dieter Baron and Thomas Klausner

  This file is part of libzip, a library to manipulate ZIP archives.
  The authors can be contacted at <libzip@nih.at>

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
  3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.
 
  THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/



#include "zipint.h"



void
zip_source_error(struct zip_source *src, int *ze, int *se)
{
    int e[2];

    if (src->src == NULL) {
        if (src->cb.f(src->ud, e, sizeof(e), ZIP_SOURCE_ERROR) < 0) {
            e[0] = ZIP_ER_INTERNAL;
            e[1] = 0;
        }
    }
    else {
	switch (src->error_source) {
	case ZIP_LES_NONE:
	    e[0] = e[1] = 0;
	    break;

	case ZIP_LES_INVAL:
	    e[0] = ZIP_ER_INVAL;
	    e[1] = 0;
	    break;

	case ZIP_LES_LOWER:
	    zip_source_error(src->src, ze, se);
	    return;

	case ZIP_LES_UPPER:
	    if (src->cb.l(src->src, src->ud, e, sizeof(e), ZIP_SOURCE_ERROR) < 0) {
		e[0] = ZIP_ER_INTERNAL;
		e[1] = 0;
	    }
	    break;

	default:
	    e[0] = ZIP_ER_INTERNAL;
	    e[1] = 0;
	    break;
	}
    }

    if (ze)
	*ze = e[0];
    if (se)
	*se = e[1];
}
