/*
zip_source_win32file.c -- create data source from HANDLE (Win32)
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


#include <wchar.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "zipint.h"
#include "zipwin32.h"

static zip_int64_t _win32_read_file(void *state, void *data, zip_uint64_t len, zip_source_cmd_t cmd);
static int _win32_create_temp_file(_zip_source_win32_read_file_t *ctx);
static int _zip_filetime_to_time_t(FILETIME ft, time_t *t);
static int _zip_seek_win32_u(void *h, zip_uint64_t offset, int whence, zip_error_t *error);
static int _zip_seek_win32(void *h, zip_int64_t offset, int whence, zip_error_t *error);
static int _zip_win32_error_to_errno(unsigned long win32err);
static int _zip_stat_win32(void *h, zip_stat_t *st, _zip_source_win32_read_file_t *ctx);

ZIP_EXTERN zip_source_t *
zip_source_win32handle(zip_t *za, HANDLE h, zip_uint64_t start, zip_int64_t len)
{
    if (za == NULL)
	return NULL;

    return zip_source_win32handle_create(h, start, len, &za->error);
}


ZIP_EXTERN zip_source_t *
zip_source_win32handle_create(HANDLE h, zip_uint64_t start, zip_int64_t length, zip_error_t *error)
{
    if (h == INVALID_HANDLE_VALUE || length < -1) {
	zip_error_set(error, ZIP_ER_INVAL, 0);
	return NULL;
    }

    return _zip_source_win32_handle_or_name(NULL, h, start, length, 1, NULL, NULL, error);
}


zip_source_t *
_zip_source_win32_handle_or_name(const void *fname, HANDLE h, zip_uint64_t start, zip_int64_t len, int closep, const zip_stat_t *st, _zip_source_win32_file_ops_t *ops, zip_error_t *error)
{
    _zip_source_win32_read_file_t *ctx;
    zip_source_t *zs;

    if (h == INVALID_HANDLE_VALUE && fname == NULL) {
	zip_error_set(error, ZIP_ER_INVAL, 0);
	return NULL;
    }

    if ((ctx = (_zip_source_win32_read_file_t *)malloc(sizeof(_zip_source_win32_read_file_t))) == NULL) {
	zip_error_set(error, ZIP_ER_MEMORY, 0);
	return NULL;
    }

    ctx->fname = NULL;
    if (fname) {
	if ((ctx->fname = ops->op_strdup(fname)) == NULL) {
	    zip_error_set(error, ZIP_ER_MEMORY, 0);
	    free(ctx);
	    return NULL;
	}
    }

    ctx->ops = ops;
    ctx->h = h;
    ctx->start = start;
    ctx->end = (len < 0 ? 0 : start + (zip_uint64_t)len);
    ctx->closep = ctx->fname ? 1 : closep;
    if (st) {
	memcpy(&ctx->st, st, sizeof(ctx->st));
	ctx->st.name = NULL;
	ctx->st.valid &= ~ZIP_STAT_NAME;
    }
    else {
	zip_stat_init(&ctx->st);
    }

    ctx->tmpname = NULL;
    ctx->hout = INVALID_HANDLE_VALUE;

    zip_error_init(&ctx->error);

    ctx->supports = ZIP_SOURCE_SUPPORTS_READABLE | zip_source_make_command_bitmap(ZIP_SOURCE_SUPPORTS, ZIP_SOURCE_TELL, -1);
    if (ctx->fname) {
	HANDLE th;

	th = ops->op_open(ctx);
	if (th == INVALID_HANDLE_VALUE || GetFileType(th) == FILE_TYPE_DISK) {
	    ctx->supports = ZIP_SOURCE_SUPPORTS_WRITABLE;
	}
	if (th != INVALID_HANDLE_VALUE) {
	    CloseHandle(th);
	}
    }
    else if (GetFileType(ctx->h) == FILE_TYPE_DISK) {
	ctx->supports = ZIP_SOURCE_SUPPORTS_SEEKABLE;
    }

    if ((zs = zip_source_function_create(_win32_read_file, ctx, error)) == NULL) {
	free(ctx->fname);
	free(ctx);
	return NULL;
    }

    return zs;
}


static zip_int64_t
_win32_read_file(void *state, void *data, zip_uint64_t len, zip_source_cmd_t cmd)
{
    _zip_source_win32_read_file_t *ctx;
    char *buf;
    zip_uint64_t n;
    DWORD i;

    ctx = (_zip_source_win32_read_file_t *)state;
    buf = (char *)data;

    switch (cmd) {
    case ZIP_SOURCE_BEGIN_WRITE:
	if (ctx->fname == NULL) {
	    zip_error_set(&ctx->error, ZIP_ER_OPNOTSUPP, 0);
	    return -1;
	}
	return _win32_create_temp_file(ctx);

    case ZIP_SOURCE_COMMIT_WRITE: {
	if (!CloseHandle(ctx->hout)) {
	    ctx->hout = INVALID_HANDLE_VALUE;
	    zip_error_set(&ctx->error, ZIP_ER_WRITE, _zip_win32_error_to_errno(GetLastError()));
	}
	ctx->hout = INVALID_HANDLE_VALUE;
	if (ctx->ops->op_rename_temp(ctx) < 0) {
	    zip_error_set(&ctx->error, ZIP_ER_RENAME, _zip_win32_error_to_errno(GetLastError()));
	    return -1;
	}
	free(ctx->tmpname);
	ctx->tmpname = NULL;
	return 0;
    }

    case ZIP_SOURCE_CLOSE:
	if (ctx->fname) {
	    CloseHandle(ctx->h);
	    ctx->h = INVALID_HANDLE_VALUE;
	}
	return 0;

    case ZIP_SOURCE_ERROR:
	return zip_error_to_data(&ctx->error, data, len);

    case ZIP_SOURCE_FREE:
	free(ctx->fname);
	free(ctx->tmpname);
	if (ctx->closep && ctx->h != INVALID_HANDLE_VALUE)
	    CloseHandle(ctx->h);
	free(ctx);
	return 0;

    case ZIP_SOURCE_OPEN:
	if (ctx->fname) {
	    if ((ctx->h = ctx->ops->op_open(ctx)) == INVALID_HANDLE_VALUE) {
		zip_error_set(&ctx->error, ZIP_ER_OPEN, _zip_win32_error_to_errno(GetLastError()));
		return -1;
	    }
	}

	if (ctx->closep && ctx->start > 0) {
	    if (_zip_seek_win32_u(ctx->h, ctx->start, SEEK_SET, &ctx->error) < 0) {
		return -1;
	    }
	}
	ctx->current = ctx->start;
	return 0;

    case ZIP_SOURCE_READ:
	if (ctx->end > 0) {
	    n = ctx->end - ctx->current;
	    if (n > len) {
		n = len;
	    }
	}
	else {
	    n = len;
	}

	if (n > SIZE_MAX)
	    n = SIZE_MAX;

	if (!ctx->closep) {
	    if (_zip_seek_win32_u(ctx->h, ctx->current, SEEK_SET, &ctx->error) < 0) {
		return -1;
	    }
	}

	if (!ReadFile(ctx->h, buf, (DWORD)n, &i, NULL)) {
	    zip_error_set(&ctx->error, ZIP_ER_READ, _zip_win32_error_to_errno(GetLastError()));
	    return -1;
	}
	ctx->current += i;

	return (zip_int64_t)i;

    case ZIP_SOURCE_REMOVE:
	if (ctx->ops->op_remove(ctx->fname) < 0) {
	    zip_error_set(&ctx->error, ZIP_ER_REMOVE, _zip_win32_error_to_errno(GetLastError()));
	    return -1;
	}
	return 0;

    case ZIP_SOURCE_ROLLBACK_WRITE:
	if (ctx->hout) {
	    CloseHandle(ctx->hout);
	    ctx->hout = INVALID_HANDLE_VALUE;
	}
	ctx->ops->op_remove(ctx->tmpname);
	free(ctx->tmpname);
	ctx->tmpname = NULL;
	return 0;

    case ZIP_SOURCE_SEEK: {
	zip_int64_t new_current;
	int need_seek;
	zip_source_args_seek_t *args = ZIP_SOURCE_GET_ARGS(zip_source_args_seek_t, data, len, &ctx->error);

	if (args == NULL)
	    return -1;

	need_seek = ctx->closep;

	switch (args->whence) {
	case SEEK_SET:
	    new_current = args->offset;
	    break;

	case SEEK_END:
	    if (ctx->end == 0) {
		LARGE_INTEGER zero;
		LARGE_INTEGER new_offset;

		if (_zip_seek_win32(ctx->h, args->offset, SEEK_END, &ctx->error) < 0) {
		    return -1;
		}
		zero.QuadPart = 0;
		if (!SetFilePointerEx(ctx->h, zero, &new_offset, FILE_CURRENT)) {
		    zip_error_set(&ctx->error, ZIP_ER_SEEK, _zip_win32_error_to_errno(GetLastError()));
		    return -1;
		}
		new_current = new_offset.QuadPart;
		need_seek = 0;
	    }
	    else {
		new_current = (zip_int64_t)ctx->end + args->offset;
	    }
	    break;
	case SEEK_CUR:
	    new_current = (zip_int64_t)ctx->current + args->offset;
	    break;

	default:
	    zip_error_set(&ctx->error, ZIP_ER_INVAL, 0);
	    return -1;
	}

	if (new_current < 0 || (zip_uint64_t)new_current < ctx->start || (ctx->end != 0 && (zip_uint64_t)new_current > ctx->end)) {
	    zip_error_set(&ctx->error, ZIP_ER_INVAL, 0);
	    return -1;
	}

	ctx->current = (zip_uint64_t)new_current;

	if (need_seek) {
	    if (_zip_seek_win32_u(ctx->h, ctx->current, SEEK_SET, &ctx->error) < 0) {
		return -1;
	    }
	}
	return 0;
    }

    case ZIP_SOURCE_SEEK_WRITE: {
	zip_source_args_seek_t *args;

	args = ZIP_SOURCE_GET_ARGS(zip_source_args_seek_t, data, len, &ctx->error);
	if (args == NULL) {
	    return -1;
	}

	if (_zip_seek_win32(ctx->hout, args->offset, args->whence, &ctx->error) < 0) {
	    return -1;
	}
	return 0;
    }

    case ZIP_SOURCE_STAT: {
	if (len < sizeof(ctx->st))
	    return -1;

	if (ctx->st.valid != 0)
	    memcpy(data, &ctx->st, sizeof(ctx->st));
	else {
	    DWORD win32err;
	    zip_stat_t *st;
	    HANDLE h;
	    int success;

	    st = (zip_stat_t *)data;

	    if (ctx->h != INVALID_HANDLE_VALUE) {
		h = ctx->h;
	    }
	    else {
		h = ctx->ops->op_open(ctx);
		if (h == INVALID_HANDLE_VALUE && GetLastError() == ERROR_FILE_NOT_FOUND) {
		    zip_error_set(&ctx->error, ZIP_ER_READ, ENOENT);
		    return -1;
		}
	    }

	    success = _zip_stat_win32(h, st, ctx);
	    win32err = GetLastError();

	    /* We're done with the handle, so close it if we just opened it. */
	    if (h != ctx->h) {
		CloseHandle(h);
	    }

	    if (success < 0) {
		/* TODO: Is this the correct error to return in all cases? */
		zip_error_set(&ctx->error, ZIP_ER_READ, _zip_win32_error_to_errno(win32err));
		return -1;
	    }
	}
	return sizeof(ctx->st);
    }

    case ZIP_SOURCE_SUPPORTS:
	return ctx->supports;

    case ZIP_SOURCE_TELL:
	return (zip_int64_t)ctx->current;

    case ZIP_SOURCE_TELL_WRITE:
    {
	LARGE_INTEGER zero;
	LARGE_INTEGER offset;

	zero.QuadPart = 0;
	if (!SetFilePointerEx(ctx->hout, zero, &offset, FILE_CURRENT)) {
	    zip_error_set(&ctx->error, ZIP_ER_TELL, _zip_win32_error_to_errno(GetLastError()));
	    return -1;
	}

	return offset.QuadPart;
    }

    case ZIP_SOURCE_WRITE:
    {
	DWORD ret;
	if (!WriteFile(ctx->hout, data, (DWORD)len, &ret, NULL) || ret != len) {
	    zip_error_set(&ctx->error, ZIP_ER_WRITE, _zip_win32_error_to_errno(GetLastError()));
	    return -1;
	}

	return (zip_int64_t)ret;
    }

    default:
	zip_error_set(&ctx->error, ZIP_ER_OPNOTSUPP, 0);
	return -1;
    }
}


