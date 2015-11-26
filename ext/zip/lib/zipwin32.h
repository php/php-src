#ifndef _HAD_ZIPWIN32_H
#define _HAD_ZIPWIN32_H

/*
  zipwin32.h -- internal declarations for Windows.
  Copyright (C) 1999-2015 Dieter Baron and Thomas Klausner

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

#include <windows.h>

/* context for Win32 source */

struct _zip_source_win32_file_ops;

struct _zip_source_win32_read_file {
    zip_error_t error;      /* last error information */
    zip_int64_t supports;

    /* operations */
    struct _zip_source_win32_file_ops *ops;

    /* reading */
    void *fname;            /* name of file to read from - ANSI (char *) or Unicode (wchar_t *) */
    void *h;                /* HANDLE for file to read from */
    int closep;             /* whether to close f on ZIP_CMD_FREE */
    struct zip_stat st;     /* stat information passed in */
    zip_uint64_t start;     /* start offset of data to read */
    zip_uint64_t end;       /* end offset of data to read, 0 for up to EOF */
    zip_uint64_t current;   /* current offset */

    /* writing */
    void *tmpname;          /* name of temp file - ANSI (char *) or Unicode (wchar_t *) */
    void *hout;             /* HANDLE for output file */
};

typedef struct _zip_source_win32_read_file _zip_source_win32_read_file_t;

/* internal operations for Win32 source */

struct _zip_source_win32_file_ops {
    void *(*op_strdup)(const void *);
    void *(*op_open)(_zip_source_win32_read_file_t *);
    void *(*op_create_temp)(_zip_source_win32_read_file_t *, void **, zip_uint32_t, PSECURITY_ATTRIBUTES);
    int (*op_rename_temp)(_zip_source_win32_read_file_t *);
    int (*op_remove)(const void *);
};

typedef struct _zip_source_win32_file_ops _zip_source_win32_file_ops_t;

zip_source_t *_zip_source_win32_handle_or_name(const void *, void *, zip_uint64_t, zip_int64_t, int, const zip_stat_t *, _zip_source_win32_file_ops_t *, zip_error_t *);

#endif /* zipwin32.h */
