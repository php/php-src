/*
  +----------------------------------------------------------------------+
  | phar php single-file executable PHP extension                        |
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Gregory Beaver <cellog@php.net>                             |
  |          Marcus Boerger <helly@php.net>                              |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>
#include "php.h"
#include "tar.h"
#include "php_ini.h"
#include "zend_constants.h"
#include "zend_execute.h"
#include "zend_exceptions.h"
#include "zend_hash.h"
#include "zend_interfaces.h"
#include "zend_operators.h"
#include "zend_sort.h"
#include "zend_vm.h"
#include "zend_smart_str.h"
#include "main/php_streams.h"
#include "main/streams/php_stream_plain_wrapper.h"
#include "main/SAPI.h"
#include "main/php_main.h"
#include "main/php_open_temporary_file.h"
#include "ext/standard/info.h"
#include "ext/standard/basic_functions.h"
#include "ext/standard/file.h"
#include "ext/standard/php_string.h"
#include "ext/standard/url.h"
#include "ext/standard/crc32.h"
#include "ext/standard/md5.h"
#include "ext/standard/sha1.h"
#include "ext/standard/php_var.h"
#include "ext/standard/php_versioning.h"
#include "Zend/zend_virtual_cwd.h"
#include "ext/spl/spl_array.h"
#include "ext/spl/spl_directory.h"
#include "ext/spl/spl_engine.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "php_phar.h"
#include "ext/hash/php_hash.h"
#include "ext/hash/php_hash_sha.h"

/* PHP_ because this is public information via MINFO */
#define PHP_PHAR_API_VERSION      "1.1.1"
/* x.y.z maps to 0xyz0 */
#define PHAR_API_VERSION          0x1110
/* if we bump PHAR_API_VERSION, change this from 0x1100 to PHAR_API_VERSION */
#define PHAR_API_VERSION_NODIR    0x1100
#define PHAR_API_MIN_DIR          0x1110
#define PHAR_API_MIN_READ         0x1000
#define PHAR_API_MAJORVERSION     0x1000
#define PHAR_API_MAJORVER_MASK    0xF000
#define PHAR_API_VER_MASK         0xFFF0

#define PHAR_HDR_COMPRESSION_MASK 0x0000F000
#define PHAR_HDR_COMPRESSED_NONE  0x00000000
#define PHAR_HDR_COMPRESSED_GZ    0x00001000
#define PHAR_HDR_COMPRESSED_BZ2   0x00002000
#define PHAR_HDR_SIGNATURE        0x00010000

/* flags for defining that the entire file should be compressed */
#define PHAR_FILE_COMPRESSION_MASK 0x00F00000
#define PHAR_FILE_COMPRESSED_NONE  0x00000000
#define PHAR_FILE_COMPRESSED_GZ    0x00100000
#define PHAR_FILE_COMPRESSED_BZ2   0x00200000

#define PHAR_SIG_MD5              0x0001
#define PHAR_SIG_SHA1             0x0002
#define PHAR_SIG_SHA256           0x0003
#define PHAR_SIG_SHA512           0x0004
#define PHAR_SIG_OPENSSL          0x0010
#define PHAR_SIG_OPENSSL_SHA256   0x0011
#define PHAR_SIG_OPENSSL_SHA512   0x0012

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

#define PHAR_FORMAT_SAME    0
#define PHAR_FORMAT_PHAR    1
#define PHAR_FORMAT_TAR     2
#define PHAR_FORMAT_ZIP     3

#define TAR_FILE    '0'
#define TAR_LINK    '1'
#define TAR_SYMLINK '2'
#define TAR_DIR     '5'
#define TAR_NEW     '8'
#define TAR_GLOBAL_HDR 'g'
#define TAR_FILE_HDR   'x'

#define PHAR_MUNG_PHP_SELF			(1<<0)
#define PHAR_MUNG_REQUEST_URI		(1<<1)
#define PHAR_MUNG_SCRIPT_NAME		(1<<2)
#define PHAR_MUNG_SCRIPT_FILENAME	(1<<3)