static int
_win32_create_temp_file(_zip_source_win32_read_file_t *ctx)
{
    /*
    Windows has GetTempFileName(), but it closes the file after
    creation, leaving it open to a horrible race condition. So
    we reinvent the wheel.
    */
    int i;
    HANDLE th = INVALID_HANDLE_VALUE;
    void *temp = NULL;
    SECURITY_INFORMATION si;
    SECURITY_ATTRIBUTES sa;
    PSECURITY_DESCRIPTOR psd = NULL;
    PSECURITY_ATTRIBUTES psa = NULL;
    DWORD len;
    BOOL success;
	zip_uint32_t value;

    /*
    Read the DACL from the original file, so we can copy it to the temp file.
    If there is no original file, or if we can't read the DACL, we'll use the
    default security descriptor.
    */
    if (ctx->h != INVALID_HANDLE_VALUE && GetFileType(ctx->h) == FILE_TYPE_DISK) {
	si = DACL_SECURITY_INFORMATION | UNPROTECTED_DACL_SECURITY_INFORMATION;
	len = 0;
	success = GetUserObjectSecurity(ctx->h, &si, NULL, len, &len);
	if (!success && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
	    if ((psd = (PSECURITY_DESCRIPTOR)malloc(len)) == NULL) {
		zip_error_set(&ctx->error, ZIP_ER_MEMORY, 0);
		return -1;
	    }
	    success = GetUserObjectSecurity(ctx->h, &si, psd, len, &len);
	}
	if (success) {
	    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	    sa.bInheritHandle = FALSE;
	    sa.lpSecurityDescriptor = psd;
	    psa = &sa;
	}
    }

    value = GetTickCount();
    for (i = 0; i < 1024 && th == INVALID_HANDLE_VALUE; i++) {
	th = ctx->ops->op_create_temp(ctx, &temp, value + i, psa);
	if (th == INVALID_HANDLE_VALUE && GetLastError() != ERROR_FILE_EXISTS)
	    break;
    }

    if (th == INVALID_HANDLE_VALUE) {
	free(temp);
	free(psd);
	zip_error_set(&ctx->error, ZIP_ER_TMPOPEN, _zip_win32_error_to_errno(GetLastError()));
	return -1;
    }

    free(psd);
    ctx->hout = th;
    ctx->tmpname = temp;

    return 0;
}


