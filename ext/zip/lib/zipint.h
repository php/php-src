#ifndef _HAD_ZIPINT_H
#define _HAD_ZIPINT_H

/*
  zipint.h -- internal declarations.
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

#ifdef PHP_WIN32
# include "php_zip_config.w32.h"
#else
# include "config.h"
#endif

/* to have *_MAX definitions for all types when compiling with g++ */
#define __STDC_LIMIT_MACROS

#include <zlib.h>

#ifdef PHP_WIN32
/* for dup(), close(), etc. */
#include <io.h>
#include "config.w32.h"
#endif

#ifndef _ZIP_COMPILING_DEPRECATED
#define ZIP_DISABLE_DEPRECATED
#endif

#include "zip.h"

#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#else
typedef char bool;
#define true    1
#define false   0
#endif

#ifdef _WIN32
#if defined(HAVE__CLOSE)
#define close		_close
#endif
#if defined(HAVE__DUP)
#define dup		_dup
#endif
/* crashes reported when using fdopen instead of _fdopen on Windows/Visual Studio 10/Win64 */
#if defined(HAVE__FDOPEN)
#define fdopen		_fdopen
#endif
#if defined(HAVE__FILENO)
#define fileno		_fileno
#endif
/* Windows' open() doesn't understand Unix permissions */
#if defined(HAVE__OPEN)
#define open(a, b, c)	_open((a), (b))
#endif
#if defined(HAVE__SNPRINTF)
#define snprintf	_snprintf
#endif
#if defined(HAVE__STRDUP)
#if !defined(HAVE_STRDUP) || defined(_WIN32)
#undef strdup
#define strdup		_strdup
#endif
#endif
#endif

#ifndef HAVE_FSEEKO
#define fseeko(s, o, w)	(fseek((s), (long int)(o), (w)))
#endif

#ifndef HAVE_FTELLO
#define ftello(s)	((long)ftell((s)))
#endif

#ifndef HAVE_MKSTEMP
int _zip_mkstemp(char *);
#define mkstemp _zip_mkstemp
#endif

#if !defined(HAVE_STRCASECMP)
#if defined(HAVE__STRICMP)
#define strcasecmp	_stricmp
#endif
#endif

#if SIZEOF_OFF_T == 8
#define ZIP_OFF_MAX ZIP_INT64_MAX
#define ZIP_OFF_MIN ZIP_INT64_MIN
#elif SIZEOF_OFF_T == 4
#define ZIP_OFF_MAX ZIP_INT32_MAX
#define ZIP_OFF_MIN ZIP_INT32_MIN
#elif SIZEOF_OFF_T == 2
#define ZIP_OFF_MAX ZIP_INT16_MAX
#define ZIP_OFF_MIN ZIP_INT16_MIN
#else
#error unsupported size of off_t
#endif

#if defined(HAVE_FTELLO) && defined(HAVE_FSEEKO)
#define ZIP_FSEEK_MAX ZIP_OFF_MAX
#define ZIP_FSEEK_MIN ZIP_OFF_MIN
#elif SIZEOF_LONG >= 8
#define ZIP_FSEEK_MAX (long)ZIP_INT64_MAX
#define ZIP_FSEEK_MIN (long)ZIP_INT64_MIN
#else
#define ZIP_FSEEK_MAX (long)ZIP_INT32_MAX
#define ZIP_FSEEK_MIN (long)ZIP_INT32_MIN
#endif

#ifndef SIZE_MAX
#if SIZEOF_SIZE_T == 8
#define SIZE_MAX ZIP_INT64_MAX
#elif SIZEOF_SIZE_T == 4
#define SIZE_MAX ZIP_INT32_MAX
#elif SIZEOF_SIZE_T == 2
#define SIZE_MAX ZIP_INT16_MAX
#else
#error unsupported size of size_t
#endif
#endif

#define CENTRAL_MAGIC "PK\1\2"
#define LOCAL_MAGIC   "PK\3\4"
#define EOCD_MAGIC    "PK\5\6"
#define DATADES_MAGIC "PK\7\8"
#define EOCD64LOC_MAGIC "PK\6\7"
#define EOCD64_MAGIC  "PK\6\6"
#define CDENTRYSIZE         46u
#define LENTRYSIZE          30
#define MAXCOMLEN        65536
#define MAXEXTLEN        65536
#define EOCDLEN             22
#define EOCD64LOCLEN	    20
#define EOCD64LEN	    56
#define CDBUFSIZE       (MAXCOMLEN+EOCDLEN+EOCD64LOCLEN)
#define BUFSIZE		8192
#define EFZIP64SIZE 28

