/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Wez Furlong <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include "php_globals.h"
#include "php_network.h"
#include "php_open_temporary_file.h"
#include "ext/standard/file.h"
#include "ext/standard/flock_compat.h"
#include <stddef.h>
#include <fcntl.h>
#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#if HAVE_SYS_FILE_H
#include <sys/file.h>
#endif

#include "php_streams_int.h"

/* parse standard "fopen" modes into open() flags */
PHPAPI int php_stream_parse_fopen_modes(const char *mode, int *open_flags)
{
	int flags;

	switch (mode[0]) {
		case 'r':
			flags = 0;
			break;
		case 'w':
			flags = O_TRUNC|O_CREAT;
			break;
		case 'a':
			flags = O_CREAT|O_APPEND;
			break;
		case 'x':
			flags = O_CREAT|O_EXCL;
			break;
		default:
			/* unknown mode */
			return FAILURE;
	}

	if (strchr(mode, '+')) {
		flags |= O_RDWR;
	} else if (flags) {
		flags |= O_WRONLY;
	} else {
		flags |= O_RDONLY;
	}

#ifdef O_BINARY
	if (strchr(mode, 'b')) {
		flags |= O_BINARY;
	}
#endif
	
	*open_flags = flags;
	return SUCCESS;
}


/* {{{ php_stream_fopen */
PHPAPI php_stream *_php_stream_fopen(const char *filename, const char *mode, char **opened_path, int options STREAMS_DC TSRMLS_DC)
{
	char *realpath = NULL;
	struct stat st;
	int open_flags;
	int fd;
	php_stream *ret;

	if (FAILURE == php_stream_parse_fopen_modes(mode, &open_flags)) {
		if (options & REPORT_ERRORS) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "`%s' is not a valid mode for fopen", mode);
		}
		return NULL;
	}
	
	realpath = expand_filepath(filename, NULL TSRMLS_CC);

	fd = open(realpath, open_flags, 0666);

	if (fd != -1)	{
		/* sanity checks for include/require */
		if (options & STREAM_OPEN_FOR_INCLUDE && (fstat(fd, &st) == -1 || !S_ISREG(st.st_mode))) {
#ifdef PHP_WIN32
			/* skip the sanity check; fstat doesn't appear to work on
			 * UNC paths */
			if (!IS_UNC_PATH(filename, strlen(filename)))
#endif
				goto err;
		} 
	
		ret = php_stream_fopen_from_fd_rel(fd, mode);

		if (ret)	{
			if (opened_path)	{
				*opened_path = realpath;
				realpath = NULL;
			}
			if (realpath)
				efree(realpath);

			return ret;
		}
err:
		close(fd);
	}
	efree(realpath);
	return NULL;
}
/* }}} */

/* {{{ ------- STDIO stream implementation -------*/

typedef struct {
	FILE *file;
	int fd;					/* underlying file descriptor */
	int is_process_pipe;	/* use pclose instead of fclose */
	int is_pipe;			/* don't try and seek */
	int lock_flag;		/* stores the lock state */
	char *temp_file_name;	/* if non-null, this is the path to a temporary file that
							 * is to be deleted when the stream is closed */
#if HAVE_FLUSHIO
	char last_op;
#endif
} php_stdio_stream_data;

PHPAPI php_stream *_php_stream_fopen_temporary_file(const char *dir, const char *pfx, char **opened_path STREAMS_DC TSRMLS_DC)
{
	FILE *fp = php_open_temporary_file(dir, pfx, opened_path TSRMLS_CC);

	if (fp)	{
		php_stream *stream = php_stream_fopen_from_file_rel(fp, "r+b");
		if (stream) {
			return stream;
		}
		fclose(fp);

		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to allocate stream");

		return NULL;
	}
	return NULL;
}

PHPAPI php_stream *_php_stream_fopen_tmpfile(int dummy STREAMS_DC TSRMLS_DC)
{
	char *opened_path = NULL;
	FILE *fp = php_open_temporary_file(NULL, "php", &opened_path TSRMLS_CC);

	if (fp)	{
		php_stream *stream = php_stream_fopen_from_file_rel(fp, "r+b");
		if (stream) {
			php_stdio_stream_data *self = (php_stdio_stream_data*)stream->abstract;

			self->temp_file_name = opened_path;
			self->lock_flag = LOCK_UN;
			
			return stream;
		}
		fclose(fp);

		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to allocate stream");

		return NULL;
	}
	return NULL;
}

