/*
  +----------------------------------------------------------------------+
  | phar php single-file executable PHP extension                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2009 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Gregory Beaver <cellog@php.net>                             |
  |          Marcus Boerger <helly@php.net>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef PHP_WIN32
#pragma pack(1)
# define PHAR_ZIP_PACK
#elif defined(__sgi)
# define PHAR_ZIP_PACK
#else
# define PHAR_ZIP_PACK __attribute__((__packed__))
#endif

#if defined(__sgi)
# pragma pack 0
#endif
typedef struct _phar_zip_file_header {
	char signature[4];       /* local file header signature     4 bytes  (0x04034b50) */
	char zipversion[2];      /* version needed to extract       2 bytes */
	php_uint16 flags;        /* general purpose bit flag        2 bytes */
	php_uint16 compressed;   /* compression method              2 bytes */
	php_uint16 timestamp;    /* last mod file time              2 bytes */
	php_uint16 datestamp;    /* last mod file date              2 bytes */
	php_uint32 crc32;        /* crc-32                          4 bytes */
	php_uint32 compsize;     /* compressed size                 4 bytes */
	php_uint32 uncompsize;   /* uncompressed size               4 bytes */
	php_uint16 filename_len; /* file name length                2 bytes */
	php_uint16 extra_len;    /* extra field length              2 bytes */
/* file name (variable size) */
/* extra field (variable size) */
} PHAR_ZIP_PACK phar_zip_file_header;

#if defined(__sgi)
# pragma pack 0
#endif
typedef struct _phar_zip_file_datadesc {
	php_uint32 crc32;      /* crc-32                          4 bytes */
	php_uint32 compsize;   /* compressed size                 4 bytes */
	php_uint32 uncompsize; /* uncompressed size               4 bytes */
} PHAR_ZIP_PACK phar_zip_data_desc;

typedef struct _phar_zip_file_datadesc_zip64 {
	php_uint32 crc32;       /* crc-32                          4 bytes */
	php_uint32 compsize;    /* compressed size                 8 bytes */
	php_uint32 compsize2;
	php_uint32 uncompsize;  /* uncompressed size               8 bytes */
	php_uint32 uncompsize2;
} PHAR_ZIP_PACK phar_zip_data_desc_zip64;

#if defined(__sgi)
# pragma pack 0
#endif
typedef struct _phar_zip_archive_extra_data_record {
	char signature[4];   /* archive extra data signature    4 bytes  (0x08064b50) */
	php_uint32 len;      /* extra field length              4 bytes */
/* extra field data                (variable size) */
} PHAR_ZIP_PACK phar_zip_archive_extra_data_record;

/* madeby/extractneeded value if bzip2 compression is used */
#define PHAR_ZIP_BZIP2 "46"
/* madeby/extractneeded value for other cases */
#define PHAR_ZIP_NORM  "20"

#define PHAR_ZIP_FLAG_ENCRYPTED 0x0001
/* data descriptor present for this file */
#define PHAR_ZIP_FLAG_DATADESC  0x0008
#define PHAR_ZIP_FLAG_UTF8      0x0400

/*
0 - The file is stored (no compression)
1 - The file is Shrunk
2 - The file is Reduced with compression factor 1
3 - The file is Reduced with compression factor 2
4 - The file is Reduced with compression factor 3
5 - The file is Reduced with compression factor 4
6 - The file is Imploded
7 - Reserved for Tokenizing compression algorithm
8 - The file is Deflated
9 - Enhanced Deflating using Deflate64(tm)
10 - PKWARE Data Compression Library Imploding (old IBM TERSE)
11 - Reserved by PKWARE
12 - File is compressed using BZIP2 algorithm
13 - Reserved by PKWARE
14 - LZMA (EFS)
15 - Reserved by PKWARE
16 - Reserved by PKWARE
17 - Reserved by PKWARE
18 - File is compressed using IBM TERSE (new)
19 - IBM LZ77 z Architecture (PFS)
97 - WavPack compressed data
98 - PPMd version I, Rev 1
*/
#define PHAR_ZIP_COMP_NONE    0
#define PHAR_ZIP_COMP_DEFLATE 8
#define PHAR_ZIP_COMP_BZIP2   12

/*
         -ASi Unix Extra Field:
          ====================

          The following is the layout of the ASi extra block for Unix.  The
          local-header and central-header versions are identical.
          (Last Revision 19960916)

          Value         Size        Description
          -----         ----        -----------
  (Unix3) 0x756e        Short       tag for this extra block type ("nu")
          TSize         Short       total data size for this block
          CRC           Long        CRC-32 of the remaining data
          Mode          Short       file permissions
          SizDev        Long        symlink'd size OR major/minor dev num
          UID           Short       user ID
          GID           Short       group ID
          (var.)        variable    symbolic link filename

          Mode is the standard Unix st_mode field from struct stat, containing
          user/group/other permissions, setuid/setgid and symlink info, etc.

          If Mode indicates that this file is a symbolic link, SizDev is the
          size of the file to which the link points.  Otherwise, if the file
          is a device, SizDev contains the standard Unix st_rdev field from
          struct stat (includes the major and minor numbers of the device).
          SizDev is undefined in other cases.

          If Mode indicates that the file is a symbolic link, the final field
          will be the name of the file to which the link points.  The file-
          name length can be inferred from TSize.

          [Note that TSize may incorrectly refer to the data size not counting
           the CRC; i.e., it may be four bytes too small.]
*/