typedef struct _phar_entry_fp phar_entry_fp;
typedef struct _phar_archive_data phar_archive_data;

ZEND_BEGIN_MODULE_GLOBALS(phar)
	/* a list of phar_archive_data objects that reference a cached phar, so
	   that if copy-on-write is performed, we can swap them out for the new value */
	HashTable   phar_persist_map;
	HashTable   phar_fname_map;
	/* for cached phars, this is a per-process store of fp/ufp */
	phar_entry_fp *cached_fp;
	HashTable   phar_alias_map;
	int         phar_SERVER_mung_list;
	int         readonly;
	char*       cache_list;
	int         manifest_cached;
	int         persist;
	int         has_zlib;
	int         has_bz2;
	bool   readonly_orig;
	bool   require_hash_orig;
	bool   intercepted;
	int         request_init;
	int         require_hash;
	int         request_done;
	int         request_ends;
	zif_handler orig_fopen;
	zif_handler orig_file_get_contents;
	zif_handler orig_is_file;
	zif_handler orig_is_link;
	zif_handler orig_is_dir;
	zif_handler orig_opendir;
	zif_handler orig_file_exists;
	zif_handler orig_fileperms;
	zif_handler orig_fileinode;
	zif_handler orig_filesize;
	zif_handler orig_fileowner;
	zif_handler orig_filegroup;
	zif_handler orig_fileatime;
	zif_handler orig_filemtime;
	zif_handler orig_filectime;
	zif_handler orig_filetype;
	zif_handler orig_is_writable;
	zif_handler orig_is_readable;
	zif_handler orig_is_executable;
	zif_handler orig_lstat;
	zif_handler orig_readfile;
	zif_handler orig_stat;
	/* used for includes with . in them inside front controller */
	char*       cwd;
	uint32_t    cwd_len;
	int         cwd_init;
	char        *openssl_privatekey;
	uint32_t    openssl_privatekey_len;
	/* phar_get_archive cache */
	char*       last_phar_name;
	uint32_t    last_phar_name_len;
	char*       last_alias;
	uint32_t    last_alias_len;
	phar_archive_data* last_phar;
	HashTable mime_types;
ZEND_END_MODULE_GLOBALS(phar)

ZEND_EXTERN_MODULE_GLOBALS(phar)
#define PHAR_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(phar, v)

#if defined(ZTS) && defined(COMPILE_DL_PHAR)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#include "pharzip.h"

typedef union _phar_archive_object  phar_archive_object;
typedef union _phar_entry_object    phar_entry_object;

/*
 * used in phar_entry_info->fp_type to
 */
enum phar_fp_type {
	/* regular file pointer phar_archive_data->fp */
	PHAR_FP,
	/* uncompressed file pointer phar_archive_data->uncompressed_fp */
	PHAR_UFP,
	/* modified file pointer phar_entry_info->fp */
	PHAR_MOD,
	/* temporary manifest entry (file outside of the phar mapped to a location inside the phar)
	   this entry stores the stream to open in link (normally used for tars, but we steal it here) */
	PHAR_TMP
};

/*
 * Represents the metadata of the phar file or a file entry within the phar.
 * Can contain any combination of serialized data and the value as needed.
 */
typedef struct _phar_metadata_tracker {
	/* Can be IS_UNDEF or a regular value */
	zval val;
	/* Nullable string with the serialized value, if the serialization was performed or read from a file. */
	zend_string *str;
} phar_metadata_tracker;

