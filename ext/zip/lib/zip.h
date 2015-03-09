#ifndef _HAD_ZIP_H
#define _HAD_ZIP_H

/*
  zip.h -- exported declarations.
  Copyright (C) 1999-2012 Dieter Baron and Thomas Klausner

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



#include "main/php.h"

#ifdef PHP_WIN32
#ifdef PHP_ZIP_EXPORTS
#  define ZIP_EXTERN __declspec(dllexport) _stdcall
# else
#  define ZIP_EXTERN
# endif
#elif defined(__GNUC__) && __GNUC__ >= 4
# define ZIP_EXTERN __attribute__ ((visibility("default")))
#else
# define ZIP_EXTERN
#endif



#ifdef __cplusplus
extern "C" {
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

#define ZIP_AFL_TORRENT		1u /* torrent zipped */
#define ZIP_AFL_RDONLY		2u /* read only -- cannot be cleared */


/* create a new extra field */

#define ZIP_EXTRA_FIELD_ALL	ZIP_UINT16_MAX
#define ZIP_EXTRA_FIELD_NEW	ZIP_UINT16_MAX

/* flags for compression and encryption sources */

#define ZIP_CODEC_DECODE	0 /* decompress/decrypt (encode flag not set) */
#define ZIP_CODEC_ENCODE	1 /* compress/encrypt */


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
#define ZIP_ER_EOF           17  /* N Premature EOF */
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
    ZIP_SOURCE_OPEN,	/* prepare for reading */
    ZIP_SOURCE_READ, 	/* read data */
    ZIP_SOURCE_CLOSE,	/* reading is done */
    ZIP_SOURCE_STAT,	/* get meta information */
    ZIP_SOURCE_ERROR,	/* get error information */
    ZIP_SOURCE_FREE	/* cleanup and free resources */
};

#define ZIP_SOURCE_ERR_LOWER	-2

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

typedef zip_uint32_t zip_flags_t;

typedef zip_int64_t (*zip_source_callback)(void *, void *, zip_uint64_t,
					   enum zip_source_cmd);



#ifndef ZIP_DISABLE_DEPRECATED
ZIP_EXTERN zip_int64_t zip_add(struct zip *, const char *, struct zip_source *); /* use zip_file_add */
ZIP_EXTERN zip_int64_t zip_add_dir(struct zip *, const char *); /* use zip_dir_add */
ZIP_EXTERN const char *zip_get_file_comment(struct zip *, zip_uint64_t, int *, int); /* use zip_file_get_comment */
ZIP_EXTERN int zip_get_num_files(struct zip *);  /* use zip_get_num_entries instead */
ZIP_EXTERN int zip_rename(struct zip *, zip_uint64_t, const char *); /* use zip_file_rename */
ZIP_EXTERN int zip_replace(struct zip *, zip_uint64_t, struct zip_source *); /* use zip_file_replace */
ZIP_EXTERN int zip_set_file_comment(struct zip *, zip_uint64_t, const char *, int); /* use zip_file_set_comment */
#endif

