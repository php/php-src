/*
  zip_free.c -- free struct zip
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



/* _zip_free:
   frees the space allocated to a zipfile struct, and closes the
   corresponding file. */

void
_zip_free(struct zip *za)
{
    int i;

    if (za == NULL)
	return;

    if (za->zn)
	free(za->zn);

    if (za->zp)
	fclose(za->zp);

    _zip_cdir_free(za->cdir);

    if (za->entry) {
	for (i=0; i<za->nentry; i++) {
	    _zip_entry_free(za->entry+i);
	}
	free(za->entry);
    }

    for (i=0; i<za->nfile; i++) {
	if (za->file[i]->error.zip_err == ZIP_ER_OK) {
	    _zip_error_set(&za->file[i]->error, ZIP_ER_ZIPCLOSED, 0);
	    za->file[i]->za = NULL;
	}
    }

    free(za->file);
    
    free(za);

    return;
}
