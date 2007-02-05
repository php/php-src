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
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>
#include "php.h"
#include "php_ini.h"
#include "zend_constants.h"
#include "zend_execute.h"
#include "zend_exceptions.h"
#include "zend_hash.h"
#include "zend_interfaces.h"
#include "zend_operators.h"
#include "zend_qsort.h"
#include "main/php_streams.h"
#include "ext/standard/info.h"
#include "ext/standard/url.h"
#include "ext/standard/crc32.h"
#include "ext/standard/md5.h"
#include "ext/standard/sha1.h"
#include "ext/standard/php_var.h"
#include "ext/standard/php_smart_str.h"
#if HAVE_SPL
#include "ext/spl/spl_array.h"
#include "ext/spl/spl_directory.h"
#include "ext/spl/spl_engine.h"
#include "ext/spl/spl_exceptions.h"
#endif
#include "php_phar.h"
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#ifndef E_RECOVERABLE_ERROR
#define E_RECOVERABLE_ERROR E_ERROR
#endif

#define PHAR_EXT_VERSION_STR      "1.0.0"
#define PHAR_API_VERSION_STR      "1.0.0"
/* x.y.z maps to 0xyz0 */
#define PHAR_API_VERSION          0x1000
#define PHAR_API_MIN_READ         0x1000
#define PHAR_API_MAJORVERSION     0x1000
#define PHAR_API_MAJORVER_MASK    0xF000
#define PHAR_API_VER_MASK         0xFFF0

#define PHAR_HDR_COMPRESSION_MASK 0x0000F000
#define PHAR_HDR_COMPRESSED_NONE  0x00000000
#define PHAR_HDR_COMPRESSED_GZ    0x00001000
#define PHAR_HDR_COMPRESSED_BZ2   0x00002000
#define PHAR_HDR_SIGNATURE        0x00010000

#define PHAR_SIG_MD5              0x0001
#define PHAR_SIG_SHA1             0x0002
#define PHAR_SIG_PGP              0x0010

#define PHAR_SIG_USE  PHAR_SIG_SHA1

/* flags byte for each file adheres to these bitmasks.
   All unused values are reserved */
#define PHAR_ENT_COMPRESSION_MASK 0x0000F000
#define PHAR_ENT_COMPRESSED_NONE  0x00000000
#define PHAR_ENT_COMPRESSED_GZ    0x00001000
#define PHAR_ENT_COMPRESSED_BZ2   0x00002000

#define PHAR_ENT_PERM_MASK        0x000001FF
#define PHAR_ENT_PERM_MASK_USR    0x000001C0
#define PHAR_ENT_PERM_SHIFT_USR   6
#define PHAR_ENT_PERM_MASK_GRP    0x00000038
#define PHAR_ENT_PERM_SHIFT_GRP   3
#define PHAR_ENT_PERM_MASK_OTH    0x00000007
#define PHAR_ENT_PERM_DEF_FILE    0x000001B6
#define PHAR_ENT_PERM_DEF_DIR     0x000001FF

ZEND_BEGIN_MODULE_GLOBALS(phar)
	HashTable   phar_fname_map;
	HashTable   phar_alias_map;
	int         readonly;
	int         require_hash;
	zend_bool   readonly_orig;
	zend_bool   require_hash_orig;
	int         request_done;
ZEND_END_MODULE_GLOBALS(phar)

ZEND_EXTERN_MODULE_GLOBALS(phar)

#ifdef ZTS
#	include "TSRM.h"
#	define PHAR_G(v) TSRMG(phar_globals_id, zend_phar_globals *, v)
#	define PHAR_GLOBALS ((zend_phar_globals *) (*((void ***) tsrm_ls))[TSRM_UNSHUFFLE_RSRC_ID(phar_globals_id)])
#else
#	define PHAR_G(v) (phar_globals.v)
#	define PHAR_GLOBALS (&phar_globals)
#endif

#ifndef php_uint16
# if SIZEOF_SHORT == 2
#  define php_uint16 unsigned short
# else
#  define php_uint16 uint16_t
# endif
#endif

#if HAVE_SPL
typedef union _phar_archive_object  phar_archive_object;
typedef union _phar_entry_object    phar_entry_object;
#endif

typedef struct _phar_archive_data phar_archive_data;
/* entry for one file in a phar file */
typedef struct _phar_entry_info {
	/* first bytes are exactly as in file */
	php_uint32               uncompressed_filesize;
	php_uint32               timestamp;
	php_uint32               compressed_filesize;
	php_uint32               crc32;
	php_uint32               flags;
	/* remainder */
	zval                     *metadata;
	php_uint32               filename_len;
	char                     *filename;
	long                     offset_within_phar;
	php_stream               *fp;
	php_stream               *cfp;
	int                      fp_refcount;
	int                      is_crc_checked:1;
	int                      is_modified:1;
	int                      is_deleted:1;
	phar_archive_data        *phar;
} phar_entry_info;