PHPAPI php_stream *_php_stream_fopen_from_fd(int fd, const char *mode STREAMS_DC TSRMLS_DC)
{
	php_stdio_stream_data *self;
	php_stream *stream;
	
	self = emalloc_rel_orig(sizeof(*self));
	self->file = NULL;
	self->is_pipe = 0;
	self->lock_flag = LOCK_UN;
	self->is_process_pipe = 0;
	self->temp_file_name = NULL;
	self->fd = fd;

#ifdef S_ISFIFO
	/* detect if this is a pipe */
	if (self->fd >= 0) {
		struct stat sb;
		self->is_pipe = (fstat(self->fd, &sb) == 0 && S_ISFIFO(sb.st_mode)) ? 1 : 0;
	}
#elif defined(PHP_WIN32)
	{
		long handle = _get_osfhandle(self->fd);
		DWORD in_buf_size, out_buf_size;

		if (handle != 0xFFFFFFFF) {
			self->is_pipe = GetNamedPipeInfo((HANDLE)handle, NULL, &out_buf_size, &in_buf_size, NULL);
		}
	}
#endif
	
	stream = php_stream_alloc_rel(&php_stream_stdio_ops, self, 0, mode);

	if (stream) {
		if (self->is_pipe) {
			stream->flags |= PHP_STREAM_FLAG_NO_SEEK;
		} else {
			stream->position = lseek(self->fd, 0, SEEK_CUR);
		}
	}

	return stream;
}

PHPAPI php_stream *_php_stream_fopen_from_file(FILE *file, const char *mode STREAMS_DC TSRMLS_DC)
{
	php_stdio_stream_data *self;
	php_stream *stream;
	
	self = emalloc_rel_orig(sizeof(*self));
	self->file = file;
	self->is_pipe = 0;
	self->lock_flag = LOCK_UN;
	self->is_process_pipe = 0;
	self->temp_file_name = NULL;
	self->fd = fileno(file);

#ifdef S_ISFIFO
	/* detect if this is a pipe */
	if (self->fd >= 0) {
		struct stat sb;
		self->is_pipe = (fstat(self->fd, &sb) == 0 && S_ISFIFO(sb.st_mode)) ? 1 : 0;
	}
#elif defined(PHP_WIN32)
	{
		long handle = _get_osfhandle(self->fd);
		DWORD in_buf_size, out_buf_size;

		if (handle != 0xFFFFFFFF) {
			self->is_pipe = GetNamedPipeInfo((HANDLE)handle, NULL, &out_buf_size, &in_buf_size, NULL);
		}
	}
#endif
	
	stream = php_stream_alloc_rel(&php_stream_stdio_ops, self, 0, mode);

	if (stream) {
		if (self->is_pipe) {
			stream->flags |= PHP_STREAM_FLAG_NO_SEEK;
		} else {
			stream->position = ftell(file);
		}
	}

	return stream;
}

PHPAPI php_stream *_php_stream_fopen_from_pipe(FILE *file, const char *mode STREAMS_DC TSRMLS_DC)
{
	php_stdio_stream_data *self;
	php_stream *stream;

	self = emalloc_rel_orig(sizeof(*self));
	self->file = file;
	self->is_pipe = 1;
	self->lock_flag = LOCK_UN;
	self->is_process_pipe = 1;
	self->fd = fileno(file);
	self->temp_file_name = NULL;

	stream = php_stream_alloc_rel(&php_stream_stdio_ops, self, 0, mode);
	stream->flags |= PHP_STREAM_FLAG_NO_SEEK;
	return stream;
}

#define PHP_STDIOP_GET_FD(anfd, data)	anfd = (data)->file ? fileno((data)->file) : (data)->fd

static size_t php_stdiop_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	php_stdio_stream_data *data = (php_stdio_stream_data*)stream->abstract;

	assert(data != NULL);

	if (data->fd >= 0) {
		int bytes_written = write(data->fd, buf, count);
		if (bytes_written < 0) return 0;
		return (size_t) bytes_written;
	} else {

#if HAVE_FLUSHIO
		if (!data->is_pipe && data->last_op == 'r') {
			fseek(data->file, 0, SEEK_CUR);
		}
		data->last_op = 'w';
#endif

		return fwrite(buf, 1, count, data->file);
	}
}

