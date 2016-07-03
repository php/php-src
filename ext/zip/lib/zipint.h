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
#ifdef PHP_WIN32
# include "php_zip_config.w32.h"
#else
# include "config.h"
#endif

#ifdef HAVE_MOVEFILEEXA
#include <windows.h>
#define _zip_rename(s, t)	(!MoveFileExA((s), (t), MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING))
#else
#define _zip_rename	rename
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
#if defined(HAVE__SNPRINTF) && !defined(PHP_WIN32)
#define snprintf	_snprintf
#endif
#if defined(HAVE__STRDUP) && !defined(HAVE_STRDUP)
#define strdup		_strdup
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
#elif SIZEOF_OFF_T == 4
#define ZIP_OFF_MAX ZIP_INT32_MAX
#elif SIZEOF_OFF_T == 2
#define ZIP_OFF_MAX ZIP_INT16_MAX
#else
#error unsupported size of off_t
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
#define TORRENT_SIG	"TORRENTZIPPED-"
#define TORRENT_SIG_LEN	14
#define TORRENT_CRC_LEN 8
#define TORRENT_MEM_LEVEL	8
#define CDENTRYSIZE         46u
#define LENTRYSIZE          30
#define MAXCOMLEN        65536
#define MAXEXTLEN        65536
#define EOCDLEN             22
#define EOCD64LOCLEN	    20
#define EOCD64LEN	    56
#define CDBUFSIZE       (MAXCOMLEN+EOCDLEN+EOCD64LOCLEN)
#define BUFSIZE		8192

#define ZIP_CM_REPLACED_DEFAULT (-2)

#define ZIP_CM_IS_DEFAULT(x)	((x) == ZIP_CM_DEFAULT || (x) == ZIP_CM_REPLACED_DEFAULT)

#define ZIP_EF_UTF_8_COMMENT	0x6375
#define ZIP_EF_UTF_8_NAME	0x7075
#define ZIP_EF_ZIP64		0x0001

#define ZIP_EF_IS_INTERNAL(id)	((id) == ZIP_EF_UTF_8_COMMENT || (id) == ZIP_EF_UTF_8_NAME || (id) == ZIP_EF_ZIP64)

/* according to unzip-6.0's zipinfo.c, this corresponds to a regular file with rw permissions for everyone */
#define ZIP_EXT_ATTRIB_DEFAULT		(0100666<<16)
/* according to unzip-6.0's zipinfo.c, this corresponds to a directory with rwx permissions for everyone */
#define ZIP_EXT_ATTRIB_DEFAULT_DIR	(0040777<<16)


/* This section contains API that won't materialize like this.  It's
   placed in the internal section, pending cleanup. */

typedef struct zip_source *(*zip_compression_implementation)(struct zip *,
						     struct zip_source *,
						     zip_int32_t, int);
typedef struct zip_source *(*zip_encryption_implementation)(struct zip *,
						    struct zip_source *,
						    zip_uint16_t, int,
						    const char *);

zip_compression_implementation _zip_get_compression_implementation(zip_int32_t);
zip_encryption_implementation _zip_get_encryption_implementation(zip_uint16_t);




/* This API is not final yet, but we need it internally, so it's private for now. */

const zip_uint8_t *zip_get_extra_field_by_id(struct zip *, int, int, zip_uint16_t, int, zip_uint16_t *);

/* This section contains API that is of limited use until support for
   user-supplied compression/encryption implementation is finished.
   Thus we will keep it private for now. */

typedef zip_int64_t (*zip_source_layered_callback)(struct zip_source *, void *,
						   void *, zip_uint64_t,
						   enum zip_source_cmd);

void zip_source_close(struct zip_source *);
struct zip_source *zip_source_crc(struct zip *, struct zip_source *,
				  int);
struct zip_source *zip_source_deflate(struct zip *,
				      struct zip_source *,
				      zip_int32_t, int);
void zip_source_error(struct zip_source *, int *, int *);
struct zip_source *zip_source_layered(struct zip *,
				      struct zip_source *,
				      zip_source_layered_callback,
				      void *);
int zip_source_open(struct zip_source *);
struct zip_source *zip_source_pkware(struct zip *,
				     struct zip_source *,
				     zip_uint16_t, int,
				     const char *);