static int
_zip_seek_win32_u(HANDLE h, zip_uint64_t offset, int whence, zip_error_t *error)
{
    if (offset > ZIP_INT64_MAX) {
	zip_error_set(error, ZIP_ER_SEEK, EOVERFLOW);
	return -1;
    }
    return _zip_seek_win32(h, (zip_int64_t)offset, whence, error);
}


static int
_zip_seek_win32(HANDLE h, zip_int64_t offset, int whence, zip_error_t *error)
{
    LARGE_INTEGER li;
    DWORD method;

    switch (whence) {
    case SEEK_SET:
	method = FILE_BEGIN;
	break;
    case SEEK_END:
	method = FILE_END;
	break;
    case SEEK_CUR:
	method = FILE_CURRENT;
	break;
    default:
	zip_error_set(error, ZIP_ER_SEEK, EINVAL);
	return -1;
    }

    li.QuadPart = (LONGLONG)offset;
    if (!SetFilePointerEx(h, li, NULL, method)) {
	zip_error_set(error, ZIP_ER_SEEK, _zip_win32_error_to_errno(GetLastError()));
	return -1;
    }

    return 0;
}


static int
_zip_win32_error_to_errno(DWORD win32err)
{
    /*
    Note: This list isn't exhaustive, but should cover common cases.
    */
    switch (win32err) {
    case ERROR_INVALID_PARAMETER:
	return EINVAL;
    case ERROR_FILE_NOT_FOUND:
	return ENOENT;
    case ERROR_INVALID_HANDLE:
	return EBADF;
    case ERROR_ACCESS_DENIED:
	return EACCES;
    case ERROR_FILE_EXISTS:
	return EEXIST;
    case ERROR_TOO_MANY_OPEN_FILES:
	return EMFILE;
    case ERROR_DISK_FULL:
	return ENOSPC;
    default:
	return 0;
    }
}