static size_t php_stdiop_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	php_stdio_stream_data *data = (php_stdio_stream_data*)stream->abstract;
	size_t ret;

	assert(data != NULL);

	if (data->fd >= 0) {
		ret = read(data->fd, buf, count);
		
		if (ret == 0 || (ret == -1 && errno != EWOULDBLOCK))
			stream->eof = 1;
				
	} else {
#if HAVE_FLUSHIO
		if (!data->is_pipe && data->last_op == 'w')
			fseek(data->file, 0, SEEK_CUR);
		data->last_op = 'r';
#endif

		ret = fread(buf, 1, count, data->file);

		if (feof(data->file))
			stream->eof = 1;
	}
	return ret;
}

static int php_stdiop_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	int ret;
	php_stdio_stream_data *data = (php_stdio_stream_data*)stream->abstract;

	assert(data != NULL);
	
	if (close_handle) {
		if (data->lock_flag != LOCK_UN) {
			php_stream_lock(stream, LOCK_UN);
		}
		if (data->file) {
			if (data->is_process_pipe) {
				errno = 0;
				ret = pclose(data->file);

#if HAVE_SYS_WAIT_H
				if (WIFEXITED(ret)) {
					ret = WEXITSTATUS(ret);
				}
#endif
			} else {
				ret = fclose(data->file);
				data->file = NULL;
			}
		} else if (data->fd != -1) {
			ret = close(data->fd);
			data->fd = -1;
		} else {
			return 0; /* everything should be closed already -> success */
		}
		if (data->temp_file_name) {
			unlink(data->temp_file_name);
			efree(data->temp_file_name);
			data->temp_file_name = NULL;
		}
	} else {
		ret = 0;
		data->file = NULL;
		data->fd = -1;
	}

	/* STDIO streams are never persistent! */
	efree(data);

	return ret;
}

static int php_stdiop_flush(php_stream *stream TSRMLS_DC)
{
	php_stdio_stream_data *data = (php_stdio_stream_data*)stream->abstract;

	assert(data != NULL);

	/*
	 * stdio buffers data in user land. By calling fflush(3), this
	 * data is send to the kernel using write(2). fsync'ing is
	 * something completely different.
	 */
	if (data->file) {
		return fflush(data->file);
	}
	return 0;
}

static int php_stdiop_seek(php_stream *stream, off_t offset, int whence, off_t *newoffset TSRMLS_DC)
{
	php_stdio_stream_data *data = (php_stdio_stream_data*)stream->abstract;
	int ret;

	assert(data != NULL);

	if (data->is_pipe) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot seek on a pipe");
		return -1;
	}

	if (data->fd >= 0) {
		off_t result;
		
		result = lseek(data->fd, offset, whence);
		if (result == (off_t)-1)
			return -1;

		*newoffset = result;
		return 0;
		
	} else {
		ret = fseek(data->file, offset, whence);
		*newoffset = ftell(data->file);
		return ret;
	}
}

static int php_stdiop_cast(php_stream *stream, int castas, void **ret TSRMLS_DC)
{
	int fd;
	php_stdio_stream_data *data = (php_stdio_stream_data*) stream->abstract;

	assert(data != NULL);
	
	/* as soon as someone touches the stdio layer, buffering may ensue,
	 * so we need to stop using the fd directly in that case */

	switch (castas)	{
		case PHP_STREAM_AS_STDIO:
			if (ret) {

				if (data->file == NULL) {
					/* we were opened as a plain file descriptor, so we
					 * need fdopen now */
					data->file = fdopen(data->fd, stream->mode);
				}
				
				*ret = data->file;
				data->fd = -1;
			}
			return SUCCESS;

		case PHP_STREAM_AS_FD:
			/* fetch the fileno rather than using data->fd, since we may
			 * have zeroed that member if someone requested the FILE*
			 * first (see above case) */
			PHP_STDIOP_GET_FD(fd, data);

			if (fd < 0) {
				return FAILURE;
			}
			if (data->file) {
				fflush(data->file);
			}
			if (ret) {
				*ret = (void*)fd;
			}
			return SUCCESS;
		default:
			return FAILURE;
	}
}