/* entry for one file in a phar file */
typedef struct _phar_entry_info {
	/* first bytes are exactly as in file */
	uint32_t                 uncompressed_filesize;
	uint32_t                 timestamp;
	uint32_t                 compressed_filesize;
	uint32_t                 crc32;
	uint32_t                 flags;
	/* remainder */
	/* when changing compression, save old flags in case fp is NULL */
	uint32_t                 old_flags;
	phar_metadata_tracker metadata_tracker;
	uint32_t                 filename_len;
	char                     *filename;
	enum phar_fp_type        fp_type;
	/* offset within original phar file of the file contents */
	zend_long                     offset_abs;
	/* offset within fp of the file contents */
	zend_long                     offset;
	/* offset within original phar file of the file header (for zip-based/tar-based) */
	zend_long                     header_offset;
	php_stream               *fp;
	php_stream               *cfp;
	int                      fp_refcount;
	char                     *tmp;
	phar_archive_data        *phar;
	char                     *link; /* symbolic link to another file */
	char                     tar_type;
	/* position in the manifest */
	uint32_t                     manifest_pos;
	/* for stat */
	unsigned short           inode;

	uint32_t             is_crc_checked:1;
	uint32_t             is_modified:1;
	uint32_t             is_deleted:1;
	uint32_t             is_dir:1;
	/* this flag is used for mounted entries (external files mapped to location
	   inside a phar */
	uint32_t             is_mounted:1;
	/* used when iterating */
	uint32_t             is_temp_dir:1;
	/* tar-based phar file stuff */
	uint32_t             is_tar:1;
	/* zip-based phar file stuff */
	uint32_t             is_zip:1;
	/* for cached phar entries */
	uint32_t             is_persistent:1;
} phar_entry_info;

/* information about a phar file (the archive itself) */
struct _phar_archive_data {
	char                     *fname;
	uint32_t                 fname_len;
	/* for phar_detect_fname_ext, this stores the location of the file extension within fname */
	char                     *ext;
	uint32_t                 ext_len;
	char                     *alias;
	uint32_t                      alias_len;
	char                     version[12];
	size_t                   internal_file_start;
	size_t                   halt_offset;
	HashTable                manifest;
	/* hash of virtual directories, as in path/to/file.txt has path/to and path as virtual directories */
	HashTable                virtual_dirs;
	/* hash of mounted directory paths */
	HashTable                mounted_dirs;
	uint32_t                 flags;
	uint32_t                 min_timestamp;
	uint32_t                 max_timestamp;
	php_stream               *fp;
	/* decompressed file contents are stored here */
	php_stream               *ufp;
	int                      refcount;
	uint32_t                 sig_flags;
	uint32_t                 sig_len;
	char                     *signature;
	phar_metadata_tracker metadata_tracker;
	uint32_t                 phar_pos;
	/* if 1, then this alias was manually specified by the user and is not a permanent alias */
	uint32_t             is_temporary_alias:1;
	uint32_t             is_modified:1;
	uint32_t             is_writeable:1;
	uint32_t             is_brandnew:1;
	/* defer phar creation */
	uint32_t             donotflush:1;
	/* zip-based phar variables */
	uint32_t             is_zip:1;
	/* tar-based phar variables */
	uint32_t             is_tar:1;
	/* PharData variables       */
	uint32_t             is_data:1;
	/* for cached phar manifests */
	uint32_t             is_persistent:1;
};

typedef struct _phar_entry_fp_info {
	enum phar_fp_type        fp_type;
	/* offset within fp of the file contents */
	zend_long                     offset;
} phar_entry_fp_info;

struct _phar_entry_fp {
	php_stream *fp;
	php_stream *ufp;
	phar_entry_fp_info *manifest;
};

static inline php_stream *phar_get_entrypfp(phar_entry_info *entry)
{
	if (!entry->is_persistent) {
		return entry->phar->fp;
	}
	return PHAR_G(cached_fp)[entry->phar->phar_pos].fp;
}

static inline php_stream *phar_get_entrypufp(phar_entry_info *entry)
{
	if (!entry->is_persistent) {
		return entry->phar->ufp;
	}
	return PHAR_G(cached_fp)[entry->phar->phar_pos].ufp;
}

static inline void phar_set_entrypfp(phar_entry_info *entry, php_stream *fp)
{
	if (!entry->phar->is_persistent) {
		entry->phar->fp =  fp;
		return;
	}

	PHAR_G(cached_fp)[entry->phar->phar_pos].fp = fp;
}

