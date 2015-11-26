/*
zip_source_win32a.c -- create data source from Windows file (ANSI)
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


#include <errno.h>
#include <stdio.h>

#include "zipint.h"
#include "zipwin32.h"

static void * _win32_strdup_a(const void *str);
static HANDLE _win32_open_a(_zip_source_win32_read_file_t *ctx);
static HANDLE _win32_create_temp_a(_zip_source_win32_read_file_t *ctx, void **temp, zip_uint32_t value, PSECURITY_ATTRIBUTES sa);
static int _win32_rename_temp_a(_zip_source_win32_read_file_t *ctx);
static int _win32_remove_a(const void *fname);

static _zip_source_win32_file_ops_t win32_ops_a = {
    _win32_strdup_a,
    _win32_open_a,
    _win32_create_temp_a,
    _win32_rename_temp_a,
    _win32_remove_a
};

ZIP_EXTERN zip_source_t *
zip_source_win32a(zip_t *za, const char *fname, zip_uint64_t start, zip_int64_t len)
{
    if (za == NULL)
	return NULL;

    return zip_source_win32a_create(fname, start, len, &za->error);
}


ZIP_EXTERN zip_source_t *
zip_source_win32a_create(const char *fname, zip_uint64_t start, zip_int64_t length, zip_error_t *error)
{
    if (fname == NULL || length < -1) {
	zip_error_set(error, ZIP_ER_INVAL, 0);
	return NULL;
    }

    return _zip_source_win32_handle_or_name(fname, INVALID_HANDLE_VALUE, start, length, 1, NULL, &win32_ops_a, error);
}


static void *
_win32_strdup_a(const void *str)
{
    return strdup((const char *)str);
}


static HANDLE
_win32_open_a(_zip_source_win32_read_file_t *ctx)
{
    return CreateFileA(ctx->fname, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}


static HANDLE
_win32_create_temp_a(_zip_source_win32_read_file_t *ctx, void **temp, zip_uint32_t value, PSECURITY_ATTRIBUTES sa)
{
    int len;

    len = strlen((const char *)ctx->fname) + 10;
    if (*temp == NULL) {
	if ((*temp = malloc(sizeof(char) * len)) == NULL) {
	    zip_error_set(&ctx->error, ZIP_ER_MEMORY, 0);
	    return INVALID_HANDLE_VALUE;
	}
    }
    if (sprintf((char *)*temp, "%s.%08x", (const char *)ctx->fname, value) != len - 1) {
	return INVALID_HANDLE_VALUE;
    }

    return CreateFileA((const char *)*temp, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, sa, CREATE_NEW, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_TEMPORARY, NULL);
}


static int
_win32_rename_temp_a(_zip_source_win32_read_file_t *ctx)
{
    if (!MoveFileExA(ctx->tmpname, ctx->fname, MOVEFILE_REPLACE_EXISTING))
	return -1;
    return 0;
}


static int
_win32_remove_a(const void *fname)
{
    DeleteFileA((const char *)fname);
    return 0;
}