static int php_stdiop_stat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC)
{
	int fd;
	php_stdio_stream_data *data = (php_stdio_stream_data*) stream->abstract;

	assert(data != NULL);

	PHP_STDIOP_GET_FD(fd, data);
	
	return fstat(fd, &ssb->sb);
}

static int php_stdiop_set_option(php_stream *stream, int option, int value, void *ptrparam TSRMLS_DC)
{
	php_stdio_stream_data *data = (php_stdio_stream_data*) stream->abstract;
	size_t size;
	int fd;
#ifdef O_NONBLOCK
	/* FIXME: make this work for win32 */
	int flags;
	int oldval;
#endif
	
	PHP_STDIOP_GET_FD(fd, data);
	
	switch(option) {
		case PHP_STREAM_OPTION_BLOCKING:
			if (fd == -1)
				return -1;
#ifdef O_NONBLOCK
			flags = fcntl(fd, F_GETFL, 0);
			oldval = (flags & O_NONBLOCK) ? 0 : 1;
			if (value)
				flags ^= O_NONBLOCK;
			else
				flags |= O_NONBLOCK;
			
			if (-1 == fcntl(fd, F_SETFL, flags))
				return -1;
			return oldval;
#else
			return -1; /* not yet implemented */
#endif
			
		case PHP_STREAM_OPTION_WRITE_BUFFER:

			if (data->file == NULL) {
				return -1;
			}
			
			if (ptrparam)
				size = *(size_t *)ptrparam;
			else
				size = BUFSIZ;

			switch(value) {
				case PHP_STREAM_BUFFER_NONE:
					stream->flags |= PHP_STREAM_FLAG_NO_BUFFER;
					return setvbuf(data->file, NULL, _IONBF, 0);
					
				case PHP_STREAM_BUFFER_LINE:
					stream->flags ^= PHP_STREAM_FLAG_NO_BUFFER;
					return setvbuf(data->file, NULL, _IOLBF, size);
					
				case PHP_STREAM_BUFFER_FULL:
					stream->flags ^= PHP_STREAM_FLAG_NO_BUFFER;
					return setvbuf(data->file, NULL, _IOFBF, size);

				default:
					return -1;
			}
			break;
		
		case PHP_STREAM_OPTION_LOCKING:
			if (fd == -1) {
				return -1;
			}

			if ((int) ptrparam == PHP_STREAM_LOCK_SUPPORTED) {
				return 0;
			}

			if (!flock(fd, value) || (errno == EWOULDBLOCK && value & LOCK_NB)) {
				data->lock_flag = value;
				return 0;
			} else {
				return -1;
			}
			break;

		default:
			return -1;
	}
}

PHPAPI php_stream_ops	php_stream_stdio_ops = {
	php_stdiop_write, php_stdiop_read,
	php_stdiop_close, php_stdiop_flush,
	"STDIO",
	php_stdiop_seek,
	php_stdiop_cast,
	php_stdiop_stat,
	php_stdiop_set_option
};
/* }}} */

static size_t php_plain_files_dirstream_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	DIR *dir = (DIR*)stream->abstract;
	/* avoid libc5 readdir problems */
	char entry[sizeof(struct dirent)+MAXPATHLEN];
	struct dirent *result = (struct dirent *)&entry;
	php_stream_dirent *ent = (php_stream_dirent*)buf;

	/* avoid problems if someone mis-uses the stream */
	if (count != sizeof(php_stream_dirent))
		return 0;

	if (php_readdir_r(dir, (struct dirent *)entry, &result) == 0 && result) {
		PHP_STRLCPY(ent->d_name, result->d_name, sizeof(ent->d_name), strlen(result->d_name));
		return sizeof(php_stream_dirent);
	}
	return 0;
}

static int php_plain_files_dirstream_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	return closedir((DIR *)stream->abstract);
}

static int php_plain_files_dirstream_rewind(php_stream *stream, off_t offset, int whence, off_t *newoffs TSRMLS_DC)
{
	rewinddir((DIR *)stream->abstract);
	return 0;
}

static php_stream_ops	php_plain_files_dirstream_ops = {
	NULL, php_plain_files_dirstream_read,
	php_plain_files_dirstream_close, NULL,
	"dir",
	php_plain_files_dirstream_rewind,
	NULL, /* cast */
	NULL, /* stat */
	NULL  /* set_option */
};