#define ZIP_CM_REPLACED_DEFAULT	(-2)

#define ZIP_CM_IS_DEFAULT(x)	((x) == ZIP_CM_DEFAULT || (x) == ZIP_CM_REPLACED_DEFAULT)

#define ZIP_EF_UTF_8_COMMENT	0x6375
#define ZIP_EF_UTF_8_NAME	0x7075
#define ZIP_EF_ZIP64		0x0001

#define ZIP_EF_IS_INTERNAL(id)	((id) == ZIP_EF_UTF_8_COMMENT || (id) == ZIP_EF_UTF_8_NAME || (id) == ZIP_EF_ZIP64)

/* according to unzip-6.0's zipinfo.c, this corresponds to a regular file with rw permissions for everyone */
#define ZIP_EXT_ATTRIB_DEFAULT		(0100666u<<16)
/* according to unzip-6.0's zipinfo.c, this corresponds to a directory with rwx permissions for everyone */
#define ZIP_EXT_ATTRIB_DEFAULT_DIR	(0040777u<<16)


#define ZIP_MAX(a, b)		((a) > (b) ? (a) : (b))
#define ZIP_MIN(a, b)		((a) < (b) ? (a) : (b))

/* This section contains API that won't materialize like this.  It's
   placed in the internal section, pending cleanup. */

/* flags for compression and encryption sources */

#define ZIP_CODEC_DECODE	0 /* decompress/decrypt (encode flag not set) */
#define ZIP_CODEC_ENCODE	1 /* compress/encrypt */


typedef zip_source_t *(*zip_compression_implementation)(zip_t *, zip_source_t *, zip_int32_t, int);
typedef zip_source_t *(*zip_encryption_implementation)(zip_t *, zip_source_t *, zip_uint16_t, int, const char *);

zip_compression_implementation _zip_get_compression_implementation(zip_int32_t);
zip_encryption_implementation _zip_get_encryption_implementation(zip_uint16_t);



/* This API is not final yet, but we need it internally, so it's private for now. */

const zip_uint8_t *zip_get_extra_field_by_id(zip_t *, int, int, zip_uint16_t, int, zip_uint16_t *);

/* This section contains API that is of limited use until support for
   user-supplied compression/encryption implementation is finished.
   Thus we will keep it private for now. */

typedef zip_int64_t (*zip_source_layered_callback)(zip_source_t *, void *, void *, zip_uint64_t, enum zip_source_cmd);
zip_source_t *zip_source_crc(zip_t *, zip_source_t *, int);
zip_source_t *zip_source_deflate(zip_t *, zip_source_t *, zip_int32_t, int);
zip_source_t *zip_source_layered(zip_t *, zip_source_t *, zip_source_layered_callback, void *);
zip_source_t *zip_source_layered_create(zip_source_t *src, zip_source_layered_callback cb, void *ud, zip_error_t *error);
zip_source_t *zip_source_pkware(zip_t *, zip_source_t *, zip_uint16_t, int, const char *);
int zip_source_remove(zip_source_t *);
zip_int64_t zip_source_supports(zip_source_t *src);
zip_source_t *zip_source_window(zip_t *, zip_source_t *, zip_uint64_t, zip_uint64_t);


/* error source for layered sources */

enum zip_les { ZIP_LES_NONE, ZIP_LES_UPPER, ZIP_LES_LOWER, ZIP_LES_INVAL };

/* directory entry: general purpose bit flags */

#define ZIP_GPBF_ENCRYPTED		0x0001	/* is encrypted */
#define ZIP_GPBF_DATA_DESCRIPTOR	0x0008	/* crc/size after file data */
#define ZIP_GPBF_STRONG_ENCRYPTION	0x0040  /* uses strong encryption */
#define ZIP_GPBF_ENCODING_UTF_8		0x0800  /* file name encoding is UTF-8 */


