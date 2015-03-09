/*
  zip_source_stat.c -- get meta information from zip_source
  Copyright (C) 2009 Dieter Baron and Thomas Klausner

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



int
zip_source_stat(struct zip_source *src, struct zip_stat *st)
{
    zip_int64_t ret;

    if (st == NULL) {
	src->error_source = ZIP_LES_INVAL;
	return -1;
    }

    if (src->src == NULL) {
	if (src->cb.f(src->ud, st, sizeof(*st), ZIP_SOURCE_STAT) < 0)
	    return -1;
	return 0;
    }

    if (zip_source_stat(src->src, st) < 0) {
	src->error_source = ZIP_LES_LOWER;
	return -1;
    }

    ret = src->cb.l(src->src, src->ud, st, sizeof(*st), ZIP_SOURCE_STAT);

    if (ret < 0) {
	if (ret == ZIP_SOURCE_ERR_LOWER)
	    src->error_source = ZIP_LES_LOWER;
	else
	    src->error_source = ZIP_LES_UPPER;
	return -1;
    }

    return 0;
}
