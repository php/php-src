#ifndef __PHAR_TAR_H
#define __PHAR_TAR_H
/*
  +----------------------------------------------------------------------+
  | TAR archive support for Phar                                         |
  +----------------------------------------------------------------------+
  | Copyright (c) 2005-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Dmitry Stogov <dmitry@zend.com>                             |
  |          Gregory Beaver <cellog@php.net>                             |
  +----------------------------------------------------------------------+
*/

#ifdef PHP_WIN32
#pragma pack(1)
# define PHAR_TAR_PACK
#elif defined(__sgi)
# define PHAR_TAR_PACK
#elif defined(__GNUC__)
# define PHAR_TAR_PACK __attribute__((__packed__))
#else
# define PHAR_TAR_PACK
#endif

#if defined(__sgi)
# pragma pack 0
#endif
/**
 * the format of the header block for a file, in the older UNIX-compatible
 * TAR format
 */
typedef struct _old_tar_header {  /* {{{ */
	char name[100];     /* name of file;
	                         directory is indicated by a trailing slash (/) */
	char mode[8];       /* file mode */
	char uid[8];        /* owner user ID */
	char gid[8];        /* owner group ID */
	char size[12];      /* length of file in bytes */
	char mtime[12];     /* modify time of file */
	char checksum[8];   /* checksum for header */
	char link;          /* indicator for links;
	                         1 for a linked file,
	                         2 for a symbolic link,
	                         0 otherwise */
	char linkname[100]; /* name of linked file */
} PHAR_TAR_PACK old_tar_header;
/* }}} */

#if defined(__sgi)
# pragma pack 0
#endif
/**
 * the new USTAR header format.
 * Note that tar can determine that the USTAR format is being used by the
 * presence of the null-terminated string "ustar" in the magic field.
 */
typedef struct _tar_header {  /* {{{ */
	char name[100];     /* name of file */
	char mode[8];       /* file mode   */
	char uid[8];        /* owner user ID */
	char gid[8];        /* owner group ID */
	char size[12];      /* length of file in bytes */
	char mtime[12];     /* modify time of file */
	char checksum[8];   /* checksum for header */
	char typeflag;      /* type of file
	                         0 Regular file
	                         1 Link to another file already archived
	                         2 Symbolic link
	                         3 Character special device
	                         4 Block special device
	                         5 Directory
	                         6 FIFO special file
	                         7 Reserved */
	char linkname[100]; /* name of linked file */
	char magic[6];      /* USTAR indicator */
	char version[2];    /* USTAR version */
	char uname[32];     /* owner user name */
	char gname[32];     /* owner group name */
	char devmajor[8];   /* device major number */
	char devminor[8];   /* device minor number */
	char prefix[155];   /* prefix for file name;
	                       the value of the prefix field, if non-null,
	                       is prefixed to the name field to allow names
	                       longer then 100 characters */
	char padding[12];   /* unused zeroed bytes */
} PHAR_TAR_PACK tar_header;
/* }}} */

#ifdef PHP_WIN32
#pragma pack()
#endif

#endif /* __PHAR_TAR_H */