/* extra fields */
#define ZIP_EF_LOCAL		ZIP_FL_LOCAL			/* include in local header */
#define ZIP_EF_CENTRAL		ZIP_FL_CENTRAL			/* include in central directory */
#define ZIP_EF_BOTH		(ZIP_EF_LOCAL|ZIP_EF_CENTRAL)	/* include in both */

#define ZIP_FL_FORCE_ZIP64	1024  /* force zip64 extra field (_zip_dirent_write) */

#define ZIP_FL_ENCODING_ALL	(ZIP_FL_ENC_GUESS|ZIP_FL_ENC_CP437|ZIP_FL_ENC_UTF_8)


/* encoding type */
enum zip_encoding_type {
    ZIP_ENCODING_UNKNOWN,       /* not yet analyzed */
    ZIP_ENCODING_ASCII,         /* plain ASCII */
    ZIP_ENCODING_UTF8_KNOWN,    /* is UTF-8 */
    ZIP_ENCODING_UTF8_GUESSED,  /* possibly UTF-8 */
    ZIP_ENCODING_CP437,         /* Code Page 437 */
    ZIP_ENCODING_ERROR          /* should be UTF-8 but isn't */
};

typedef enum zip_encoding_type zip_encoding_type_t;

typedef struct zip_cdir zip_cdir_t;
typedef struct zip_dirent zip_dirent_t;
typedef struct zip_entry zip_entry_t;
typedef struct zip_extra_field zip_extra_field_t;
typedef struct zip_string zip_string_t;
typedef struct zip_buffer zip_buffer_t;


/* zip archive, part of API */

struct zip {
    zip_source_t *src;                  /* data source for archive */
    unsigned int open_flags;		/* flags passed to zip_open */
    zip_error_t error;                  /* error information */

    unsigned int flags;			/* archive global flags */
    unsigned int ch_flags;		/* changed archive global flags */

    char *default_password;		/* password used when no other supplied */

    zip_string_t *comment_orig;         /* archive comment */
    zip_string_t *comment_changes;  /* changed archive comment */
    bool comment_changed;		/* whether archive comment was changed */

    zip_uint64_t nentry;		/* number of entries */
    zip_uint64_t nentry_alloc;		/* number of entries allocated */
    zip_entry_t *entry;                 /* entries */

    unsigned int nopen_source;		/* number of open sources using archive */
    unsigned int nopen_source_alloc;	/* number of sources allocated */
    zip_source_t **open_source;         /* open sources using archive */
    
    char *tempdir;                      /* custom temp dir (needed e.g. for OS X sandboxing) */
};

/* file in zip archive, part of API */

struct zip_file {
    zip_t *za;		/* zip archive containing this file */
    zip_error_t error;	/* error information */
    bool eof;
    zip_source_t *src;	/* data source */
};

/* zip archive directory entry (central or local) */

#define ZIP_DIRENT_COMP_METHOD	0x0001u
#define ZIP_DIRENT_FILENAME	0x0002u
#define ZIP_DIRENT_COMMENT	0x0004u
#define ZIP_DIRENT_EXTRA_FIELD	0x0008u
#define ZIP_DIRENT_ATTRIBUTES	0x0010u
#define ZIP_DIRENT_LAST_MOD	0x0020u
#define ZIP_DIRENT_ALL		0xffffu

struct zip_dirent {
    zip_uint32_t changed;
    bool local_extra_fields_read;	/*      whether we already read in local header extra fields */
    bool cloned;                         /*      whether this instance is cloned, and thus shares non-changed strings */

    zip_uint16_t version_madeby;	/* (c)  version of creator */
    zip_uint16_t version_needed;	/* (cl) version needed to extract */
    zip_uint16_t bitflags;		/* (cl) general purpose bit flag */
    zip_int32_t comp_method;		/* (cl) compression method used (uint16 and ZIP_CM_DEFAULT (-1)) */
    time_t last_mod;			/* (cl) time of last modification */
    zip_uint32_t crc;			/* (cl) CRC-32 of uncompressed data */
    zip_uint64_t comp_size;		/* (cl) size of compressed data */
    zip_uint64_t uncomp_size;		/* (cl) size of uncompressed data */
    zip_string_t *filename;		/* (cl) file name (NUL-terminated) */
    zip_extra_field_t *extra_fields;	/* (cl) extra fields, parsed */
    zip_string_t *comment;		/* (c)  file comment */
    zip_uint32_t disk_number;		/* (c)  disk number start */
    zip_uint16_t int_attrib;		/* (c)  internal file attributes */
    zip_uint32_t ext_attrib;		/* (c)  external file attributes */
    zip_uint64_t offset;		/* (c)  offset of local header */
};

