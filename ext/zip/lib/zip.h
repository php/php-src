#ifndef _HAD_ZIP_H
#define _HAD_ZIP_H

/*
  zip.h -- exported declarations.
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


#ifndef ZIP_EXTERN
# ifndef ZIP_STATIC
#  if defined(_WIN32) && defined(PHP_ZIP_EXPORTS)
#   define ZIP_EXTERN __declspec(dllimport)
#  elif defined(__GNUC__) && __GNUC__ >= 4
#   define ZIP_EXTERN __attribute__ ((visibility ("default")))
#  else
#   define ZIP_EXTERN
#  endif
# else
#  define ZIP_EXTERN
# endif
#endif

#ifdef __cplusplus
extern "C" {
#if 0
} /* fix autoindent */
#endif
#endif

#include <zipconf.h>

#include <sys/types.h>
#include <stdio.h>
#include <time.h>

/* flags for zip_open */

#define ZIP_CREATE           1
#define ZIP_EXCL             2
#define ZIP_CHECKCONS        4
#define ZIP_TRUNCATE         8
#define ZIP_RDONLY          16


/* flags for zip_name_locate, zip_fopen, zip_stat, ... */

#define ZIP_FL_NOCASE		1u /* ignore case on name lookup */
#define ZIP_FL_NODIR		2u /* ignore directory component */
#define ZIP_FL_COMPRESSED	4u /* read compressed data */
#define ZIP_FL_UNCHANGED	8u /* use original data, ignoring changes */
#define ZIP_FL_RECOMPRESS      16u /* force recompression of data */
#define ZIP_FL_ENCRYPTED       32u /* read encrypted data (implies ZIP_FL_COMPRESSED) */
#define ZIP_FL_ENC_GUESS        0u /* guess string encoding (is default) */
#define ZIP_FL_ENC_RAW         64u /* get unmodified string */
#define ZIP_FL_ENC_STRICT     128u /* follow specification strictly */
#define ZIP_FL_LOCAL	      256u /* in local header */
#define ZIP_FL_CENTRAL	      512u /* in central directory */
/*                           1024u    reserved for internal use */
#define ZIP_FL_ENC_UTF_8     2048u /* string is UTF-8 encoded */
#define ZIP_FL_ENC_CP437     4096u /* string is CP437 encoded */
#define ZIP_FL_OVERWRITE     8192u /* zip_file_add: if file with name exists, overwrite (replace) it */

/* archive global flags flags */

#define ZIP_AFL_RDONLY		2u /* read only -- cannot be cleared */


/* create a new extra field */

#define ZIP_EXTRA_FIELD_ALL	ZIP_UINT16_MAX
#define ZIP_EXTRA_FIELD_NEW	ZIP_UINT16_MAX


/* libzip error codes */

#define ZIP_ER_OK             0  /* N No error */
#define ZIP_ER_MULTIDISK      1  /* N Multi-disk zip archives not supported */
#define ZIP_ER_RENAME         2  /* S Renaming temporary file failed */
#define ZIP_ER_CLOSE          3  /* S Closing zip archive failed */
#define ZIP_ER_SEEK           4  /* S Seek error */
#define ZIP_ER_READ           5  /* S Read error */
#define ZIP_ER_WRITE          6  /* S Write error */
#define ZIP_ER_CRC            7  /* N CRC error */
#define ZIP_ER_ZIPCLOSED      8  /* N Containing zip archive was closed */
#define ZIP_ER_NOENT          9  /* N No such file */
#define ZIP_ER_EXISTS        10  /* N File already exists */
#define ZIP_ER_OPEN          11  /* S Can't open file */
#define ZIP_ER_TMPOPEN       12  /* S Failure to create temporary file */
#define ZIP_ER_ZLIB          13  /* Z Zlib error */
#define ZIP_ER_MEMORY        14  /* N Malloc failure */
#define ZIP_ER_CHANGED       15  /* N Entry has been changed */
#define ZIP_ER_COMPNOTSUPP   16  /* N Compression method not supported */
#define ZIP_ER_EOF           17  /* N Premature end of file */
#define ZIP_ER_INVAL         18  /* N Invalid argument */
#define ZIP_ER_NOZIP         19  /* N Not a zip archive */
#define ZIP_ER_INTERNAL      20  /* N Internal error */
#define ZIP_ER_INCONS        21  /* N Zip archive inconsistent */
#define ZIP_ER_REMOVE        22  /* S Can't remove file */
#define ZIP_ER_DELETED       23  /* N Entry has been deleted */
#define ZIP_ER_ENCRNOTSUPP   24  /* N Encryption method not supported */
#define ZIP_ER_RDONLY        25  /* N Read-only archive */ 
#define ZIP_ER_NOPASSWD      26  /* N No password provided */
#define ZIP_ER_WRONGPASSWD   27  /* N Wrong password provided */
#define ZIP_ER_OPNOTSUPP     28  /* N Operation not supported */
#define ZIP_ER_INUSE         29  /* N Resource still in use */
#define ZIP_ER_TELL          30  /* S Tell error */