zip_int64_t zip_source_read(struct zip_source *, void *,
			    zip_uint64_t);
int zip_source_stat(struct zip_source *, struct zip_stat *);
struct zip_source *zip_source_window(struct zip *, struct zip_source *,
				     zip_uint64_t, zip_uint64_t);


/* This function will probably remain private.  It is not needed to
   implement compression/encryption routines.  (We should probably
   rename it to _zip_source_pop.) */

struct zip_source *zip_source_pop(struct zip_source *);



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

/* error information */

struct zip_error {
    int zip_err;	/* libzip error code (ZIP_ER_*) */
    int sys_err;	/* copy of errno (E*) or zlib error code */
    char *str;		/* string representation or NULL */
};

/* zip archive, part of API */

struct zip {
    char *zn;				/* file name */
    FILE *zp;				/* file */
    unsigned int open_flags;		/* flags passed to zip_open */
    struct zip_error error;		/* error information */

    unsigned int flags;			/* archive global flags */
    unsigned int ch_flags;		/* changed archive global flags */

    char *default_password;		/* password used when no other supplied */

    struct zip_string *comment_orig;	/* archive comment */
    struct zip_string *comment_changes; /* changed archive comment */
    int comment_changed;		/* whether archive comment was changed */

    zip_uint64_t nentry;		/* number of entries */
    zip_uint64_t nentry_alloc;		/* number of entries allocated */
    struct zip_entry *entry;		/* entries */

    unsigned int nfile;			/* number of opened files within archive */
    unsigned int nfile_alloc;		/* number of files allocated */
    struct zip_file **file;		/* opened files within archive */
    
    char *tempdir;                      /* custom temp dir (needed e.g. for OS X sandboxing) */
};

/* file in zip archive, part of API */

struct zip_file {
    struct zip *za;		/* zip archive containing this file */
    struct zip_error error;	/* error information */
    int eof;
    struct zip_source *src;	/* data source */
};

/* zip archive directory entry (central or local) */

#define ZIP_DIRENT_COMP_METHOD	0x0001u
#define ZIP_DIRENT_FILENAME	0x0002u
#define ZIP_DIRENT_COMMENT	0x0004u
#define ZIP_DIRENT_EXTRA_FIELD	0x0008u
#define ZIP_DIRENT_ATTRIBUTES	0x0010u
#define ZIP_DIRENT_ALL		0xffffu

struct zip_dirent {
    zip_uint32_t changed;
    int local_extra_fields_read;		/*      whether we already read in local header extra fields */
    int cloned;                                 /*      whether this instance is cloned, and thus shares non-changed strings */

    zip_uint16_t version_madeby;		/* (c)  version of creator */
    zip_uint16_t version_needed;		/* (cl) version needed to extract */
    zip_uint16_t bitflags;			/* (cl) general purpose bit flag */
    zip_int32_t comp_method;			/* (cl) compression method used (uint16 and ZIP_CM_DEFAULT (-1)) */
    time_t last_mod;				/* (cl) time of last modification */
    zip_uint32_t crc;				/* (cl) CRC-32 of uncompressed data */
    zip_uint64_t comp_size;			/* (cl) size of compressed data */
    zip_uint64_t uncomp_size;			/* (cl) size of uncompressed data */
    struct zip_string *filename;		/* (cl) file name (NUL-terminated) */
    struct zip_extra_field *extra_fields;	/* (cl) extra fields, parsed */
    struct zip_string *comment;			/* (c)  file comment */
    zip_uint32_t disk_number;			/* (c)  disk number start */
    zip_uint16_t int_attrib;			/* (c)  internal file attributes */
    zip_uint32_t ext_attrib;			/* (c)  external file attributes */
    zip_uint64_t offset;			/* (c)  offset of local header */
};

/* zip archive central directory */

struct zip_cdir {
    struct zip_entry *entry;	 		/* directory entries */
    zip_uint64_t nentry;			/* number of entries */
    zip_uint64_t nentry_alloc;			/* number of entries allocated */

    off_t size;                                 /* size of central directory */
    off_t offset;		 		/* offset of central directory in file */
    struct zip_string *comment;			/* zip archive comment */
};