#if defined(__sgi)
# pragma pack 0
#endif
typedef struct _phar_zip_extra_field_header {
	char tag[2];
	php_uint16 size;
} PHAR_ZIP_PACK phar_zip_extra_field_header;

#if defined(__sgi)
# pragma pack 0
#endif
typedef struct _phar_zip_unix3 {
	char tag[2];            /* 0x756e        Short       tag for this extra block type ("nu") */
	php_uint16 size;        /* TSize         Short       total data size for this block */
	php_uint32 crc32;       /* CRC           Long        CRC-32 of the remaining data */
	php_uint16 perms;       /* Mode          Short       file permissions */
	php_uint32 symlinksize; /* SizDev        Long        symlink'd size OR major/minor dev num */
	php_uint16 uid;         /* UID           Short       user ID */
	php_uint16 gid;         /* GID           Short       group ID */
/* (var.)        variable    symbolic link filename */
} PHAR_ZIP_PACK phar_zip_unix3;

#if defined(__sgi)
# pragma pack 0
#endif
typedef struct _phar_zip_central_dir_file {
	char signature[4];            /* central file header signature   4 bytes  (0x02014b50) */
	char madeby[2];               /* version made by                 2 bytes */
	char zipversion[2];           /* version needed to extract       2 bytes */
	php_uint16 flags;             /* general purpose bit flag        2 bytes */
	php_uint16 compressed;        /* compression method              2 bytes */
	php_uint16 timestamp;         /* last mod file time              2 bytes */
	php_uint16 datestamp;         /* last mod file date              2 bytes */
	php_uint32 crc32;             /* crc-32                          4 bytes */
	php_uint32 compsize;          /* compressed size                 4 bytes */
	php_uint32 uncompsize;        /* uncompressed size               4 bytes */
	php_uint16 filename_len;      /* file name length                2 bytes */
	php_uint16 extra_len;         /* extra field length              2 bytes */
	php_uint16 comment_len;       /* file comment length             2 bytes */
	php_uint16 disknumber;        /* disk number start               2 bytes */
	php_uint16 internal_atts;     /* internal file attributes        2 bytes */
	php_uint32 external_atts;     /* external file attributes        4 bytes */
	php_uint32 offset;            /* relative offset of local header 4 bytes */

/* file name (variable size) */
/* extra field (variable size) */
/* file comment (variable size) */
} PHAR_ZIP_PACK phar_zip_central_dir_file;

#if defined(__sgi)
# pragma pack 0
#endif
typedef struct _phar_zip_dir_signature {
	char signature[4];  /* header signature                4 bytes  (0x05054b50) */
	php_uint16 size;    /* size of data                    2 bytes */
} PHAR_ZIP_PACK phar_zip_dir_signature;

#if defined(__sgi)
# pragma pack 0
#endif
typedef struct _phar_zip64_dir_end {
	char signature[4];        /* zip64 end of central dir 
                                 signature                       4 bytes  (0x06064b50) */
	php_uint32 size1;         /* size of zip64 end of central
                                 directory record                8 bytes */
	php_uint32 size2;
	char madeby[2];           /* version made by                 2 bytes */
	php_uint16 extractneeded; /* version needed to extract       2 bytes */
	php_uint32 disknum;       /* number of this disk             4 bytes */
	php_uint32 cdir_num;      /* number of the disk with the 
                                 start of the central directory  4 bytes */
	php_uint32 entries1;      /* total number of entries in the
                                 central directory on this disk  8 bytes */
	php_uint32 entries2;
	php_uint32 entriestotal1; /* total number of entries in the
                                 central directory               8 bytes */
	php_uint32 entriestotal2;
	php_uint32 cdirsize1;     /* size of the central directory   8 bytes */
	php_uint32 cdirsize2;
	php_uint32 offset1;       /* offset of start of central
                                 directory with respect to
                                 the starting disk number        8 bytes */
	php_uint32 offset2;
/* zip64 extensible data sector    (variable size) */
} PHAR_ZIP_PACK phar_zip64_dir_end;

#if defined(__sgi)
# pragma pack 0
#endif
typedef struct _phar_zip64_dir_locator {
	char signature[4];     /* zip64 end of central dir locator 
                              signature                       4 bytes  (0x07064b50) */
	php_uint32 disknum;    /* number of the disk with the
                              start of the zip64 end of 
                              central directory               4 bytes */
	php_uint32 diroffset1; /* relative offset of the zip64
                              end of central directory record 8 bytes */
	php_uint32 diroffset2;
	php_uint32 totaldisks; /* total number of disks           4 bytes */
} PHAR_ZIP_PACK phar_zip64_dir_locator;

#if defined(__sgi)
# pragma pack 0
#endif
typedef struct _phar_zip_dir_end {
	char signature[4];           /* end of central dir signature    4 bytes  (0x06054b50) */
	php_uint16 disknumber;       /* number of this disk             2 bytes */
	php_uint16 centraldisk;      /* number of the disk with the
                                    start of the central directory  2 bytes */
	php_uint16 counthere;        /* total number of entries in the
                                    central directory on this disk  2 bytes */
	php_uint16 count;            /* total number of entries in
                                    the central directory           2 bytes */
	php_uint32 cdir_size;        /* size of the central directory   4 bytes */
	php_uint32 cdir_offset;      /* offset of start of central
                                    directory with respect to
                                    the starting disk number        4 bytes */
	php_uint16 comment_len;      /* .ZIP file comment length        2 bytes */
/* .ZIP file comment       (variable size) */
} PHAR_ZIP_PACK phar_zip_dir_end;
#ifdef PHP_WIN32
#pragma pack()
#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
