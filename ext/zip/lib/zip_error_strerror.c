/*
  zip_error_sterror.c -- get string representation of struct zip_error
  Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

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


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zipint.h"


ZIP_EXTERN const char *
zip_error_strerror(zip_error_t *err)
{
    const char *zs, *ss;
    char buf[128], *s;

    zip_error_fini(err);

    if (err->zip_err < 0 || err->zip_err >= _zip_nerr_str) {
	sprintf(buf, "Unknown error %d", err->zip_err);
	zs = NULL;
	ss = buf;
    }
    else {
	zs = _zip_err_str[err->zip_err];
	
	switch (_zip_err_type[err->zip_err]) {
	case ZIP_ET_SYS:
	    ss = strerror(err->sys_err);
	    break;

	case ZIP_ET_ZLIB:
	    ss = zError(err->sys_err);
	    break;

	default:
	    ss = NULL;
	}
    }

    if (ss == NULL)
	return zs;
    else {
	if ((s=(char *)malloc(strlen(ss)
			      + (zs ? strlen(zs)+2 : 0) + 1)) == NULL)
	    return _zip_err_str[ZIP_ER_MEMORY];
	
	sprintf(s, "%s%s%s",
		(zs ? zs : ""),
		(zs ? ": " : ""),
		ss);
	err->str = s;

	return s;
    }
}