struct zip_extra_field {
    struct zip_extra_field *next;
    zip_flags_t flags;				/* in local/central header */
    zip_uint16_t id;				/* header id */
    zip_uint16_t size;				/* data size */
    zip_uint8_t *data;
};



struct zip_source {
    struct zip_source *src;
    union {
	zip_source_callback f;
	zip_source_layered_callback l;
    } cb;
    void *ud;
    enum zip_les error_source;
    int is_open;
};

/* entry in zip archive directory */

struct zip_entry {
    struct zip_dirent *orig;
    struct zip_dirent *changes;
    struct zip_source *source;
    int deleted;
};



/* file or archive comment, or filename */

struct zip_string {
    zip_uint8_t *raw;			/* raw string */
    zip_uint16_t length;		/* length of raw string */
    enum zip_encoding_type encoding; 	/* autorecognized encoding */
    zip_uint8_t *converted;     	/* autoconverted string */
    zip_uint32_t converted_length;	/* length of converted */
};



/* which files to write, and in which order (name is for torrentzip sorting) */

struct zip_filelist {
    zip_uint64_t idx;
    const char *name;
};



extern const char * const _zip_err_str[];
extern const int _zip_nerr_str;
extern const int _zip_err_type[];



#define ZIP_ENTRY_CHANGED(e, f)	((e)->changes && ((e)->changes->changed & (f)))

#define ZIP_ENTRY_DATA_CHANGED(x)	((x)->source != NULL)

#define ZIP_IS_RDONLY(za)	((za)->ch_flags & ZIP_AFL_RDONLY)



zip_int64_t _zip_add_entry(struct zip *);

int _zip_cdir_compute_crc(struct zip *, uLong *);
void _zip_cdir_free(struct zip_cdir *);
int _zip_cdir_grow(struct zip_cdir *, zip_uint64_t, struct zip_error *);
struct zip_cdir *_zip_cdir_new(zip_uint64_t, struct zip_error *);
zip_int64_t _zip_cdir_write(struct zip *, const struct zip_filelist *, zip_uint64_t, FILE *);

struct zip_dirent *_zip_dirent_clone(const struct zip_dirent *);
void _zip_dirent_free(struct zip_dirent *);
void _zip_dirent_finalize(struct zip_dirent *);
void _zip_dirent_init(struct zip_dirent *);
int _zip_dirent_needs_zip64(const struct zip_dirent *, zip_flags_t);
struct zip_dirent *_zip_dirent_new(void);
int _zip_dirent_read(struct zip_dirent *, FILE *, const unsigned char **,
		     zip_uint64_t *, int, struct zip_error *);
zip_int32_t _zip_dirent_size(FILE *, zip_uint16_t, struct zip_error *);
void _zip_dirent_torrent_normalize(struct zip_dirent *);
int _zip_dirent_write(struct zip_dirent *, FILE *, zip_flags_t, struct zip_error *);

struct zip_extra_field *_zip_ef_clone(const struct zip_extra_field *, struct zip_error *);
struct zip_extra_field *_zip_ef_delete_by_id(struct zip_extra_field *, zip_uint16_t, zip_uint16_t, zip_flags_t);
void _zip_ef_free(struct zip_extra_field *);
const zip_uint8_t *_zip_ef_get_by_id(const struct zip_extra_field *, zip_uint16_t *, zip_uint16_t, zip_uint16_t, zip_flags_t, struct zip_error *);
struct zip_extra_field *_zip_ef_merge(struct zip_extra_field *, struct zip_extra_field *);
struct zip_extra_field *_zip_ef_new(zip_uint16_t, zip_uint16_t, const zip_uint8_t *, zip_flags_t);
struct zip_extra_field *_zip_ef_parse(const zip_uint8_t *, zip_uint16_t, zip_flags_t, struct zip_error *);
struct zip_extra_field *_zip_ef_remove_internal(struct zip_extra_field *);
zip_uint16_t _zip_ef_size(const struct zip_extra_field *, zip_flags_t);
void _zip_ef_write(const struct zip_extra_field *, zip_flags_t, FILE *);

