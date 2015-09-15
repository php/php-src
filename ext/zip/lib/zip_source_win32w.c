/*
zip_source_win32w.c -- create data source from Windows file (UTF-16)
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

static void * _win32_strdup_w(const void *str);
static HANDLE _win32_open_w(_zip_source_win32_read_file_t *ctx);
static HANDLE _win32_create_temp_w(_zip_source_win32_read_file_t *ctx, void **temp, zip_uint32_t value, PSECURITY_ATTRIBUTES sa);
static int _win32_rename_temp_w(_zip_source_win32_read_file_t *ctx);
static int _win32_remove_w(const void *fname);

static _zip_source_win32_file_ops_t win32_ops_w = {
    _win32_strdup_w,
    _win32_open_w,
    _win32_create_temp_w,
    _win32_rename_temp_w,
    _win32_remove_w
};

ZIP_EXTERN zip_source_t *
zip_source_win32w(zip_t *za, const wchar_t *fname, zip_uint64_t start, zip_int64_t len)
{
    if (za == NULL)
	return NULL;

    return zip_source_win32w_create(fname, start, len, &za->error);
}


ZIP_EXTERN zip_source_t *
zip_source_win32w_create(const wchar_t *fname, zip_uint64_t start, zip_int64_t length, zip_error_t *error)
{
    if (fname == NULL || length < -1) {
	zip_error_set(error, ZIP_ER_INVAL, 0);
	return NULL;
    }

    return _zip_source_win32_handle_or_name(fname, INVALID_HANDLE_VALUE, start, length, 1, NULL, &win32_ops_w, error);
}


static void *
_win32_strdup_w(const void *str)
{
    return _wcsdup((const wchar_t *)str);
}


static HANDLE
_win32_open_w(_zip_source_win32_read_file_t *ctx)
{
    return CreateFileW(ctx->fname, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}


static HANDLE
_win32_create_temp_w(_zip_source_win32_read_file_t *ctx, void **temp, zip_uint32_t value, PSECURITY_ATTRIBUTES sa)
{
    int len;

    len = wcslen((const wchar_t *)ctx->fname) + 10;
    if (*temp == NULL) {
	if ((*temp = malloc(sizeof(wchar_t) * len)) == NULL) {
	    zip_error_set(&ctx->error, ZIP_ER_MEMORY, 0);
	    return INVALID_HANDLE_VALUE;
	}
    }
    if (swprintf((wchar_t *)*temp, len, L"%s.%08x", (const wchar_t *)ctx->fname, value) != len - 1) {
	return INVALID_HANDLE_VALUE;
    }

    return CreateFileW((const wchar_t *)*temp, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, sa, CREATE_NEW, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_TEMPORARY, NULL);
}


static int
_win32_rename_temp_w(_zip_source_win32_read_file_t *ctx)
{
    if (!MoveFileExW(ctx->tmpname, ctx->fname, MOVEFILE_REPLACE_EXISTING))
	return -1;
    return 0;
}


static int
_win32_remove_w(const void *fname)
{
    DeleteFileW((const wchar_t *)fname);
    return 0;
}
