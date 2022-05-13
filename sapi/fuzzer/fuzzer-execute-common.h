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
   | Authors: Nikita Popov <nikic@php.net>                                |
   +----------------------------------------------------------------------+
 */

#include <main/php.h>

#if defined(__FreeBSD__)
# include <sys/sysctl.h>
#endif

#include "fuzzer.h"
#include "fuzzer-sapi.h"
#include "zend_exceptions.h"

#define FILE_NAME "/tmp/fuzzer.php"
#define MAX_STEPS 1000
#define MAX_SIZE (8 * 1024)
static uint32_t steps_left;
static bool bailed_out = false;

/* Because the fuzzer is always compiled with clang,
 * we can assume that we don't use global registers / hybrid VM. */
typedef int (ZEND_FASTCALL *opcode_handler_t)(zend_execute_data *);

static zend_always_inline void fuzzer_bailout(void) {
	bailed_out = true;
	zend_bailout();
}

static zend_always_inline void fuzzer_step(void) {
	if (--steps_left == 0) {
		/* Reset steps before bailing out, so code running after bailout (e.g. in
		 * destructors) will get another MAX_STEPS, rather than UINT32_MAX steps. */
		steps_left = MAX_STEPS;
		fuzzer_bailout();
	}
}

static void (*orig_execute_ex)(zend_execute_data *execute_data);

static void fuzzer_execute_ex(zend_execute_data *execute_data) {
	while (1) {
		int ret;
		fuzzer_step();
		if ((ret = ((opcode_handler_t) EX(opline)->handler)(execute_data)) != 0) {
			if (ret > 0) {
				execute_data = EG(current_execute_data);
			} else {
				return;
			}
		}
	}
}

static zend_op_array *(*orig_compile_string)(
		zend_string *source_string, const char *filename, zend_compile_position position);

static zend_op_array *fuzzer_compile_string(
		zend_string *str, const char *filename, zend_compile_position position) {
	if (ZSTR_LEN(str) > MAX_SIZE) {
		/* Avoid compiling huge inputs via eval(). */
		fuzzer_bailout();
	}

	return orig_compile_string(str, filename, position);
}

static void (*orig_execute_internal)(zend_execute_data *execute_data, zval *return_value);

static void fuzzer_execute_internal(zend_execute_data *execute_data, zval *return_value) {
	fuzzer_step();

	uint32_t num_args = ZEND_CALL_NUM_ARGS(execute_data);
	for (uint32_t i = 0; i < num_args; i++) {
		/* Some internal functions like preg_replace() may be slow on large inputs.
		 * Limit the maximum size of string inputs. */
		zval *arg = ZEND_CALL_VAR_NUM(execute_data, i);
		if (Z_TYPE_P(arg) == IS_STRING && Z_STRLEN_P(arg) > MAX_SIZE) {
			fuzzer_bailout();
		}
	}

	orig_execute_internal(execute_data, return_value);
}

static void fuzzer_init_php_for_execute(const char *extra_ini) {
	/* Compilation will often trigger fatal errors.
	 * Use tracked allocation mode to avoid leaks in that case. */
	putenv("USE_TRACKED_ALLOC=1");

	/* Just like other SAPIs, ignore SIGPIPEs. */
	signal(SIGPIPE, SIG_IGN);

	fuzzer_init_php(extra_ini);

	orig_execute_ex = zend_execute_ex;
	zend_execute_ex = fuzzer_execute_ex;
	orig_execute_internal = zend_execute_internal ? zend_execute_internal : execute_internal;
	zend_execute_internal = fuzzer_execute_internal;
	orig_compile_string = zend_compile_string;
	zend_compile_string = fuzzer_compile_string;
}

ZEND_ATTRIBUTE_UNUSED static void create_file(void) {
	/* For opcache_invalidate() to work, the dummy file name used for fuzzing needs to
	 * actually exist. */
	FILE *f = fopen(FILE_NAME, "w");
	fclose(f);
}

ZEND_ATTRIBUTE_UNUSED static void opcache_invalidate(void) {
	steps_left = MAX_STEPS;
	zend_exception_save();
	zval retval, func, args[2];
	ZVAL_STRING(&func, "opcache_invalidate");
	ZVAL_STRING(&args[0], FILE_NAME);
	ZVAL_TRUE(&args[1]);
	call_user_function(CG(function_table), NULL, &func, &retval, 2, args);
	ZEND_ASSERT(Z_TYPE(retval) == IS_TRUE);
	zval_ptr_dtor(&args[0]);
	zval_ptr_dtor(&retval);
	zval_ptr_dtor(&func);
	zend_exception_restore();
}

ZEND_ATTRIBUTE_UNUSED char *get_opcache_path(void) {
	/* Try relative to cwd. */
	char *p = realpath("modules/opcache.so", NULL);
	if (p) {
		return p;
	}

	/* Try relative to binary location. */
	char path[MAXPATHLEN];
#if defined(__FreeBSD__)
	size_t pathlen = sizeof(path);
	int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
	if (sysctl(mib, 4, path, &pathlen, NULL, 0) < 0) {
#else
	if (readlink("/proc/self/exe", path, sizeof(path)) < 0) {
#endif
		ZEND_ASSERT(0 && "Failed to get binary path");
		return NULL;
	}

	/* Get basename. */
	char *last_sep = strrchr(path, '/');
	if (last_sep) {
		*last_sep = '\0';
	}

	strlcat(path, "/modules/opcache.so", sizeof(path));
	return realpath(path, NULL);
}