/* zip archive central directory */

struct zip_cdir {
    zip_entry_t *entry;	 		/* directory entries */
    zip_uint64_t nentry;		/* number of entries */
    zip_uint64_t nentry_alloc;		/* number of entries allocated */

    zip_uint64_t size;                  /* size of central directory */
    zip_uint64_t offset;		/* offset of central directory in file */
    zip_string_t *comment;		/* zip archive comment */
};

struct zip_extra_field {
    zip_extra_field_t *next;
    zip_flags_t flags;			/* in local/central header */
    zip_uint16_t id;			/* header id */
    zip_uint16_t size;			/* data size */
    zip_uint8_t *data;
};

enum zip_source_write_state {
    ZIP_SOURCE_WRITE_CLOSED,    /* write is not in progress */
    ZIP_SOURCE_WRITE_OPEN,      /* write is in progress */
    ZIP_SOURCE_WRITE_FAILED,    /* commit failed, only rollback allowed */
    ZIP_SOURCE_WRITE_REMOVED    /* file was removed */
};
typedef enum zip_source_write_state zip_source_write_state_t;

struct zip_source {
    zip_source_t *src;
    union {
	zip_source_callback f;
	zip_source_layered_callback l;
    } cb;
    void *ud;
    zip_error_t error;
    zip_int64_t supports;       /* supported commands */
    unsigned int open_count;    /* number of times source was opened (directly or as lower layer) */
    zip_source_write_state_t write_state;          /* whether source is open for writing */
    bool source_closed;         /* set if source archive is closed */
    zip_t *source_archive;      /* zip archive we're reading from, NULL if not from archive */
    unsigned int refcount;
};

#define ZIP_SOURCE_IS_OPEN_READING(src) ((src)->open_count > 0)
#define ZIP_SOURCE_IS_OPEN_WRITING(src) ((src)->write_state == ZIP_SOURCE_WRITE_OPEN)
#define ZIP_SOURCE_IS_LAYERED(src)  ((src)->src != NULL)

/* entry in zip archive directory */

struct zip_entry {
    zip_dirent_t *orig;
    zip_dirent_t *changes;
    zip_source_t *source;
    bool deleted;
};


/* file or archive comment, or filename */

struct zip_string {
    zip_uint8_t *raw;			/* raw string */
    zip_uint16_t length;		/* length of raw string */
    enum zip_encoding_type encoding; 	/* autorecognized encoding */
    zip_uint8_t *converted;     	/* autoconverted string */
    zip_uint32_t converted_length;	/* length of converted */
};


/* bounds checked access to memory buffer */

struct zip_buffer {
    bool ok;
    bool free_data;
    
    zip_uint8_t *data;
    zip_uint64_t size;
    zip_uint64_t offset;
};

/* which files to write in which order */

struct zip_filelist {
    zip_uint64_t idx;
// TODO    const char *name;
};

typedef struct zip_filelist zip_filelist_t;


extern const char * const _zip_err_str[];
extern const int _zip_nerr_str;
extern const int _zip_err_type[];


#define ZIP_ENTRY_CHANGED(e, f)	((e)->changes && ((e)->changes->changed & (f)))

#define ZIP_ENTRY_DATA_CHANGED(x)	((x)->source != NULL)

#define ZIP_IS_RDONLY(za)	((za)->ch_flags & ZIP_AFL_RDONLY)


zip_int64_t _zip_add_entry(zip_t *);

