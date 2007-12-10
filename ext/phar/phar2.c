/*
  +----------------------------------------------------------------------+
  | phar php single-file executable PHP extension                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2007 The PHP Group                                |
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
  |          based on work by Dmitry Stogov <dmitry@zend.com>            |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "phar_internal.h"


#define TAR_FILE    '0'
#define TAR_LINK    '1'
#define TAR_SYMLINK '2'
#define TAR_DIR     '5'
#define TAR_NEW     '8'

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
	char chksum[8];     /* checksum for header */
	char link;          /* indicator for links;
	                         1 for a linked file,
	                         2 for a symbolic link,
	                         0 otherwise */
	char linkname[100]; /* name of linked file */
} old_tar_header;
/* }}} */

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
	char chksum[8];     /* checksum for header */
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
} tar_header;
/* }}} */

typedef struct _tar_file tar_file;

typedef struct _tar_entry { /* {{{ */
	tar_file *tar;
	char      type;
	off_t	  start;
	off_t	  pos;
	off_t	  size;
	mode_t	  mode;
	time_t    mtime;
	uid_t	  uid;
	gid_t	  gid;
	union {
		HashTable *content;
		char      *link;
	} u;
} tar_entry;
/* }}} */

struct _tar_file { /* {{{ */
	php_stream *stream;
	char       *real_name;
	int         refcount;
	int         opened;
	tar_entry   root;
};
/* }}} */

static int tar_number(char* buf, int len) /* {{{ */
{
	int num = 0;
	int i = 0;

	while (i < len && buf[i] == ' ') {
		i++;
	}
	while (i < len &&
	       buf[i] >= '0' &&
	       buf[i] <= '7') {
		num = num * 8 + (buf[i] - '0');
		i++;
	}
	return num;
}
/* }}} */

static int tar_checksum(char* buf, int len) /* {{{ */
{
	int sum = 0;
	char *end = buf + len;

	while (buf != end) {
		sum += (unsigned char)*buf;
		buf++;
	}
	return sum;
}
/* }}} */
#define MAPPHAR_ALLOC_FAIL(msg) \
	php_stream_close(fp);\
	if (error) {\
		spprintf(error, 0, msg, fname);\
	}\
	return FAILURE;

#ifdef WORDS_BIGENDIAN
# define PHAR_GET_32(buffer, var) \
	var = ((((unsigned char*)(buffer))[3]) << 24) \
		| ((((unsigned char*)(buffer))[2]) << 16) \
		| ((((unsigned char*)(buffer))[1]) <<  8) \
		| (((unsigned char*)(buffer))[0]); \
	(buffer) += 4
# define PHAR_GET_16(buffer, var) \
	var = ((((unsigned char*)(buffer))[1]) <<  8) \
		| (((unsigned char*)(buffer))[0]); \
	(buffer) += 2
#else
# define PHAR_GET_32(buffer, var) \
	var = *(php_uint32*)(buffer); \
	buffer += 4
# define PHAR_GET_16(buffer, var) \
	var = *(php_uint16*)(buffer); \
	buffer += 2
#endif

int phar2_open_file(php_stream *fp, char *fname, int fname_len, char *alias, int alias_len, long halt_offset, phar_archive_data** pphar, char *buffer, char **error TSRMLS_DC) /* {{{ */
{
	/* The structure of a tar-based phar archive is as follows:
	   - the first file contains the loader stub and file hash.  The file hash is a mapping
	     of filename hash to location of manifest data within the official phar manifest using
             an implementation of open hash that allows the entire thing to be read in without any
             processing beyond signature check.
	   - the second file contains the official phar manifest as used in phar 1.x
	   - third file to second-to-last file contain the phar's files
	   - final file contains the phar signature
	*/
	php_uint32 openhash_len;
	phar_archive_data *mydata;

	buffer += 6;
	PHAR_GET_32(buffer), openhash_len);
	if (openhash_len != sizeof(phar2_openhash) && openhash_len != sizeof(phar2_openhash_large)) {
		MAPPHAR_ALLOC_FAIL("Corrupted phar2 archive, filename hash is wrong size in phar \"%s\"");
	}
	buffer -= 6;
	buffer = (char *) erealloc((void *) buffer, openhash_len);
	if (!buffer) {
		MAPPHAR_ALLOC_FAIL("Could not allocate space for fast filename hash in phar \"%s\"");
	}
	if (openhash_len != php_stream_read(fp, buffer, openhash_len)) {
		MAPPHAR_ALLOC_FAIL("Could not read fast filename hash in phar \"%s\"");
	}

	PHAR_G(fast_fname_map) = (phar2_openhash *) buffer;
	mydata = ecalloc(sizeof(phar_archive_data), 1);

	/* check whether we have meta data, zero check works regardless of byte order */
	if (phar_parse_metadata(fp, &buffer, endbuffer, &mydata->metadata TSRMLS_CC) == FAILURE) {
		MAPPHAR_FAIL("unable to read phar metadata in .phar file \"%s\"");
	}
	
	/* set up our manifest - entries will be created JIT as accessed */
	zend_hash_init(&mydata->manifest, sizeof(phar_entry_info),
		zend_get_hash_value, destroy_phar_manifest, 0);
	snprintf(mydata->version, sizeof(mydata->version), "%u.%u.%u", manifest_ver >> 12, (manifest_ver >> 8) & 0xF, (manifest_ver >> 4) & 0xF);
	mydata->internal_file_start = halt_offset + manifest_len + 4;
	mydata->halt_offset = halt_offset;
	mydata->flags = manifest_flags;
	mydata->fp = fp;
	mydata->fname = estrndup(fname, fname_len);
#ifdef PHP_WIN32
	phar_unixify_path_separators(mydata->fname, fname_len);
#endif
	mydata->fname_len = fname_len;
	mydata->alias = alias ? estrndup(alias, alias_len) : mydata->fname;
	mydata->alias_len = alias ? alias_len : fname_len;
	mydata->sig_flags = sig_flags;
	mydata->sig_len = sig_len;
	mydata->signature = signature;
	phar_request_initialize(TSRMLS_C);
	zend_hash_add(&(PHAR_GLOBALS->phar_fname_map), mydata->fname, fname_len, (void*)&mydata, sizeof(phar_archive_data*),  NULL);
	if (register_alias) {
		mydata->is_explicit_alias = 1;
		zend_hash_add(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, (void*)&mydata, sizeof(phar_archive_data*), NULL);
	} else {
		mydata->is_explicit_alias = 0;
	}
	efree(savebuf);
	
	if (pphar) {
		*pphar = mydata;
	}

	return SUCCESS;
}
/* }}} */