static inline void phar_set_entrypufp(phar_entry_info *entry, php_stream *fp)
{
	if (!entry->phar->is_persistent) {
		entry->phar->ufp =  fp;
		return;
	}

	PHAR_G(cached_fp)[entry->phar->phar_pos].ufp = fp;
}

static inline php_stream *phar_get_pharfp(phar_archive_data *phar)
{
	if (!phar->is_persistent) {
		return phar->fp;
	}
	return PHAR_G(cached_fp)[phar->phar_pos].fp;
}

static inline php_stream *phar_get_pharufp(phar_archive_data *phar)
{
	if (!phar->is_persistent) {
		return phar->ufp;
	}
	return PHAR_G(cached_fp)[phar->phar_pos].ufp;
}

static inline void phar_set_pharfp(phar_archive_data *phar, php_stream *fp)
{
	if (!phar->is_persistent) {
		phar->fp =  fp;
		return;
	}

	PHAR_G(cached_fp)[phar->phar_pos].fp = fp;
}

static inline void phar_set_pharufp(phar_archive_data *phar, php_stream *fp)
{
	if (!phar->is_persistent) {
		phar->ufp =  fp;
		return;
	}

	PHAR_G(cached_fp)[phar->phar_pos].ufp = fp;
}

static inline void phar_set_fp_type(phar_entry_info *entry, enum phar_fp_type type, zend_off_t offset)
{
	phar_entry_fp_info *data;

	if (!entry->is_persistent) {
		entry->fp_type = type;
		entry->offset = offset;
		return;
	}
	data = &(PHAR_G(cached_fp)[entry->phar->phar_pos].manifest[entry->manifest_pos]);
	data->fp_type = type;
	data->offset = offset;
}

static inline enum phar_fp_type phar_get_fp_type(phar_entry_info *entry)
{
	if (!entry->is_persistent) {
		return entry->fp_type;
	}
	return PHAR_G(cached_fp)[entry->phar->phar_pos].manifest[entry->manifest_pos].fp_type;
}

static inline zend_off_t phar_get_fp_offset(phar_entry_info *entry)
{
	if (!entry->is_persistent) {
		return entry->offset;
	}
	if (PHAR_G(cached_fp)[entry->phar->phar_pos].manifest[entry->manifest_pos].fp_type == PHAR_FP) {
		if (!PHAR_G(cached_fp)[entry->phar->phar_pos].manifest[entry->manifest_pos].offset) {
			PHAR_G(cached_fp)[entry->phar->phar_pos].manifest[entry->manifest_pos].offset = entry->offset;
		}
	}
	return PHAR_G(cached_fp)[entry->phar->phar_pos].manifest[entry->manifest_pos].offset;
}

#define PHAR_MIME_PHP '\0'
#define PHAR_MIME_PHPS '\1'
#define PHAR_MIME_OTHER '\2'

typedef struct _phar_mime_type {
	char *mime;
	uint32_t len;
	/* one of PHAR_MIME_* */
	char type;
} phar_mime_type;

/* stream access data for one file entry in a phar file */
typedef struct _phar_entry_data {
	phar_archive_data        *phar;
	php_stream               *fp;
	/* stream position proxy, allows multiple open streams referring to the same fp */
	zend_off_t                    position;
	/* for copies of the phar fp, defines where 0 is */
	zend_off_t                    zero;
	uint32_t             for_write:1;
	uint32_t             is_zip:1;
	uint32_t             is_tar:1;
	phar_entry_info          *internal_file;
} phar_entry_data;

/* archive php object */
union _phar_archive_object {
	spl_filesystem_object    spl;
	phar_archive_data        *archive;
};

/* entry php object */
union _phar_entry_object {
	spl_filesystem_object    spl;
	phar_entry_info          *entry;
};

BEGIN_EXTERN_C()

#ifdef PHP_WIN32
static inline void phar_unixify_path_separators(char *path, size_t path_len)
{
	char *s;

	/* unixify win paths */
	for (s = path; (size_t)(s - path) < path_len; ++s) {
		if (*s == '\\') {
			*s = '/';
		}
	}
}
#endif
/**
 * validate an alias, returns 1 for success, 0 for failure
 */