zip_uint8_t *_zip_buffer_data(zip_buffer_t *buffer);
bool _zip_buffer_eof(zip_buffer_t *buffer);
void _zip_buffer_free(zip_buffer_t *buffer);
zip_uint8_t *_zip_buffer_get(zip_buffer_t *buffer, zip_uint64_t length);
zip_uint16_t _zip_buffer_get_16(zip_buffer_t *buffer);
zip_uint32_t _zip_buffer_get_32(zip_buffer_t *buffer);
zip_uint64_t _zip_buffer_get_64(zip_buffer_t *buffer);
zip_uint8_t _zip_buffer_get_8(zip_buffer_t *buffer);
zip_uint64_t _zip_buffer_left(zip_buffer_t *buffer);
zip_buffer_t *_zip_buffer_new(zip_uint8_t *data, zip_uint64_t size);
zip_buffer_t *_zip_buffer_new_from_source(zip_source_t *src, zip_uint64_t size, zip_uint8_t *buf, zip_error_t *error);
zip_uint64_t _zip_buffer_offset(zip_buffer_t *buffer);
bool _zip_buffer_ok(zip_buffer_t *buffer);
int _zip_buffer_put(zip_buffer_t *buffer, const void *src, size_t length);
int _zip_buffer_put_16(zip_buffer_t *buffer, zip_uint16_t i);
int _zip_buffer_put_32(zip_buffer_t *buffer, zip_uint32_t i);
int _zip_buffer_put_64(zip_buffer_t *buffer, zip_uint64_t i);
int _zip_buffer_put_8(zip_buffer_t *buffer, zip_uint8_t i);
int _zip_buffer_set_offset(zip_buffer_t *buffer, zip_uint64_t offset);
zip_uint64_t _zip_buffer_size(zip_buffer_t *buffer);

int _zip_cdir_compute_crc(zip_t *, uLong *);
void _zip_cdir_free(zip_cdir_t *);
zip_cdir_t *_zip_cdir_new(zip_uint64_t, zip_error_t *);
zip_int64_t _zip_cdir_write(zip_t *za, const zip_filelist_t *filelist, zip_uint64_t survivors);
void _zip_deregister_source(zip_t *za, zip_source_t *src);

zip_dirent_t *_zip_dirent_clone(const zip_dirent_t *);
void _zip_dirent_free(zip_dirent_t *);
void _zip_dirent_finalize(zip_dirent_t *);
void _zip_dirent_init(zip_dirent_t *);
bool _zip_dirent_needs_zip64(const zip_dirent_t *, zip_flags_t);
zip_dirent_t *_zip_dirent_new(void);
zip_int64_t _zip_dirent_read(zip_dirent_t *zde, zip_source_t *src, zip_buffer_t *buffer, bool local, zip_error_t *error);
zip_int32_t _zip_dirent_size(zip_source_t *src, zip_uint16_t, zip_error_t *);
int _zip_dirent_write(zip_t *za, zip_dirent_t *dirent, zip_flags_t flags);

zip_extra_field_t *_zip_ef_clone(const zip_extra_field_t *, zip_error_t *);
zip_extra_field_t *_zip_ef_delete_by_id(zip_extra_field_t *, zip_uint16_t, zip_uint16_t, zip_flags_t);
void _zip_ef_free(zip_extra_field_t *);
const zip_uint8_t *_zip_ef_get_by_id(const zip_extra_field_t *, zip_uint16_t *, zip_uint16_t, zip_uint16_t, zip_flags_t, zip_error_t *);
zip_extra_field_t *_zip_ef_merge(zip_extra_field_t *, zip_extra_field_t *);
zip_extra_field_t *_zip_ef_new(zip_uint16_t, zip_uint16_t, const zip_uint8_t *, zip_flags_t);
zip_extra_field_t *_zip_ef_parse(const zip_uint8_t *, zip_uint16_t, zip_flags_t, zip_error_t *);
zip_extra_field_t *_zip_ef_remove_internal(zip_extra_field_t *);
zip_uint16_t _zip_ef_size(const zip_extra_field_t *, zip_flags_t);
int _zip_ef_write(zip_t *za, const zip_extra_field_t *ef, zip_flags_t flags);

void _zip_entry_finalize(zip_entry_t *);
void _zip_entry_init(zip_entry_t *);

void _zip_error_clear(zip_error_t *);
void _zip_error_get(const zip_error_t *, int *, int *);

void _zip_error_copy(zip_error_t *dst, const zip_error_t *src);
void _zip_error_set_from_source(zip_error_t *, zip_source_t *);

const zip_uint8_t *_zip_extract_extra_field_by_id(zip_error_t *, zip_uint16_t, int, const zip_uint8_t *, zip_uint16_t, zip_uint16_t *);

