#ifndef _HAD_ZIP_H
#define _HAD_ZIP_H

/*
  zip.h -- exported declarations.
  Copyright (C) 1999-2008 Dieter Baron and Thomas Klausner

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
#	include "zip_win32.h"
# ifdef PHP_ZIP_EXPORTS
#		define ZIP_EXTERN(rt) __declspec(dllexport)rt _stdcall
# else
#		define ZIP_EXTERN(rt) rt
# endif
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define ZIP_EXTERN(rt) __attribute__ ((visibility("default"))) rt
#else
#	define ZIP_EXTERN(rt) rt
#endif

BEGIN_EXTERN_C()

#include <sys/types.h>
#include <stdio.h>
#include <time.h>

/* flags for zip_open */

#define ZIP_CREATE           1
#define ZIP_EXCL             2
#define ZIP_CHECKCONS        4
#define ZIP_OVERWRITE        8


/* flags for zip_name_locate, zip_fopen, zip_stat, ... */

#define ZIP_FL_NOCASE		1 /* ignore case on name lookup */
#define ZIP_FL_NODIR		2 /* ignore directory component */
#define ZIP_FL_COMPRESSED	4 /* read compressed data */
#define ZIP_FL_UNCHANGED	8 /* use original data, ignoring changes */
#define ZIP_FL_RECOMPRESS      16 /* force recompression of data */

/* archive global flags flags */

#define ZIP_AFL_TORRENT		1 /* torrent zipped */

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



enum zip_source_cmd {
    ZIP_SOURCE_OPEN,	/* prepare for reading */
    ZIP_SOURCE_READ, 	/* read data */
    ZIP_SOURCE_CLOSE,	/* reading is done */
    ZIP_SOURCE_STAT,	/* get meta information */
    ZIP_SOURCE_ERROR,	/* get error information */
    ZIP_SOURCE_FREE	/* cleanup and free resources */
};

typedef ssize_t (*zip_source_callback)(void *state, void *data,
				       size_t len, enum zip_source_cmd cmd);

struct zip_stat {
    const char *name;			/* name of the file */
    int index;				/* index within archive */
    unsigned int crc;			/* crc of file data */
    time_t mtime;			/* modification time */
    off_t size;				/* size of file (uncompressed) */
    off_t comp_size;			/* size of file (compressed) */
    unsigned short comp_method;		/* compression method used */
    unsigned short encryption_method;	/* encryption method used */
};

struct zip;
struct zip_file;
struct zip_source;



ZIP_EXTERN(int) zip_add(struct zip *, const char *, struct zip_source *);
ZIP_EXTERN(int) zip_add_dir(struct zip *, const char *);
ZIP_EXTERN(int) zip_close(struct zip *);
ZIP_EXTERN(int) zip_delete(struct zip *, int);
ZIP_EXTERN(void) zip_error_clear(struct zip *);
ZIP_EXTERN(void) zip_error_get(struct zip *, int *, int *);
ZIP_EXTERN(int) zip_error_get_sys_type(int);
ZIP_EXTERN(int) zip_error_to_str(char *, size_t, int, int);
ZIP_EXTERN(int) zip_fclose(struct zip_file *);
ZIP_EXTERN(void) zip_file_error_clear(struct zip_file *);
ZIP_EXTERN(void) zip_file_error_get(struct zip_file *, int *, int *);
ZIP_EXTERN(const char *)zip_file_strerror(struct zip_file *);
ZIP_EXTERN(struct zip_file *)zip_fopen(struct zip *, const char *, int);
ZIP_EXTERN(struct zip_file *)zip_fopen_index(struct zip *, int, int);
ZIP_EXTERN(ssize_t) zip_fread(struct zip_file *, void *, size_t);
ZIP_EXTERN(const char *)zip_get_archive_comment(struct zip *, int *, int);
ZIP_EXTERN(int) zip_get_archive_flag(struct zip *, int, int);
ZIP_EXTERN(const char *)zip_get_file_comment(struct zip *, int, int *, int);
ZIP_EXTERN(const char *)zip_get_name(struct zip *, int, int);
ZIP_EXTERN(int) zip_get_num_files(struct zip *);
ZIP_EXTERN(int) zip_name_locate(struct zip *, const char *, int);
ZIP_EXTERN(struct zip *)zip_open(const char *, int, int *);
ZIP_EXTERN(int) zip_rename(struct zip *, int, const char *);
ZIP_EXTERN(int) zip_replace(struct zip *, int, struct zip_source *);
ZIP_EXTERN(int) zip_set_archive_comment(struct zip *, const char *, int);
ZIP_EXTERN(int) zip_set_archive_flag(struct zip *, int, int);
ZIP_EXTERN(int) zip_set_file_comment(struct zip *, int, const char *, int);
ZIP_EXTERN(struct zip_source *)zip_source_buffer(struct zip *, const void *,
						off_t, int);
ZIP_EXTERN(struct zip_source *)zip_source_file(struct zip *, const char *,
					      off_t, off_t);
ZIP_EXTERN(struct zip_source *)zip_source_filep(struct zip *, FILE *,
					       off_t, off_t);
ZIP_EXTERN(void) zip_source_free(struct zip_source *);
ZIP_EXTERN(struct zip_source *)zip_source_function(struct zip *,
				       zip_source_callback, void *);
ZIP_EXTERN(struct zip_source *)zip_source_zip(struct zip *, struct zip *,
					     int, int, off_t, off_t);
ZIP_EXTERN(int) zip_stat(struct zip *, const char *, int, struct zip_stat *);
ZIP_EXTERN(int) zip_stat_index(struct zip *, int, int, struct zip_stat *);
ZIP_EXTERN(void) zip_stat_init(struct zip_stat *);
ZIP_EXTERN(const char *)zip_strerror(struct zip *);
ZIP_EXTERN(int) zip_unchange(struct zip *, int);
ZIP_EXTERN(int) zip_unchange_all(struct zip *);
ZIP_EXTERN(int) zip_unchange_archive(struct zip *);

END_EXTERN_C();
#endif /* _HAD_ZIP_H */