/* type of system error value */

#define ZIP_ET_NONE	      0  /* sys_err unused */
#define ZIP_ET_SYS	      1  /* sys_err is errno */
#define ZIP_ET_ZLIB	      2  /* sys_err is zlib error code */

/* compression methods */

#define ZIP_CM_DEFAULT	      -1  /* better of deflate or store */
#define ZIP_CM_STORE	       0  /* stored (uncompressed) */
#define ZIP_CM_SHRINK	       1  /* shrunk */
#define ZIP_CM_REDUCE_1	       2  /* reduced with factor 1 */
#define ZIP_CM_REDUCE_2	       3  /* reduced with factor 2 */
#define ZIP_CM_REDUCE_3	       4  /* reduced with factor 3 */
#define ZIP_CM_REDUCE_4	       5  /* reduced with factor 4 */
#define ZIP_CM_IMPLODE	       6  /* imploded */
/* 7 - Reserved for Tokenizing compression algorithm */
#define ZIP_CM_DEFLATE	       8  /* deflated */
#define ZIP_CM_DEFLATE64       9  /* deflate64 */
#define ZIP_CM_PKWARE_IMPLODE 10  /* PKWARE imploding */
/* 11 - Reserved by PKWARE */
#define ZIP_CM_BZIP2          12  /* compressed using BZIP2 algorithm */
/* 13 - Reserved by PKWARE */
#define ZIP_CM_LZMA	      14  /* LZMA (EFS) */
/* 15-17 - Reserved by PKWARE */
#define ZIP_CM_TERSE	      18  /* compressed using IBM TERSE (new) */
#define ZIP_CM_LZ77           19  /* IBM LZ77 z Architecture (PFS) */
#define ZIP_CM_WAVPACK	      97  /* WavPack compressed data */
#define ZIP_CM_PPMD	      98  /* PPMd version I, Rev 1 */

/* encryption methods */

#define ZIP_EM_NONE	       0  /* not encrypted */
#define ZIP_EM_TRAD_PKWARE     1  /* traditional PKWARE encryption */
#if 0 /* Strong Encryption Header not parsed yet */
#define ZIP_EM_DES        0x6601  /* strong encryption: DES */
#define ZIP_EM_RC2_OLD    0x6602  /* strong encryption: RC2, version < 5.2 */
#define ZIP_EM_3DES_168   0x6603
#define ZIP_EM_3DES_112   0x6609
#define ZIP_EM_AES_128    0x660e
#define ZIP_EM_AES_192    0x660f
#define ZIP_EM_AES_256    0x6610
#define ZIP_EM_RC2        0x6702  /* strong encryption: RC2, version >= 5.2 */
#define ZIP_EM_RC4        0x6801
#endif
#define ZIP_EM_UNKNOWN    0xffff  /* unknown algorithm */