void _zip_entry_finalize(struct zip_entry *);
void _zip_entry_init(struct zip_entry *);

void _zip_error_clear(struct zip_error *);
void _zip_error_copy(struct zip_error *, const struct zip_error *);
void _zip_error_fini(struct zip_error *);
void _zip_error_get(const struct zip_error *, int *, int *);
void _zip_error_init(struct zip_error *);
void _zip_error_set(struct zip_error *, int, int);
void _zip_error_set_from_source(struct zip_error *, struct zip_source *);
const char *_zip_error_strerror(struct zip_error *);

const zip_uint8_t *_zip_extract_extra_field_by_id(struct zip_error *, zip_uint16_t, int, const zip_uint8_t *, zip_uint16_t, zip_uint16_t *);

int _zip_file_extra_field_prepare_for_change(struct zip *, zip_uint64_t);
int _zip_file_fillbuf(void *, size_t, struct zip_file *);
zip_uint64_t _zip_file_get_offset(const struct zip *, zip_uint64_t, struct zip_error *);

int _zip_filerange_crc(FILE *, off_t, off_t, uLong *, struct zip_error *);

struct zip_dirent *_zip_get_dirent(struct zip *, zip_uint64_t, zip_flags_t, struct zip_error *);

enum zip_encoding_type _zip_guess_encoding(struct zip_string *, enum zip_encoding_type);
zip_uint8_t *_zip_cp437_to_utf8(const zip_uint8_t * const, zip_uint32_t,
				zip_uint32_t *, struct zip_error *error);

struct zip *_zip_open(const char *, FILE *, unsigned int, int *);

int _zip_read_local_ef(struct zip *, zip_uint64_t);

struct zip_source *_zip_source_file_or_p(struct zip *, const char *, FILE *,
					 zip_uint64_t, zip_int64_t, int,
					 const struct zip_stat *);
struct zip_source *_zip_source_new(struct zip *);
struct zip_source *_zip_source_zip_new(struct zip *, struct zip *, zip_uint64_t, zip_flags_t,
				       zip_uint64_t, zip_uint64_t, const char *);

int _zip_string_equal(const struct zip_string *, const struct zip_string *);
void _zip_string_free(struct zip_string *);
zip_uint32_t _zip_string_crc32(const struct zip_string *);
const zip_uint8_t *_zip_string_get(struct zip_string *, zip_uint32_t *, zip_flags_t, struct zip_error *);
zip_uint16_t _zip_string_length(const struct zip_string *);
struct zip_string *_zip_string_new(const zip_uint8_t *, zip_uint16_t, zip_flags_t, struct zip_error *);
void _zip_string_write(const struct zip_string *, FILE *);

int _zip_changed(const struct zip *, zip_uint64_t *);
const char *_zip_get_name(struct zip *, zip_uint64_t, zip_flags_t, struct zip_error *);
int _zip_local_header_read(struct zip *, int);
void *_zip_memdup(const void *, size_t, struct zip_error *);
zip_int64_t _zip_name_locate(struct zip *, const char *, zip_flags_t, struct zip_error *);
struct zip *_zip_new(struct zip_error *);
zip_uint16_t _zip_read2(const zip_uint8_t **);
zip_uint32_t _zip_read4(const zip_uint8_t **);
zip_uint64_t _zip_read8(const zip_uint8_t **);
zip_uint8_t *_zip_read_data(const zip_uint8_t **, FILE *, size_t, int, struct zip_error *);
zip_int64_t _zip_file_replace(struct zip *, zip_uint64_t, const char *, struct zip_source *, zip_flags_t);
int _zip_set_name(struct zip *, zip_uint64_t, const char *, zip_flags_t);
void _zip_u2d_time(time_t, zip_uint16_t *, zip_uint16_t *);
int _zip_unchange(struct zip *, zip_uint64_t, int);
void _zip_unchange_data(struct zip_entry *);

void _zip_poke4(zip_uint32_t, zip_uint8_t **);
void _zip_poke8(zip_uint64_t, zip_uint8_t **);
void _zip_write2(zip_uint16_t, FILE *);
void _zip_write4(zip_uint32_t, FILE *);
void _zip_write8(zip_uint64_t, FILE *);


#endif /* zipint.h */
