/*
  $NiH: zip_unchange.c,v 1.19 2006/04/23 13:21:18 wiz Exp $

  zip_unchange.c -- undo changes to file in zip archive
  Copyright (C) 1999, 2004, 2006 Dieter Baron and Thomas Klausner

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



#include <stdlib.h>
#include "zip.h"
#include "zipint.h"



PHPZIPAPI int
zip_unchange(struct zip *za, int idx)
{
    return _zip_unchange(za, idx, 0);
}



PHPZIPAPI int
_zip_unchange(struct zip *za, int idx, int allow_duplicates)
{
    int i;
    
    if (idx < 0 || idx >= za->nentry) {
	_zip_error_set(&za->error, ZIP_ER_INVAL, 0);
	return -1;
    }

    if (za->entry[idx].ch_filename) {
	if (!allow_duplicates) {
	    i = _zip_name_locate(za,
			 _zip_get_name(za, idx, ZIP_FL_UNCHANGED, NULL),
				 0, NULL);
	    if (i != -1 && i != idx) {
		_zip_error_set(&za->error, ZIP_ER_EXISTS, 0);
		return -1;
	    }
	}

	free(za->entry[idx].ch_filename);
	za->entry[idx].ch_filename = NULL;
    }

    free(za->entry[idx].ch_comment);
    za->entry[idx].ch_comment = NULL;
    za->entry[idx].ch_comment_len = -1;

    _zip_unchange_data(za->entry+idx);

    return 0;
}
