/*
  +----------------------------------------------------------------------+
  | phar php single-file executable PHP extension                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2016 The PHP Group                                |
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

typedef struct _phar_zip_file_header {
	char signature[4];       /* local file header signature     4 bytes  (0x04034b50) */
	char zipversion[2];      /* version needed to extract       2 bytes */
	char flags[2];        /* general purpose bit flag        2 bytes */
	char compressed[2];   /* compression method              2 bytes */
	char timestamp[2];    /* last mod file time              2 bytes */
	char datestamp[2];    /* last mod file date              2 bytes */
	char crc32[4];        /* crc-32                          4 bytes */
	char compsize[4];     /* compressed size                 4 bytes */
	char uncompsize[4];   /* uncompressed size               4 bytes */
	char filename_len[2]; /* file name length                2 bytes */
	char extra_len[2];    /* extra field length              2 bytes */
/* file name (variable size) */
/* extra field (variable size) */
} phar_zip_file_header;

/* unused in this release */
typedef struct _phar_zip_file_datadesc {
	char signature[4];  /* signature (optional)            4 bytes */
	char crc32[4];      /* crc-32                          4 bytes */
	char compsize[4];   /* compressed size                 4 bytes */
	char uncompsize[4]; /* uncompressed size               4 bytes */
} phar_zip_data_desc;

/* unused in this release */
typedef struct _phar_zip_file_datadesc_zip64 {
	char crc32[4];       /* crc-32                          4 bytes */
	char compsize[4];    /* compressed size                 8 bytes */
	char compsize2[4];
	char uncompsize[4];  /* uncompressed size               8 bytes */
	char uncompsize2[4];
} phar_zip_data_desc_zip64;

typedef struct _phar_zip_archive_extra_data_record {
	char signature[4];   /* archive extra data signature    4 bytes  (0x08064b50) */
	char len[4];      /* extra field length              4 bytes */
/* extra field data                (variable size) */
} phar_zip_archive_extra_data_record;

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

typedef struct _phar_zip_extra_field_header {
	char tag[2];
	char size[2];
} phar_zip_extra_field_header;

typedef struct _phar_zip_unix3 {
	char tag[2];            /* 0x756e        Short       tag for this extra block type ("nu") */
	char size[2];        /* TSize         Short       total data size for this block */
	char crc32[4];       /* CRC           Long        CRC-32 of the remaining data */
	char perms[2];       /* Mode          Short       file permissions */
	char symlinksize[4]; /* SizDev        Long        symlink'd size OR major/minor dev num */
	char uid[2];         /* UID           Short       user ID */
	char gid[2];         /* GID           Short       group ID */
/* (var.)        variable    symbolic link filename */
} phar_zip_unix3;

typedef struct _phar_zip_central_dir_file {
	char signature[4];            /* central file header signature   4 bytes  (0x02014b50) */
	char madeby[2];               /* version made by                 2 bytes */
	char zipversion[2];           /* version needed to extract       2 bytes */
	char flags[2];             /* general purpose bit flag        2 bytes */
	char compressed[2];        /* compression method              2 bytes */
	char timestamp[2];         /* last mod file time              2 bytes */
	char datestamp[2];         /* last mod file date              2 bytes */
	char crc32[4];             /* crc-32                          4 bytes */
	char compsize[4];          /* compressed size                 4 bytes */
	char uncompsize[4];        /* uncompressed size               4 bytes */
	char filename_len[2];      /* file name length                2 bytes */
	char extra_len[2];         /* extra field length              2 bytes */
	char comment_len[2];       /* file comment length             2 bytes */
	char disknumber[2];        /* disk number start               2 bytes */
	char internal_atts[2];     /* internal file attributes        2 bytes */
	char external_atts[4];     /* external file attributes        4 bytes */
	char offset[4];            /* relative offset of local header 4 bytes */

/* file name (variable size) */
/* extra field (variable size) */
/* file comment (variable size) */
} phar_zip_central_dir_file;

typedef struct _phar_zip_dir_signature {
	char signature[4];  /* header signature                4 bytes  (0x05054b50) */
	char size[2];    /* size of data                    2 bytes */
} phar_zip_dir_signature;

/* unused in this release */
typedef struct _phar_zip64_dir_end {
	char signature[4];        /* zip64 end of central dir 
                                 signature                       4 bytes  (0x06064b50) */
	char size1[4];         /* size of zip64 end of central
                                 directory record                8 bytes */
	char size2[4];
	char madeby[2];           /* version made by                 2 bytes */
	char extractneeded[2]; /* version needed to extract       2 bytes */
	char disknum[4];       /* number of this disk             4 bytes */
	char cdir_num[4];      /* number of the disk with the 
                                 start of the central directory  4 bytes */
	char entries1[4];      /* total number of entries in the
                                 central directory on this disk  8 bytes */
	char entries2[4];
	char entriestotal1[4]; /* total number of entries in the
                                 central directory               8 bytes */
	char entriestotal2[4];
	char cdirsize1[4];     /* size of the central directory   8 bytes */
	char cdirsize2[4];
	char offset1[4];       /* offset of start of central
                                 directory with respect to
                                 the starting disk number        8 bytes */
	char offset2[4];
/* zip64 extensible data sector    (variable size) */
} phar_zip64_dir_end;

/* unused in this release */
typedef struct _phar_zip64_dir_locator {
	char signature[4];     /* zip64 end of central dir locator 
                              signature                       4 bytes  (0x07064b50) */
	char disknum[4];    /* number of the disk with the
                              start of the zip64 end of 
                              central directory               4 bytes */
	char diroffset1[4]; /* relative offset of the zip64
                              end of central directory record 8 bytes */
	char diroffset2[4];
	char totaldisks[4]; /* total number of disks           4 bytes */
} phar_zip64_dir_locator;

typedef struct _phar_zip_dir_end {
	char signature[4];           /* end of central dir signature    4 bytes  (0x06054b50) */
	char disknumber[2];       /* number of this disk             2 bytes */
	char centraldisk[2];      /* number of the disk with the
                                    start of the central directory  2 bytes */
	char counthere[2];        /* total number of entries in the
                                    central directory on this disk  2 bytes */
	char count[2];            /* total number of entries in
                                    the central directory           2 bytes */
	char cdir_size[4];        /* size of the central directory   4 bytes */
	char cdir_offset[4];      /* offset of start of central
                                    directory with respect to
                                    the starting disk number        4 bytes */
	char comment_len[2];      /* .ZIP file comment length        2 bytes */
/* .ZIP file comment       (variable size) */
} phar_zip_dir_end;
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
