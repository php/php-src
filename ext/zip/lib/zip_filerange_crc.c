/*
  zip_filerange_crc.c -- compute CRC32 for a range of a file
  Copyright (C) 2008-2015 Dieter Baron and Thomas Klausner

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


#include <stdio.h>

#include "zipint.h"



int
_zip_filerange_crc(zip_source_t *src, zip_uint64_t start, zip_uint64_t len, uLong *crcp, zip_error_t *error)
{
    Bytef buf[BUFSIZE];
    zip_int64_t n;

    *crcp = crc32(0L, Z_NULL, 0);

    if (start > ZIP_INT64_MAX) {
	zip_error_set(error, ZIP_ER_SEEK, EFBIG);
	return -1;
    }

    if (zip_source_seek(src, (zip_int64_t)start, SEEK_SET) != 0) {
	_zip_error_set_from_source(error, src);
	return -1;
    }
    
    while (len > 0) {
	n = (zip_int64_t)(len > BUFSIZE ? BUFSIZE : len);
	if ((n = zip_source_read(src, buf, (zip_uint64_t)n)) < 0) {
	    _zip_error_set_from_source(error, src);
	    return -1;
	}
	if (n == 0) {
	    zip_error_set(error, ZIP_ER_EOF, 0);
	    return -1;
	}

	*crcp = crc32(*crcp, buf, (uInt)n);

	len -= (zip_uint64_t)n;
    }

    return 0;
}
