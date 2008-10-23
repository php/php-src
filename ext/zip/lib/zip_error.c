/*
  zip_error.c -- struct zip_error helper functions
  Copyright (C) 1999-2007 Dieter Baron and Thomas Klausner

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



#include <stdlib.h>

#include "zipint.h"



void
_zip_error_clear(struct zip_error *err)
{
    err->zip_err = ZIP_ER_OK;
    err->sys_err = 0;
}



void
_zip_error_copy(struct zip_error *dst, struct zip_error *src)
{
    dst->zip_err = src->zip_err;
    dst->sys_err = src->sys_err;
}



void
_zip_error_fini(struct zip_error *err)
{
    free(err->str);
    err->str = NULL;
}



void
_zip_error_get(struct zip_error *err, int *zep, int *sep)
{
    if (zep)
	*zep = err->zip_err;
    if (sep) {
	if (zip_error_get_sys_type(err->zip_err) != ZIP_ET_NONE)
	    *sep = err->sys_err;
	else
	    *sep = 0;
    }
}



void
_zip_error_init(struct zip_error *err)
{
    err->zip_err = ZIP_ER_OK;
    err->sys_err = 0;
    err->str = NULL;
}



void
_zip_error_set(struct zip_error *err, int ze, int se)
{
    if (err) {
	err->zip_err = ze;
	err->sys_err = se;
    }
}