static php_stream *php_plain_files_dir_opener(php_stream_wrapper *wrapper, char *path, char *mode,
		int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	DIR *dir = NULL;
	php_stream *stream = NULL;

	if (((options & STREAM_DISABLE_OPEN_BASEDIR) == 0) && php_check_open_basedir(path TSRMLS_CC)) {
		return NULL;
	}
	
	if (PG(safe_mode) &&(!php_checkuid(path, NULL, CHECKUID_ALLOW_ONLY_FILE))) {
		return NULL;
	}
	
	dir = VCWD_OPENDIR(path);

#ifdef PHP_WIN32
	if (dir && dir->finished) {
		closedir(dir);
		dir = NULL;
	}
#endif
	if (dir) {
		stream = php_stream_alloc(&php_plain_files_dirstream_ops, dir, 0, mode);
		if (stream == NULL)
			closedir(dir);
	}
		
	return stream;
}



static php_stream *php_plain_files_stream_opener(php_stream_wrapper *wrapper, char *path, char *mode,
		int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	if ((options & USE_PATH) && PG(include_path) != NULL) {
		return php_stream_fopen_with_path_rel(path, mode, PG(include_path), opened_path, options);
	}

	if (((options & STREAM_DISABLE_OPEN_BASEDIR) == 0) && php_check_open_basedir(path TSRMLS_CC)) {
		return NULL;
	}

	if ((options & ENFORCE_SAFE_MODE) && PG(safe_mode) && (!php_checkuid(path, mode, CHECKUID_CHECK_MODE_PARAM)))
		return NULL;

	return php_stream_fopen_rel(path, mode, opened_path, options);
}

static int php_plain_files_url_stater(php_stream_wrapper *wrapper, char *url, php_stream_statbuf *ssb TSRMLS_DC)
{
	return VCWD_STAT(url, &ssb->sb);
}

static php_stream_wrapper_ops php_plain_files_wrapper_ops = {
	php_plain_files_stream_opener,
	NULL,
	NULL,
	php_plain_files_url_stater,
	php_plain_files_dir_opener,
	"plainfile"
};

php_stream_wrapper php_plain_files_wrapper = {
	&php_plain_files_wrapper_ops,
	NULL,
	0
};