#define ZIP_OPSYS_DOS	  	0x00u
#define ZIP_OPSYS_AMIGA	 	0x01u
#define ZIP_OPSYS_OPENVMS	0x02u
#define ZIP_OPSYS_UNIX	  	0x03u
#define ZIP_OPSYS_VM_CMS	0x04u
#define ZIP_OPSYS_ATARI_ST	0x05u
#define ZIP_OPSYS_OS_2		0x06u
#define ZIP_OPSYS_MACINTOSH	0x07u
#define ZIP_OPSYS_Z_SYSTEM	0x08u
#define ZIP_OPSYS_CPM	  	0x09u
#define ZIP_OPSYS_WINDOWS_NTFS	0x0au
#define ZIP_OPSYS_MVS	  	0x0bu
#define ZIP_OPSYS_VSE	  	0x0cu
#define ZIP_OPSYS_ACORN_RISC	0x0du
#define ZIP_OPSYS_VFAT	  	0x0eu
#define ZIP_OPSYS_ALTERNATE_MVS	0x0fu
#define ZIP_OPSYS_BEOS	  	0x10u
#define ZIP_OPSYS_TANDEM	0x11u
#define ZIP_OPSYS_OS_400	0x12u
#define ZIP_OPSYS_OS_X	  	0x13u

#define ZIP_OPSYS_DEFAULT	ZIP_OPSYS_UNIX


enum zip_source_cmd {
    ZIP_SOURCE_OPEN,            /* prepare for reading */
    ZIP_SOURCE_READ,            /* read data */
    ZIP_SOURCE_CLOSE,           /* reading is done */
    ZIP_SOURCE_STAT,            /* get meta information */
    ZIP_SOURCE_ERROR,           /* get error information */
    ZIP_SOURCE_FREE,            /* cleanup and free resources */
    ZIP_SOURCE_SEEK,            /* set position for reading */
    ZIP_SOURCE_TELL,            /* get read position */
    ZIP_SOURCE_BEGIN_WRITE,     /* prepare for writing */
    ZIP_SOURCE_COMMIT_WRITE,    /* writing is done */
    ZIP_SOURCE_ROLLBACK_WRITE,  /* discard written changes */
    ZIP_SOURCE_WRITE,           /* write data */
    ZIP_SOURCE_SEEK_WRITE,      /* set position for writing */
    ZIP_SOURCE_TELL_WRITE,      /* get write position */
    ZIP_SOURCE_SUPPORTS,        /* check whether source supports command */
    ZIP_SOURCE_REMOVE           /* remove file */
};
typedef enum zip_source_cmd zip_source_cmd_t;

#define ZIP_SOURCE_MAKE_COMMAND_BITMASK(cmd)    (1<<(cmd))

#define ZIP_SOURCE_SUPPORTS_READABLE	(ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_OPEN) \
                                         | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_READ) \
                                         | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_CLOSE) \
                                         | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_STAT) \
                                         | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_ERROR) \
                                         | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_FREE))

#define ZIP_SOURCE_SUPPORTS_SEEKABLE	(ZIP_SOURCE_SUPPORTS_READABLE \
                                         | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_SEEK) \
                                         | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_TELL) \
                                         | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_SUPPORTS))

#define ZIP_SOURCE_SUPPORTS_WRITABLE    (ZIP_SOURCE_SUPPORTS_SEEKABLE \
                                         | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_BEGIN_WRITE) \
                                         | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_COMMIT_WRITE) \
                                         | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_ROLLBACK_WRITE) \
                                         | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_WRITE) \
                                         | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_SEEK_WRITE) \
                                         | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_TELL_WRITE) \
                                         | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_REMOVE))

/* for use by sources */
struct zip_source_args_seek {
    zip_int64_t offset;
    int whence;
};

typedef struct zip_source_args_seek zip_source_args_seek_t;
#define ZIP_SOURCE_GET_ARGS(type, data, len, error) ((len) < sizeof(type) ? zip_error_set((error), ZIP_ER_INVAL, 0), (type *)NULL : (type *)(data))


/* error information */
/* use zip_error_*() to access */
struct zip_error {
    int zip_err;	/* libzip error code (ZIP_ER_*) */
    int sys_err;	/* copy of errno (E*) or zlib error code */
    char *str;		/* string representation or NULL */
};

#define ZIP_STAT_NAME			0x0001u
#define ZIP_STAT_INDEX			0x0002u
#define ZIP_STAT_SIZE			0x0004u
#define ZIP_STAT_COMP_SIZE		0x0008u
#define ZIP_STAT_MTIME			0x0010u
#define ZIP_STAT_CRC			0x0020u
#define ZIP_STAT_COMP_METHOD		0x0040u
#define ZIP_STAT_ENCRYPTION_METHOD	0x0080u
#define ZIP_STAT_FLAGS			0x0100u

