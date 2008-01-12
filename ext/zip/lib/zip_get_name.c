/*
  $NiH: zip_get_name.c,v 1.13 2005/01/20 21:00:54 dillo Exp $

  zip_get_name.c -- get filename for a file in zip file
  Copyright (C) 1999, 2003, 2004, 2005 Dieter Baron and Thomas Klausner

  This file is part of libzip, a library to manipulate ZIP archives.
  The authors can be contacted at <nih@giga.or.at>

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



#include "zip.h"
#include "zipint.h"



const char *
zip_get_name(struct zip *za, int idx, int flags)
{
    return _zip_get_name(za, idx, flags, &za->error);
}



const char *
_zip_get_name(struct zip *za, int idx, int flags, struct zip_error *error)
{
    if (idx < 0 || idx >= za->nentry) {
	_zip_error_set(error, ZIP_ER_INVAL, 0);
	return NULL;
    }

    if ((flags & ZIP_FL_UNCHANGED) == 0) {
	if (za->entry[idx].state == ZIP_ST_DELETED) {
	    _zip_error_set(error, ZIP_ER_DELETED, 0);
	    return NULL;
	}
	if (za->entry[idx].ch_filename)
	    return za->entry[idx].ch_filename;
    }

    if (za->cdir == NULL || idx >= za->cdir->nentry) {
	_zip_error_set(error, ZIP_ER_INVAL, 0);
	return NULL;
    }
    
    return za->cdir->entry[idx].filename;
}