/* information about a phar file (the archive itself) */
struct _phar_archive_data {
	char                     *fname;
	int                      fname_len;
	char                     *alias;
	int                      alias_len;
	char                     version[12];
	size_t                   internal_file_start;
	size_t                   halt_offset;
	HashTable                manifest;
	php_uint32               flags;
	php_uint32               min_timestamp;
	php_uint32               max_timestamp;
	php_stream               *fp;
	int                      refcount;
	php_uint32               sig_flags;
	int                      sig_len;
	char                     *signature;
	zval                     *metadata;
	int                      is_explicit_alias:1;
	int                      is_modified:1;
	int                      is_writeable:1;
	int                      is_brandnew:1;
	/* defer phar creation */
	int                      donotflush:1;
};


/* stream access data for one file entry in a phar file */
typedef struct _phar_entry_data {
	phar_archive_data        *phar;
	/* stream position proxy, allows multiple open streams referring to the same fp */
	php_stream               *fp;
	off_t                    position;
	phar_entry_info          *internal_file;
} phar_entry_data;

#if HAVE_SPL
/* archive php object */
union _phar_archive_object {
	zend_object              std;
	spl_filesystem_object    spl;
	struct {
	    zend_object          std;
	    phar_archive_data    *archive;
	} arc;
};
#endif

#if HAVE_SPL
/* entry php object */
union _phar_entry_object {
	zend_object              std;
	spl_filesystem_object    spl;
	struct {
	    zend_object          std;
	    phar_entry_info      *entry;
	} ent;
};
#endif

BEGIN_EXTERN_C()

void phar_object_init(TSRMLS_D);

int phar_open_filename(char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar, char **error TSRMLS_DC);
int phar_open_or_create_filename(char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar, char **error TSRMLS_DC);
int phar_open_compiled_file(char *alias, int alias_len, char **error TSRMLS_DC);

#ifdef PHAR_MAIN

static int phar_open_fp(php_stream* fp, char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar, char **error TSRMLS_DC);

static php_url* phar_open_url(php_stream_wrapper *wrapper, char *filename, char *mode, int options TSRMLS_DC);

static php_stream* phar_wrapper_open_url(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC);
static php_stream* phar_wrapper_open_dir(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC);
static int phar_wrapper_rename(php_stream_wrapper *wrapper, char *url_from, char *url_to, int options, php_stream_context *context TSRMLS_DC);
static int phar_wrapper_unlink(php_stream_wrapper *wrapper, char *url, int options, php_stream_context *context TSRMLS_DC);
static int phar_wrapper_stat(php_stream_wrapper *wrapper, char *url, int flags, php_stream_statbuf *ssb, php_stream_context *context TSRMLS_DC);

/* file/stream handlers */
static size_t phar_stream_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC);
static size_t phar_stream_read( php_stream *stream, char *buf, size_t count TSRMLS_DC);
static int    phar_stream_close(php_stream *stream, int close_handle TSRMLS_DC);
static int    phar_stream_flush(php_stream *stream TSRMLS_DC);
static int    phar_stream_seek( php_stream *stream, off_t offset, int whence, off_t *newoffset TSRMLS_DC);
static int    phar_stream_stat( php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC);

/* directory handlers */
static size_t phar_dir_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC);
static size_t phar_dir_read( php_stream *stream, char *buf, size_t count TSRMLS_DC);
static int    phar_dir_close(php_stream *stream, int close_handle TSRMLS_DC);
static int    phar_dir_flush(php_stream *stream TSRMLS_DC);
static int    phar_dir_seek( php_stream *stream, off_t offset, int whence, off_t *newoffset TSRMLS_DC);
static int    phar_dir_stat( php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC);

#endif

int phar_archive_delref(phar_archive_data *phar TSRMLS_DC);
int phar_entry_delref(phar_entry_data *idata TSRMLS_DC);

phar_entry_info *phar_get_entry_info(phar_archive_data *phar, char *path, int path_len, char **error TSRMLS_DC);
phar_entry_data *phar_get_or_create_entry_data(char *fname, int fname_len, char *path, int path_len, char *mode, char **error TSRMLS_DC);
int phar_flush(phar_archive_data *archive, char *user_stub, long len, char **error TSRMLS_DC);
int phar_split_fname(char *filename, int filename_len, char **arch, int *arch_len, char **entry, int *entry_len TSRMLS_DC);

typedef enum {
	pcr_use_query,
	pcr_is_ok,
	pcr_err_double_slash,
	pcr_err_up_dir,
	pcr_err_curr_dir,
	pcr_err_back_slash,
	pcr_err_star,
	pcr_err_illegal_char,
	pcr_err_empty_entry
} phar_path_check_result;

phar_path_check_result phar_path_check(char **p, int *len, const char **error);

END_EXTERN_C()

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