/* {{{ php_stream_fopen_with_path */
PHPAPI php_stream *_php_stream_fopen_with_path(char *filename, char *mode, char *path, char **opened_path, int options STREAMS_DC TSRMLS_DC)
{
	/* code ripped off from fopen_wrappers.c */
	char *pathbuf, *ptr, *end;
	char *exec_fname;
	char trypath[MAXPATHLEN];
	struct stat sb;
	php_stream *stream;
	int path_length;
	int filename_length;
	int exec_fname_length;

	if (opened_path) {
		*opened_path = NULL;
	}

	if(!filename) {
		return NULL;
	}

	filename_length = strlen(filename);

	/* Relative path open */
	if (*filename == '.' && (IS_SLASH(filename[1]) || filename[1] == '.')) {
		/* further checks, we could have ....... filenames */
		ptr = filename + 1;
		if (*ptr == '.') {
			while (*(++ptr) == '.');
			if (!IS_SLASH(*ptr)) { /* not a relative path after all */
				goto not_relative_path;
			}
		}


		if (((options & STREAM_DISABLE_OPEN_BASEDIR) == 0) && php_check_open_basedir(filename TSRMLS_CC)) {
			return NULL;
		}

		if (PG(safe_mode) && (!php_checkuid(filename, mode, CHECKUID_CHECK_MODE_PARAM))) {
			return NULL;
		}
		return php_stream_fopen_rel(filename, mode, opened_path, options);
	}

	/*
	 * files in safe_mode_include_dir (or subdir) are excluded from
	 * safe mode GID/UID checks
	 */

not_relative_path:

	/* Absolute path open */
	if (IS_ABSOLUTE_PATH(filename, filename_length)) {

		if (((options & STREAM_DISABLE_OPEN_BASEDIR) == 0) && php_check_open_basedir(filename TSRMLS_CC)) {
			return NULL;
		}

		if ((php_check_safe_mode_include_dir(filename TSRMLS_CC)) == 0)
			/* filename is in safe_mode_include_dir (or subdir) */
			return php_stream_fopen_rel(filename, mode, opened_path, options);

		if (PG(safe_mode) && (!php_checkuid(filename, mode, CHECKUID_CHECK_MODE_PARAM)))
			return NULL;

		return php_stream_fopen_rel(filename, mode, opened_path, options);
	}
	
#ifdef PHP_WIN32
	if (IS_SLASH(filename[0])) {
		int cwd_len;
		char *cwd;
		cwd = virtual_getcwd_ex(&cwd_len TSRMLS_CC);
		/* getcwd() will return always return [DRIVE_LETTER]:/) on windows. */
		*(cwd+3) = '\0';
	
		snprintf(trypath, MAXPATHLEN, "%s%s", cwd, filename);
		
		free(cwd);
		
		if (((options & STREAM_DISABLE_OPEN_BASEDIR) == 0) && php_check_open_basedir(trypath TSRMLS_CC)) {
			return NULL;
		}
		if ((php_check_safe_mode_include_dir(trypath TSRMLS_CC)) == 0) {
			return php_stream_fopen_rel(trypath, mode, opened_path, options);
		}	
		if (PG(safe_mode) && (!php_checkuid(trypath, mode, CHECKUID_CHECK_MODE_PARAM))) {
			return NULL;
		}
		
		return php_stream_fopen_rel(trypath, mode, opened_path, options);
	}
#endif

	if (!path || (path && !*path)) {

		if (((options & STREAM_DISABLE_OPEN_BASEDIR) == 0) && php_check_open_basedir(path TSRMLS_CC)) {
			return NULL;
		}

		if (PG(safe_mode) && (!php_checkuid(filename, mode, CHECKUID_CHECK_MODE_PARAM))) {
			return NULL;
		}
		return php_stream_fopen_rel(filename, mode, opened_path, options);
	}

	/* check in provided path */
	/* append the calling scripts' current working directory
	 * as a fall back case
	 */
	if (zend_is_executing(TSRMLS_C)) {
		exec_fname = zend_get_executed_filename(TSRMLS_C);
		exec_fname_length = strlen(exec_fname);
		path_length = strlen(path);

		while ((--exec_fname_length >= 0) && !IS_SLASH(exec_fname[exec_fname_length]));
		if ((exec_fname && exec_fname[0] == '[')
				|| exec_fname_length<=0) {
			/* [no active file] or no path */
			pathbuf = estrdup(path);
		} else {
			pathbuf = (char *) emalloc(exec_fname_length + path_length +1 +1);
			memcpy(pathbuf, path, path_length);
			pathbuf[path_length] = DEFAULT_DIR_SEPARATOR;
			memcpy(pathbuf+path_length+1, exec_fname, exec_fname_length);
			pathbuf[path_length + exec_fname_length +1] = '\0';
		}
	} else {
		pathbuf = estrdup(path);
	}

	ptr = pathbuf;

	while (ptr && *ptr) {
		end = strchr(ptr, DEFAULT_DIR_SEPARATOR);
		if (end != NULL) {
			*end = '\0';
			end++;
		}
		snprintf(trypath, MAXPATHLEN, "%s/%s", ptr, filename);
		
		if (((options & STREAM_DISABLE_OPEN_BASEDIR) == 0) && php_check_open_basedir(trypath TSRMLS_CC)) {
			stream = NULL;
			goto stream_done;
		}
		
		if (PG(safe_mode)) {
			if (VCWD_STAT(trypath, &sb) == 0) {
				/* file exists ... check permission */
				if ((php_check_safe_mode_include_dir(trypath TSRMLS_CC) == 0) ||
						php_checkuid(trypath, mode, CHECKUID_CHECK_MODE_PARAM)) {
					/* UID ok, or trypath is in safe_mode_include_dir */
					stream = php_stream_fopen_rel(trypath, mode, opened_path, options);
				} else {
					stream = NULL;
				}
				goto stream_done;
			}
		}
		stream = php_stream_fopen_rel(trypath, mode, opened_path, options);
		if (stream) {
stream_done:
			efree(pathbuf);
			return stream;
		}
		ptr = end;
	} /* end provided path */

	efree(pathbuf);
	return NULL;

}
/* }}} */




/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