struct zip_stat {
    zip_uint64_t valid;			/* which fields have valid values */
    const char *name;			/* name of the file */
    zip_uint64_t index;			/* index within archive */
    zip_uint64_t size;			/* size of file (uncompressed) */
    zip_uint64_t comp_size;		/* size of file (compressed) */
    time_t mtime;			/* modification time */
    zip_uint32_t crc;			/* crc of file data */
    zip_uint16_t comp_method;		/* compression method used */
    zip_uint16_t encryption_method;	/* encryption method used */
    zip_uint32_t flags;			/* reserved for future use */
};

struct zip;
struct zip_file;
struct zip_source;

typedef struct zip zip_t;
typedef struct zip_error zip_error_t;
typedef struct zip_file zip_file_t;
typedef struct zip_source zip_source_t;
typedef struct zip_stat zip_stat_t;

typedef zip_uint32_t zip_flags_t;    

typedef zip_int64_t (*zip_source_callback)(void *, void *, zip_uint64_t, zip_source_cmd_t);


#ifndef ZIP_DISABLE_DEPRECATED
ZIP_EXTERN zip_int64_t zip_add(zip_t *, const char *, zip_source_t *); /* use zip_file_add */
ZIP_EXTERN zip_int64_t zip_add_dir(zip_t *, const char *); /* use zip_dir_add */
ZIP_EXTERN const char *zip_get_file_comment(zip_t *, zip_uint64_t, int *, int); /* use zip_file_get_comment */
ZIP_EXTERN int zip_get_num_files(zip_t *);  /* use zip_get_num_entries instead */
ZIP_EXTERN int zip_rename(zip_t *, zip_uint64_t, const char *); /* use zip_file_rename */
ZIP_EXTERN int zip_replace(zip_t *, zip_uint64_t, zip_source_t *); /* use zip_file_replace */
ZIP_EXTERN int zip_set_file_comment(zip_t *, zip_uint64_t, const char *, int); /* use zip_file_set_comment */
ZIP_EXTERN int zip_error_get_sys_type(int); /* use zip_error_system_type */
ZIP_EXTERN void zip_error_get(zip_t *, int *, int *); /* use zip_get_error, zip_error_code_zip / zip_error_code_system */
ZIP_EXTERN int zip_error_to_str(char *, zip_uint64_t, int, int);
ZIP_EXTERN void zip_file_error_get(zip_file_t *, int *, int *); /* use zip_file_get_error, zip_error_code_zip / zip_error_code_system */
#endif

ZIP_EXTERN int zip_archive_set_tempdir(zip_t *, const char *);
ZIP_EXTERN int zip_close(zip_t *);
ZIP_EXTERN int zip_delete(zip_t *, zip_uint64_t);
ZIP_EXTERN zip_int64_t zip_dir_add(zip_t *, const char *, zip_flags_t);
ZIP_EXTERN void zip_discard(zip_t *);

ZIP_EXTERN zip_error_t *zip_get_error(zip_t *);
ZIP_EXTERN void zip_error_clear(zip_t *);
ZIP_EXTERN int zip_error_code_zip(const zip_error_t *);
ZIP_EXTERN int zip_error_code_system(const zip_error_t *);
ZIP_EXTERN void zip_error_fini(zip_error_t *);
ZIP_EXTERN void zip_error_init(zip_error_t *);
ZIP_EXTERN void zip_error_init_with_code(zip_error_t *, int);
ZIP_EXTERN void zip_error_set(zip_error_t *, int, int);
ZIP_EXTERN const char *zip_error_strerror(zip_error_t *);
ZIP_EXTERN int zip_error_system_type(const zip_error_t *);
ZIP_EXTERN zip_int64_t zip_error_to_data(const zip_error_t *, void *, zip_uint64_t);