static int
_zip_stat_win32(HANDLE h, zip_stat_t *st, _zip_source_win32_read_file_t *ctx)
{
    FILETIME mtimeft;
    time_t mtime;
    LARGE_INTEGER size;
    int regularp;

    if (!GetFileTime(h, NULL, NULL, &mtimeft)) {
	zip_error_set(&ctx->error, ZIP_ER_READ, _zip_win32_error_to_errno(GetLastError()));
	return -1;
    }
    if (_zip_filetime_to_time_t(mtimeft, &mtime) < 0) {
	zip_error_set(&ctx->error, ZIP_ER_READ, ERANGE);
	return -1;
    }

    regularp = 0;
    if (GetFileType(h) == FILE_TYPE_DISK) {
	regularp = 1;
    }

    if (!GetFileSizeEx(h, &size)) {
	zip_error_set(&ctx->error, ZIP_ER_READ, _zip_win32_error_to_errno(GetLastError()));
	return -1;
    }

    zip_stat_init(st);
    st->mtime = mtime;
    st->valid |= ZIP_STAT_MTIME;
    if (ctx->end != 0) {
	st->size = ctx->end - ctx->start;
	st->valid |= ZIP_STAT_SIZE;
    }
    else if (regularp) {
	st->size = (zip_uint64_t)size.QuadPart;
	st->valid |= ZIP_STAT_SIZE;
    }

    return 0;
}


static int
_zip_filetime_to_time_t(FILETIME ft, time_t *t)
{
    /*
    Inspired by http://stackoverflow.com/questions/6161776/convert-windows-filetime-to-second-in-unix-linux
    */
    const zip_int64_t WINDOWS_TICK = 10000000LL;
    const zip_int64_t SEC_TO_UNIX_EPOCH = 11644473600LL;
    ULARGE_INTEGER li;
    zip_int64_t secs;
    time_t temp;

    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
    secs = (li.QuadPart / WINDOWS_TICK - SEC_TO_UNIX_EPOCH);

    temp = (time_t)secs;
    if (secs != (zip_int64_t)temp)
	return -1;

    *t = temp;
    return 0;
}
