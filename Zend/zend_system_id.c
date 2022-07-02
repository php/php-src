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
   | Authors: Sammy Kaye Powers <sammyk@php.net>                          |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "zend_system_id.h"
#include "zend_extensions.h"
#include "ext/standard/md5.h"
#include "ext/hash/php_hash.h"

ZEND_API char zend_system_id[32];

static PHP_MD5_CTX context;
static int finalized = 0;

ZEND_API ZEND_RESULT_CODE zend_add_system_entropy(const char *module_name, const char *hook_name, const void *data, size_t size)
{
	if (finalized == 0) {
		PHP_MD5Update(&context, module_name, strlen(module_name));
		PHP_MD5Update(&context, hook_name, strlen(hook_name));
		if (size) {
			PHP_MD5Update(&context, data, size);
		}
		return SUCCESS;
	}
	return FAILURE;
}

#define ZEND_BIN_ID "BIN_" ZEND_TOSTR(SIZEOF_INT) ZEND_TOSTR(SIZEOF_LONG) ZEND_TOSTR(SIZEOF_SIZE_T) ZEND_TOSTR(SIZEOF_ZEND_LONG) ZEND_TOSTR(ZEND_MM_ALIGNMENT)

void zend_startup_system_id(void)
{
	PHP_MD5Init(&context);
	PHP_MD5Update(&context, PHP_VERSION, sizeof(PHP_VERSION)-1);
	PHP_MD5Update(&context, ZEND_EXTENSION_BUILD_ID, sizeof(ZEND_EXTENSION_BUILD_ID)-1);
	PHP_MD5Update(&context, ZEND_BIN_ID, sizeof(ZEND_BIN_ID)-1);
	if (strstr(PHP_VERSION, "-dev") != 0) {
		/* Development versions may be changed from build to build */
		PHP_MD5Update(&context, __DATE__, sizeof(__DATE__)-1);
		PHP_MD5Update(&context, __TIME__, sizeof(__TIME__)-1);
	}
	zend_system_id[0] = '\0';
}

#define ZEND_HOOK_AST_PROCESS      (1 << 0)
#define ZEND_HOOK_COMPILE_FILE     (1 << 1)
#define ZEND_HOOK_EXECUTE_EX       (1 << 2)
#define ZEND_HOOK_EXECUTE_INTERNAL (1 << 3)

void zend_finalize_system_id(void)
{
	unsigned char digest[16];
	zend_uchar hooks = 0;

	if (zend_ast_process) {
		hooks |= ZEND_HOOK_AST_PROCESS;
	}
	if (zend_compile_file != compile_file) {
		hooks |= ZEND_HOOK_COMPILE_FILE;
	}
	if (zend_execute_ex != execute_ex) {
		hooks |= ZEND_HOOK_EXECUTE_EX;
	}
	if (zend_execute_internal) {
		hooks |= ZEND_HOOK_EXECUTE_INTERNAL;
	}
	PHP_MD5Update(&context, &hooks, sizeof hooks);

	for (int16_t i = 0; i < 256; i++) {
		if (zend_get_user_opcode_handler((zend_uchar) i) != NULL) {
			PHP_MD5Update(&context, &i, sizeof i);
		}
	}

	PHP_MD5Final(digest, &context);
	php_hash_bin2hex(zend_system_id, digest, sizeof digest);
	finalized = 1;
}