ZIP_EXTERN int zip_archive_set_tempdir(struct zip *, const char *);
ZIP_EXTERN int zip_close(struct zip *);
ZIP_EXTERN int zip_delete(struct zip *, zip_uint64_t);
ZIP_EXTERN zip_int64_t zip_dir_add(struct zip *, const char *, zip_flags_t);
ZIP_EXTERN void zip_discard(struct zip *);
ZIP_EXTERN void zip_error_clear(struct zip *);
ZIP_EXTERN void zip_error_get(struct zip *, int *, int *);
ZIP_EXTERN int zip_error_get_sys_type(int);
ZIP_EXTERN int zip_error_to_str(char *, zip_uint64_t, int, int);
ZIP_EXTERN int zip_fclose(struct zip_file *);
ZIP_EXTERN struct zip *zip_fdopen(int, int, int *);
ZIP_EXTERN zip_int64_t zip_file_add(struct zip *, const char *, struct zip_source *, zip_flags_t);
ZIP_EXTERN void zip_file_error_clear(struct zip_file *);
ZIP_EXTERN void zip_file_error_get(struct zip_file *, int *, int *);
ZIP_EXTERN int zip_file_extra_field_delete(struct zip *, zip_uint64_t, zip_uint16_t, zip_flags_t);
ZIP_EXTERN int zip_file_extra_field_delete_by_id(struct zip *, zip_uint64_t, zip_uint16_t, zip_uint16_t, zip_flags_t);
ZIP_EXTERN int zip_file_extra_field_set(struct zip *, zip_uint64_t, zip_uint16_t, zip_uint16_t, const zip_uint8_t *, zip_uint16_t, zip_flags_t);
ZIP_EXTERN zip_int16_t zip_file_extra_fields_count(struct zip *, zip_uint64_t, zip_flags_t);
ZIP_EXTERN zip_int16_t zip_file_extra_fields_count_by_id(struct zip *, zip_uint64_t, zip_uint16_t, zip_flags_t);
ZIP_EXTERN const zip_uint8_t *zip_file_extra_field_get(struct zip *, zip_uint64_t, zip_uint16_t, zip_uint16_t *, zip_uint16_t *, zip_flags_t);
ZIP_EXTERN const zip_uint8_t *zip_file_extra_field_get_by_id(struct zip *, zip_uint64_t, zip_uint16_t, zip_uint16_t, zip_uint16_t *, zip_flags_t);
ZIP_EXTERN const char *zip_file_get_comment(struct zip *, zip_uint64_t, zip_uint32_t *, zip_flags_t);
ZIP_EXTERN int zip_file_get_external_attributes(struct zip *, zip_uint64_t, zip_flags_t, zip_uint8_t *, zip_uint32_t *);
ZIP_EXTERN int zip_file_rename(struct zip *, zip_uint64_t, const char *, zip_flags_t);
ZIP_EXTERN int zip_file_replace(struct zip *, zip_uint64_t, struct zip_source *, zip_flags_t);
ZIP_EXTERN int zip_file_set_comment(struct zip *, zip_uint64_t, const char *, zip_uint16_t, zip_flags_t);
ZIP_EXTERN int zip_file_set_external_attributes(struct zip *, zip_uint64_t, zip_flags_t, zip_uint8_t, zip_uint32_t);
ZIP_EXTERN const char *zip_file_strerror(struct zip_file *);
ZIP_EXTERN struct zip_file *zip_fopen(struct zip *, const char *, zip_flags_t);
ZIP_EXTERN struct zip_file *zip_fopen_encrypted(struct zip *, const char *, zip_flags_t, const char *);
ZIP_EXTERN struct zip_file *zip_fopen_index(struct zip *, zip_uint64_t, zip_flags_t);
ZIP_EXTERN struct zip_file *zip_fopen_index_encrypted(struct zip *, zip_uint64_t, zip_flags_t, const char *);
ZIP_EXTERN zip_int64_t zip_fread(struct zip_file *, void *, zip_uint64_t);
ZIP_EXTERN const char *zip_get_archive_comment(struct zip *, int *, zip_flags_t);
ZIP_EXTERN int zip_get_archive_flag(struct zip *, zip_flags_t, zip_flags_t);
ZIP_EXTERN const char *zip_get_name(struct zip *, zip_uint64_t, zip_flags_t);
ZIP_EXTERN zip_int64_t zip_get_num_entries(struct zip *, zip_flags_t);
ZIP_EXTERN zip_int64_t zip_name_locate(struct zip *, const char *, zip_flags_t);
ZIP_EXTERN struct zip *zip_open(const char *, int, int *);
ZIP_EXTERN int zip_set_archive_comment(struct zip *, const char *, zip_uint16_t);
ZIP_EXTERN int zip_set_archive_flag(struct zip *, zip_flags_t, int);
ZIP_EXTERN int zip_set_default_password(struct zip *, const char *);
ZIP_EXTERN int zip_set_file_compression(struct zip *, zip_uint64_t, zip_int32_t, zip_uint32_t);
ZIP_EXTERN struct zip_source *zip_source_buffer(struct zip *, const void *, zip_uint64_t, int);
ZIP_EXTERN struct zip_source *zip_source_file(struct zip *, const char *, zip_uint64_t, zip_int64_t);
ZIP_EXTERN struct zip_source *zip_source_filep(struct zip *, FILE *, zip_uint64_t, zip_int64_t);
ZIP_EXTERN void zip_source_free(struct zip_source *);
ZIP_EXTERN struct zip_source *zip_source_function(struct zip *, zip_source_callback, void *);
ZIP_EXTERN struct zip_source *zip_source_zip(struct zip *, struct zip *, zip_uint64_t, zip_flags_t, zip_uint64_t, zip_int64_t);
ZIP_EXTERN int zip_stat(struct zip *, const char *, zip_flags_t, struct zip_stat *);
ZIP_EXTERN int zip_stat_index(struct zip *, zip_uint64_t, zip_flags_t, struct zip_stat *);
ZIP_EXTERN void zip_stat_init(struct zip_stat *);
ZIP_EXTERN const char *zip_strerror(struct zip *);
ZIP_EXTERN int zip_unchange(struct zip *, zip_uint64_t);
ZIP_EXTERN int zip_unchange_all(struct zip *);
ZIP_EXTERN int zip_unchange_archive(struct zip *);

#ifdef __cplusplus
}
#endif

#endif /* _HAD_ZIP_H */