static inline int phar_validate_alias(const char *alias, size_t alias_len) /* {{{ */
{
	return !(memchr(alias, '/', alias_len) || memchr(alias, '\\', alias_len) || memchr(alias, ':', alias_len) ||
		memchr(alias, ';', alias_len) || memchr(alias, '\n', alias_len) || memchr(alias, '\r', alias_len));
}
/* }}} */

static inline void phar_set_inode(phar_entry_info *entry) /* {{{ */
{
	char tmp[MAXPATHLEN];
	size_t tmp_len;
	size_t len1, len2;

	tmp_len = MIN(MAXPATHLEN, entry->filename_len + entry->phar->fname_len);

	len1 = MIN(entry->phar->fname_len, tmp_len);
	if (entry->phar->fname) {
		memcpy(tmp, entry->phar->fname, len1);
	}

	len2 = MIN(tmp_len - len1, entry->filename_len);
	memcpy(tmp + len1, entry->filename, len2);

	entry->inode = (unsigned short) zend_hash_func(tmp, tmp_len);
}
/* }}} */

void phar_request_initialize(void);

void phar_object_init(void);
void phar_destroy_phar_data(phar_archive_data *phar);

int phar_open_entry_file(phar_archive_data *phar, phar_entry_info *entry, char **error);
int phar_postprocess_file(phar_entry_data *idata, uint32_t crc32, char **error, int process_zip);
int phar_open_from_filename(char *fname, size_t fname_len, char *alias, size_t alias_len, uint32_t options, phar_archive_data** pphar, char **error);
int phar_open_or_create_filename(char *fname, size_t fname_len, char *alias, size_t alias_len, bool is_data, uint32_t options, phar_archive_data** pphar, char **error);
int phar_create_or_parse_filename(char *fname, size_t fname_len, char *alias, size_t alias_len, bool is_data, uint32_t options, phar_archive_data** pphar, char **error);
int phar_open_executed_filename(char *alias, size_t alias_len, char **error);
int phar_free_alias(phar_archive_data *phar, char *alias, size_t alias_len);
int phar_get_archive(phar_archive_data **archive, char *fname, size_t fname_len, char *alias, size_t alias_len, char **error);
int phar_open_parsed_phar(char *fname, size_t fname_len, char *alias, size_t alias_len, bool is_data, uint32_t options, phar_archive_data** pphar, char **error);
int phar_verify_signature(php_stream *fp, size_t end_of_phar, uint32_t sig_type, char *sig, size_t sig_len, char *fname, char **signature, size_t *signature_len, char **error);
int phar_create_signature(phar_archive_data *phar, php_stream *fp, char **signature, size_t *signature_length, char **error);

/* utility functions */
zend_string *phar_create_default_stub(const char *index_php, const char *web_index, char **error);
char *phar_decompress_filter(phar_entry_info * entry, int return_unknown);
char *phar_compress_filter(phar_entry_info * entry, int return_unknown);