ZIP_EXTERN int zip_fclose(zip_file_t *);
ZIP_EXTERN zip_t *zip_fdopen(int, int, int *);
ZIP_EXTERN zip_int64_t zip_file_add(zip_t *, const char *, zip_source_t *, zip_flags_t);
ZIP_EXTERN void zip_file_error_clear(zip_file_t *);
ZIP_EXTERN int zip_file_extra_field_delete(zip_t *, zip_uint64_t, zip_uint16_t, zip_flags_t);
ZIP_EXTERN int zip_file_extra_field_delete_by_id(zip_t *, zip_uint64_t, zip_uint16_t, zip_uint16_t, zip_flags_t);
ZIP_EXTERN int zip_file_extra_field_set(zip_t *, zip_uint64_t, zip_uint16_t, zip_uint16_t, const zip_uint8_t *, zip_uint16_t, zip_flags_t);
ZIP_EXTERN zip_int16_t zip_file_extra_fields_count(zip_t *, zip_uint64_t, zip_flags_t);
ZIP_EXTERN zip_int16_t zip_file_extra_fields_count_by_id(zip_t *, zip_uint64_t, zip_uint16_t, zip_flags_t);
ZIP_EXTERN const zip_uint8_t *zip_file_extra_field_get(zip_t *, zip_uint64_t, zip_uint16_t, zip_uint16_t *, zip_uint16_t *, zip_flags_t);
ZIP_EXTERN const zip_uint8_t *zip_file_extra_field_get_by_id(zip_t *, zip_uint64_t, zip_uint16_t, zip_uint16_t, zip_uint16_t *, zip_flags_t);
ZIP_EXTERN const char *zip_file_get_comment(zip_t *, zip_uint64_t, zip_uint32_t *, zip_flags_t);
ZIP_EXTERN zip_error_t *zip_file_get_error(zip_file_t *);
ZIP_EXTERN int zip_file_get_external_attributes(zip_t *, zip_uint64_t, zip_flags_t, zip_uint8_t *, zip_uint32_t *);
ZIP_EXTERN int zip_file_rename(zip_t *, zip_uint64_t, const char *, zip_flags_t);
ZIP_EXTERN int zip_file_replace(zip_t *, zip_uint64_t, zip_source_t *, zip_flags_t);
ZIP_EXTERN int zip_file_set_comment(zip_t *, zip_uint64_t, const char *, zip_uint16_t, zip_flags_t);
ZIP_EXTERN int zip_file_set_external_attributes(zip_t *, zip_uint64_t, zip_flags_t, zip_uint8_t, zip_uint32_t);
ZIP_EXTERN int zip_file_set_mtime(zip_t *, zip_uint64_t, time_t, zip_flags_t);
ZIP_EXTERN const char *zip_file_strerror(zip_file_t *);
ZIP_EXTERN zip_file_t *zip_fopen(zip_t *, const char *, zip_flags_t);
ZIP_EXTERN zip_file_t *zip_fopen_encrypted(zip_t *, const char *, zip_flags_t, const char *);
ZIP_EXTERN zip_file_t *zip_fopen_index(zip_t *, zip_uint64_t, zip_flags_t);
ZIP_EXTERN zip_file_t *zip_fopen_index_encrypted(zip_t *, zip_uint64_t, zip_flags_t, const char *);
ZIP_EXTERN zip_int64_t zip_fread(zip_file_t *, void *, zip_uint64_t);
ZIP_EXTERN const char *zip_get_archive_comment(zip_t *, int *, zip_flags_t);
ZIP_EXTERN int zip_get_archive_flag(zip_t *, zip_flags_t, zip_flags_t);
ZIP_EXTERN const char *zip_get_name(zip_t *, zip_uint64_t, zip_flags_t);
ZIP_EXTERN zip_int64_t zip_get_num_entries(zip_t *, zip_flags_t);
ZIP_EXTERN zip_int64_t zip_name_locate(zip_t *, const char *, zip_flags_t);
ZIP_EXTERN zip_t *zip_open(const char *, int, int *);
ZIP_EXTERN zip_t *zip_open_from_source(zip_source_t *, int, zip_error_t *);
ZIP_EXTERN int zip_set_archive_comment(zip_t *, const char *, zip_uint16_t);
ZIP_EXTERN int zip_set_archive_flag(zip_t *, zip_flags_t, int);
ZIP_EXTERN int zip_set_default_password(zip_t *, const char *);
ZIP_EXTERN int zip_set_file_compression(zip_t *, zip_uint64_t, zip_int32_t, zip_uint32_t);
ZIP_EXTERN int zip_source_begin_write(zip_source_t *);
ZIP_EXTERN zip_source_t *zip_source_buffer(zip_t *, const void *, zip_uint64_t, int);
ZIP_EXTERN zip_source_t *zip_source_buffer_create(const void *, zip_uint64_t, int, zip_error_t *);
ZIP_EXTERN int zip_source_close(zip_source_t *);
ZIP_EXTERN int zip_source_commit_write(zip_source_t *);
ZIP_EXTERN zip_error_t *zip_source_error(zip_source_t *src);
ZIP_EXTERN zip_source_t *zip_source_file(zip_t *, const char *, zip_uint64_t, zip_int64_t);
ZIP_EXTERN zip_source_t *zip_source_file_create(const char *, zip_uint64_t, zip_int64_t, zip_error_t *);
ZIP_EXTERN zip_source_t *zip_source_filep(zip_t *, FILE *, zip_uint64_t, zip_int64_t);
ZIP_EXTERN zip_source_t *zip_source_filep_create(FILE *, zip_uint64_t, zip_int64_t, zip_error_t *);
ZIP_EXTERN void zip_source_free(zip_source_t *);
ZIP_EXTERN zip_source_t *zip_source_function(zip_t *, zip_source_callback, void *);
ZIP_EXTERN zip_source_t *zip_source_function_create(zip_source_callback, void *, zip_error_t *);
ZIP_EXTERN int zip_source_is_deleted(zip_source_t *);
ZIP_EXTERN void zip_source_keep(zip_source_t *);
ZIP_EXTERN zip_int64_t zip_source_make_command_bitmap(zip_source_cmd_t, ...);
ZIP_EXTERN int zip_source_open(zip_source_t *);
ZIP_EXTERN zip_int64_t zip_source_read(zip_source_t *, void *, zip_uint64_t);
ZIP_EXTERN void zip_source_rollback_write(zip_source_t *);
ZIP_EXTERN int zip_source_seek(zip_source_t *, zip_int64_t, int);
ZIP_EXTERN zip_int64_t zip_source_seek_compute_offset(zip_uint64_t, zip_uint64_t, void *, zip_uint64_t, zip_error_t *);
ZIP_EXTERN int zip_source_seek_write(zip_source_t *, zip_int64_t, int);
ZIP_EXTERN int zip_source_stat(zip_source_t *, zip_stat_t *);
ZIP_EXTERN zip_int64_t zip_source_tell(zip_source_t *);
ZIP_EXTERN zip_int64_t zip_source_tell_write(zip_source_t *);
#ifdef _WIN32
ZIP_EXTERN zip_source_t *zip_source_win32a(zip_t *, const char *, zip_uint64_t, zip_int64_t);
ZIP_EXTERN zip_source_t *zip_source_win32a_create(const char *, zip_uint64_t, zip_int64_t, zip_error_t *);
ZIP_EXTERN zip_source_t *zip_source_win32handle(zip_t *, void *, zip_uint64_t, zip_int64_t);
ZIP_EXTERN zip_source_t *zip_source_win32handle_create(void *, zip_uint64_t, zip_int64_t, zip_error_t *);
ZIP_EXTERN zip_source_t *zip_source_win32w(zip_t *, const wchar_t *, zip_uint64_t, zip_int64_t);
ZIP_EXTERN zip_source_t *zip_source_win32w_create(const wchar_t *, zip_uint64_t, zip_int64_t, zip_error_t *);
#endif
ZIP_EXTERN zip_int64_t zip_source_write(zip_source_t *, const void *, zip_uint64_t);
ZIP_EXTERN zip_source_t *zip_source_zip(zip_t *, zip_t *, zip_uint64_t, zip_flags_t, zip_uint64_t, zip_int64_t);
ZIP_EXTERN int zip_stat(zip_t *, const char *, zip_flags_t, zip_stat_t *);
ZIP_EXTERN int zip_stat_index(zip_t *, zip_uint64_t, zip_flags_t, zip_stat_t *);
ZIP_EXTERN void zip_stat_init(zip_stat_t *);
ZIP_EXTERN const char *zip_strerror(zip_t *);
ZIP_EXTERN int zip_unchange(zip_t *, zip_uint64_t);
ZIP_EXTERN int zip_unchange_all(zip_t *);
ZIP_EXTERN int zip_unchange_archive(zip_t *);

#ifdef __cplusplus
}
#endif

#endif /* _HAD_ZIP_H */