int _zip_file_extra_field_prepare_for_change(zip_t *, zip_uint64_t);
int _zip_file_fillbuf(void *, size_t, zip_file_t *);
zip_uint64_t _zip_file_get_offset(const zip_t *, zip_uint64_t, zip_error_t *);

int _zip_filerange_crc(zip_source_t *src, zip_uint64_t offset, zip_uint64_t length, uLong *crcp, zip_error_t *error);

zip_dirent_t *_zip_get_dirent(zip_t *, zip_uint64_t, zip_flags_t, zip_error_t *);

enum zip_encoding_type _zip_guess_encoding(zip_string_t *, enum zip_encoding_type);
zip_uint8_t *_zip_cp437_to_utf8(const zip_uint8_t * const, zip_uint32_t, zip_uint32_t *, zip_error_t *);

zip_t *_zip_open(zip_source_t *, unsigned int, zip_error_t *);

int _zip_read(zip_source_t *src, zip_uint8_t *data, zip_uint64_t length, zip_error_t *error);
int _zip_read_at_offset(zip_source_t *src, zip_uint64_t offset, unsigned char *b, size_t length, zip_error_t *error);
zip_uint8_t *_zip_read_data(zip_buffer_t *buffer, zip_source_t *src, size_t length, bool nulp, zip_error_t *error);
int _zip_read_local_ef(zip_t *, zip_uint64_t);
zip_string_t *_zip_read_string(zip_buffer_t *buffer, zip_source_t *src, zip_uint16_t lenght, bool nulp, zip_error_t *error);
int _zip_register_source(zip_t *za, zip_source_t *src);

void _zip_set_open_error(int *zep, const zip_error_t *err, int ze);

zip_int64_t _zip_source_call(zip_source_t *src, void *data, zip_uint64_t length, zip_source_cmd_t command);
zip_source_t *_zip_source_file_or_p(const char *, FILE *, zip_uint64_t, zip_int64_t, const zip_stat_t *, zip_error_t *error);
void _zip_source_invalidate(zip_source_t *src);
zip_source_t *_zip_source_new(zip_error_t *error);
int _zip_source_set_source_archive(zip_source_t *, zip_t *);
zip_source_t *_zip_source_window_new(zip_source_t *src, zip_uint64_t start, zip_uint64_t length, zip_stat_t *st, zip_error_t *error);
zip_source_t *_zip_source_zip_new(zip_t *, zip_t *, zip_uint64_t, zip_flags_t, zip_uint64_t, zip_uint64_t, const char *);

int _zip_stat_merge(zip_stat_t *dst, const zip_stat_t *src, zip_error_t *error);
int _zip_string_equal(const zip_string_t *, const zip_string_t *);
void _zip_string_free(zip_string_t *);
zip_uint32_t _zip_string_crc32(const zip_string_t *);
const zip_uint8_t *_zip_string_get(zip_string_t *, zip_uint32_t *, zip_flags_t, zip_error_t *);
zip_uint16_t _zip_string_length(const zip_string_t *);
zip_string_t *_zip_string_new(const zip_uint8_t *, zip_uint16_t, zip_flags_t, zip_error_t *);
int _zip_string_write(zip_t *za, const zip_string_t *string);

int _zip_changed(const zip_t *, zip_uint64_t *);
const char *_zip_get_name(zip_t *, zip_uint64_t, zip_flags_t, zip_error_t *);
int _zip_local_header_read(zip_t *, int);
void *_zip_memdup(const void *, size_t, zip_error_t *);
zip_int64_t _zip_name_locate(zip_t *, const char *, zip_flags_t, zip_error_t *);
zip_t *_zip_new(zip_error_t *);

zip_int64_t _zip_file_replace(zip_t *, zip_uint64_t, const char *, zip_source_t *, zip_flags_t);
int _zip_set_name(zip_t *, zip_uint64_t, const char *, zip_flags_t);
void _zip_u2d_time(time_t, zip_uint16_t *, zip_uint16_t *);
int _zip_unchange(zip_t *, zip_uint64_t, int);
void _zip_unchange_data(zip_entry_t *);
int _zip_write(zip_t *za, const void *data, zip_uint64_t length);

#endif /* zipint.h */