/* void phar_remove_virtual_dirs(phar_archive_data *phar, char *filename, size_t filename_len); */
void phar_add_virtual_dirs(phar_archive_data *phar, char *filename, size_t filename_len);
int phar_mount_entry(phar_archive_data *phar, char *filename, size_t filename_len, char *path, size_t path_len);
zend_string *phar_find_in_include_path(char *file, size_t file_len, phar_archive_data **pphar);
char *phar_fix_filepath(char *path, size_t *new_len, int use_cwd);
phar_entry_info * phar_open_jit(phar_archive_data *phar, phar_entry_info *entry, char **error);
void phar_parse_metadata_lazy(const char *buffer, phar_metadata_tracker *tracker, uint32_t zip_metadata_len, int persistent);
bool phar_metadata_tracker_has_data(const phar_metadata_tracker* tracker, int persistent);
/* If this has data, free it and set all values to undefined. */
void phar_metadata_tracker_free(phar_metadata_tracker* val, int persistent);
void phar_metadata_tracker_copy(phar_metadata_tracker* dest, const phar_metadata_tracker *source, int persistent);
void phar_metadata_tracker_clone(phar_metadata_tracker* tracker);
void phar_metadata_tracker_try_ensure_has_serialized_data(phar_metadata_tracker* tracker, int persistent);
int phar_metadata_tracker_unserialize_or_copy(phar_metadata_tracker* tracker, zval *value, int persistent, HashTable *unserialize_options, const char* method_name);
void phar_release_entry_metadata(phar_entry_info *entry);
void phar_release_archive_metadata(phar_archive_data *phar);
void destroy_phar_manifest_entry(zval *zv);
int phar_seek_efp(phar_entry_info *entry, zend_off_t offset, int whence, zend_off_t position, int follow_links);
php_stream *phar_get_efp(phar_entry_info *entry, int follow_links);
int phar_copy_entry_fp(phar_entry_info *source, phar_entry_info *dest, char **error);
int phar_open_entry_fp(phar_entry_info *entry, char **error, int follow_links);
phar_entry_info *phar_get_link_source(phar_entry_info *entry);
int phar_create_writeable_entry(phar_archive_data *phar, phar_entry_info *entry, char **error);
int phar_separate_entry_fp(phar_entry_info *entry, char **error);
int phar_open_archive_fp(phar_archive_data *phar);
int phar_copy_on_write(phar_archive_data **pphar);

/* tar functions in tar.c */
int phar_is_tar(char *buf, char *fname);
int phar_parse_tarfile(php_stream* fp, char *fname, size_t fname_len, char *alias, size_t alias_len, phar_archive_data** pphar, int is_data, uint32_t compression, char **error);
int phar_open_or_create_tar(char *fname, size_t fname_len, char *alias, size_t alias_len, int is_data, uint32_t options, phar_archive_data** pphar, char **error);
int phar_tar_flush(phar_archive_data *phar, char *user_stub, zend_long len, int defaultstub, char **error);

/* zip functions in zip.c */
int phar_parse_zipfile(php_stream *fp, char *fname, size_t fname_len, char *alias, size_t alias_len, phar_archive_data** pphar, char **error);
int phar_open_or_create_zip(char *fname, size_t fname_len, char *alias, size_t alias_len, int is_data, uint32_t options, phar_archive_data** pphar, char **error);
int phar_zip_flush(phar_archive_data *archive, char *user_stub, zend_long len, int defaultstub, char **error);

#ifdef PHAR_MAIN
static int phar_open_from_fp(php_stream* fp, char *fname, size_t fname_len, char *alias, size_t alias_len, uint32_t options, phar_archive_data** pphar, int is_data, char **error);
extern const php_stream_wrapper php_stream_phar_wrapper;
#else
extern HashTable cached_phars;
extern HashTable cached_alias;
#endif

int phar_archive_delref(phar_archive_data *phar);
int phar_entry_delref(phar_entry_data *idata);

phar_entry_info *phar_get_entry_info(phar_archive_data *phar, char *path, size_t path_len, char **error, int security);
phar_entry_info *phar_get_entry_info_dir(phar_archive_data *phar, char *path, size_t path_len, char dir, char **error, int security);
phar_entry_data *phar_get_or_create_entry_data(char *fname, size_t fname_len, char *path, size_t path_len, const char *mode, char allow_dir, char **error, int security);
int phar_get_entry_data(phar_entry_data **ret, char *fname, size_t fname_len, char *path, size_t path_len, const char *mode, char allow_dir, char **error, int security);
int phar_flush(phar_archive_data *archive, char *user_stub, zend_long len, int convert, char **error);
int phar_detect_phar_fname_ext(const char *filename, size_t filename_len, const char **ext_str, size_t *ext_len, int executable, int for_create, int is_complete);
int phar_split_fname(const char *filename, size_t filename_len, char **arch, size_t *arch_len, char **entry, size_t *entry_len, int executable, int for_create);

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

phar_path_check_result phar_path_check(char **p, size_t *len, const char **error);

END_EXTERN_C()
