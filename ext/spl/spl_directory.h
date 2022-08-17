/*
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
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifndef SPL_DIRECTORY_H
#define SPL_DIRECTORY_H

#include "php.h"
#include "php_spl.h"

extern PHPAPI zend_class_entry *spl_ce_SplFileInfo;
extern PHPAPI zend_class_entry *spl_ce_DirectoryIterator;
extern PHPAPI zend_class_entry *spl_ce_FilesystemIterator;
extern PHPAPI zend_class_entry *spl_ce_RecursiveDirectoryIterator;
extern PHPAPI zend_class_entry *spl_ce_GlobIterator;
extern PHPAPI zend_class_entry *spl_ce_SplFileObject;
extern PHPAPI zend_class_entry *spl_ce_SplTempFileObject;

PHP_MINIT_FUNCTION(spl_directory);

/* Internal objecte structure and helpers for Directory and File SPL objects */
typedef struct _spl_filesystem_object  spl_filesystem_object;

typedef void (*spl_foreign_dtor_t)(spl_filesystem_object *object);
typedef void (*spl_foreign_clone_t)(spl_filesystem_object *src, spl_filesystem_object *dst);

PHPAPI zend_string *spl_filesystem_object_get_path(spl_filesystem_object *intern);

typedef struct _spl_other_handler {
	spl_foreign_dtor_t     dtor;
	spl_foreign_clone_t    clone;
} spl_other_handler;

typedef enum {
	SPL_FS_INFO, /* must be 0 */
	SPL_FS_DIR,
	SPL_FS_FILE
} SPL_FS_OBJ_TYPE;

struct _spl_filesystem_object {
	void               *oth;
	const spl_other_handler  *oth_handler;
	zend_string        *path;
	zend_string        *orig_path;
	zend_string        *file_name;
	SPL_FS_OBJ_TYPE    type;
	zend_long               flags;
	zend_class_entry   *file_class;
	zend_class_entry   *info_class;
	union {
		struct {
			php_stream         *dirp;
			zend_string        *sub_path;
			int                index;
			int                is_recursive;
			zend_function      *func_rewind;
			zend_function      *func_next;
			zend_function      *func_valid;
			php_stream_dirent  entry;
		} dir;
		struct {
			php_stream         *stream;
			php_stream_context *context;
			zval               *zcontext;
			zend_string        *open_mode;
			zval               current_zval;
			char               *current_line;
			size_t             current_line_len;
			size_t             max_line_len;
			zend_long               current_line_num;
			zval               zresource;
			zend_function      *func_getCurr;
			char               delimiter;
			char               enclosure;
			int                escape;
		} file;
	} u;
	zend_object        std;
};

#define SPL_FILE_OBJECT_DROP_NEW_LINE      0x00000001 /* drop new lines */
#define SPL_FILE_OBJECT_READ_AHEAD         0x00000002 /* read on rewind/next */
#define SPL_FILE_OBJECT_SKIP_EMPTY         0x00000004 /* skip empty lines */
#define SPL_FILE_OBJECT_READ_CSV           0x00000008 /* read via fgetcsv */
#define SPL_FILE_OBJECT_MASK               0x0000000F /* read via fgetcsv */

#define SPL_FILE_DIR_CURRENT_AS_FILEINFO   0x00000000 /* make RecursiveDirectoryTree::current() return SplFileInfo */
#define SPL_FILE_DIR_CURRENT_AS_SELF       0x00000010 /* make RecursiveDirectoryTree::current() return getSelf() */
#define SPL_FILE_DIR_CURRENT_AS_PATHNAME   0x00000020 /* make RecursiveDirectoryTree::current() return getPathname() */
#define SPL_FILE_DIR_CURRENT_MODE_MASK     0x000000F0 /* mask RecursiveDirectoryTree::current() */
#define SPL_FILE_DIR_CURRENT(intern,mode)  ((intern->flags&SPL_FILE_DIR_CURRENT_MODE_MASK)==mode)

#define SPL_FILE_DIR_KEY_AS_PATHNAME       0x00000000 /* make RecursiveDirectoryTree::key() return getPathname() */
#define SPL_FILE_DIR_KEY_AS_FILENAME       0x00000100 /* make RecursiveDirectoryTree::key() return getFilename() */
#define SPL_FILE_DIR_KEY_MODE_MASK         0x00000F00 /* mask RecursiveDirectoryTree::key() */
#define SPL_FILE_NEW_CURRENT_AND_KEY       SPL_FILE_DIR_KEY_AS_FILENAME|SPL_FILE_DIR_CURRENT_AS_FILEINFO
#define SPL_FILE_DIR_KEY(intern,mode)      ((intern->flags&SPL_FILE_DIR_KEY_MODE_MASK)==mode)

#define SPL_FILE_DIR_SKIPDOTS              0x00001000 /* Tells whether it should skip dots or not */
#define SPL_FILE_DIR_UNIXPATHS             0x00002000 /* Whether to unixify path separators */
#define SPL_FILE_DIR_FOLLOW_SYMLINKS       0x00004000 /* make RecursiveDirectoryTree::hasChildren() follow symlinks */
#define SPL_FILE_DIR_OTHERS_MASK           0x00007000 /* mask used for get/setFlags */

#endif /* SPL_DIRECTORY_H */
