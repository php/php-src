/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2006 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Michael Wallner <mike@php.net>                               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_OUTPUT_H
#define PHP_OUTPUT_H

#define PHP_OUTPUT_NEWAPI 1

/* handler ops */
#define PHP_OUTPUT_HANDLER_WRITE	0x00	/* standard passthru */
#define PHP_OUTPUT_HANDLER_START	0x01	/* start */
#define PHP_OUTPUT_HANDLER_CLEAN	0x02	/* restart */
#define PHP_OUTPUT_HANDLER_FLUSH	0x04	/* pass along as much as possible */
#define PHP_OUTPUT_HANDLER_FINAL	0x08	/* finalize */
#define PHP_OUTPUT_HANDLER_CONT		PHP_OUTPUT_HANDLER_WRITE
#define PHP_OUTPUT_HANDLER_END		PHP_OUTPUT_HANDLER_FINAL

/* handler types */
#define PHP_OUTPUT_HANDLER_INTERNAL		0x0000
#define PHP_OUTPUT_HANDLER_USER			0x0001

/* handler ability flags */
#define PHP_OUTPUT_HANDLER_CLEANABLE	0x0010
#define PHP_OUTPUT_HANDLER_REMOVABLE	0x0020
#define PHP_OUTPUT_HANDLER_STDFLAGS		0x0030

/* handler status flags */
#define PHP_OUTPUT_HANDLER_STARTED		0x1000
#define PHP_OUTPUT_HANDLER_DISABLED		0x2000

/* handler op return values */
#define PHP_OUTPUT_HANDLER_FAILURE		0
#define PHP_OUTPUT_HANDLER_SUCCESS		1
#define PHP_OUTPUT_HANDLER_NO_DATA		2

/* real global flags */
#define PHP_OUTPUT_IMPLICITFLUSH		0x01
#define PHP_OUTPUT_DISABLED				0x02
/* supplementary flags for php_output_get_status() */
#define PHP_OUTPUT_ACTIVE				0x10
#define PHP_OUTPUT_LOCKED				0x20

/* handler hooks */
#define PHP_OUTPUT_HANDLER_HOOK_GET_OPAQ	1
#define PHP_OUTPUT_HANDLER_HOOK_GET_FLAGS	2
#define PHP_OUTPUT_HANDLER_HOOK_IMMUTABLE	3
#define PHP_OUTPUT_HANDLER_HOOK_DISABLE		4

#define PHP_OUTPUT_HANDLER_INITBUF_SIZE(s) \
( (s) ? \
	(s) + PHP_OUTPUT_HANDLER_ALIGNTO_SIZE - ((s) % (PHP_OUTPUT_HANDLER_ALIGNTO_SIZE >> 2)) : \
	PHP_OUTPUT_HANDLER_DEFAULT_SIZE \
)
#define PHP_OUTPUT_HANDLER_ALIGNTO_SIZE		0x1000
#define PHP_OUTPUT_HANDLER_DEFAULT_SIZE		0x4000
#define PHP_OUTPUT_HANDLER_DEFAULT_NAME		"default output handler"
#define PHP_OUTPUT_HANDLER_DEVNULL_NAME		"null output handler"

PHPAPI zval php_output_handler_default;
PHPAPI zval php_output_handler_devnull;

typedef struct _php_output_buffer {
	char *data;
	size_t size;
	size_t used;
	uint free:1;
	uint _res:31;
} php_output_buffer;

typedef struct _php_output_context {
	int op;
	php_output_buffer in;
	php_output_buffer out;
#ifdef ZTS
	void ***tsrm_ls;
#endif
} php_output_context;

#define PHP_OUTPUT_TSRMLS(ctx) TSRMLS_FETCH_FROM_CTX((ctx)->tsrm_ls)

/* old-style, stateless callback */
typedef void (*php_output_handler_func_t)(char *output, uint output_len, char **handled_output, uint *handled_output_len, int mode TSRMLS_DC);
/* new-style, opaque context callback */
typedef int (*php_output_handler_context_func_t)(void **handler_context, php_output_context *output_context);
/* conflict check callback */
typedef int (*php_output_handler_conflict_check_t)(zval *handler_name TSRMLS_DC);

typedef struct _php_output_handler {
	zval *name;
	int flags;
	int level;
	size_t size;
	php_output_buffer buffer;
	
	void *opaq;
	void (*dtor)(void *opaq TSRMLS_DC);
	
	union {
		zval *user;
		php_output_handler_context_func_t internal;
	};
} php_output_handler;

ZEND_BEGIN_MODULE_GLOBALS(output)
	int flags;
	zend_stack handlers;
	php_output_handler *active;
	php_output_handler *running;
	char *output_start_filename;
	int output_start_lineno;
	zval *default_output_handler_name;
	zval *devnull_output_handler_name;
ZEND_END_MODULE_GLOBALS(output);

/* there should not be a need to use OG() from outside of output.c */
#ifdef ZTS
#define OG(v) TSRMG(output_globals_id, zend_output_globals *, v)
#else
#define OG(v) (output_globals.v)
#endif

/* convenience macros */
#define PHPWRITE(str, str_len)		php_output_write((str), (str_len))
#define PHPWRITE_H(str, str_len)	php_output_write_unbuffered((str), (str_len))

#define PUTC(c)						(php_output_write(&(c), 1), (c))
#define PUTC_H(c)					(php_output_write_unbuffered(&(c), 1), (c))

#define PUTS(str)					do {				\
	const char *__str = (str);							\
	php_output_write(__str, strlen(__str));				\
} while (0)
#define PUTS_H(str)					do {				\
	const char *__str = (str);							\
	php_output_write_unbuffered(__str, strlen(__str));	\
} while (0)


BEGIN_EXTERN_C()
#define php_output_tearup() \
	php_output_startup(); \
	php_output_activate()
#define php_output_teardown() \
	php_output_end_all(); \
	php_output_deactivate(); \
	php_output_shutdown()

/* MINIT */
PHPAPI void php_output_startup(void);
/* MSHUTDOWN */
PHPAPI void php_output_shutdown(void);

#define php_output_register_constants() _php_output_register_constants(TSRMLS_C)
PHPAPI void _php_output_register_constants(TSRMLS_D);

/* RINIT */
#define php_output_activate() _php_output_activate(TSRMLS_C)
PHPAPI int _php_output_activate(TSRMLS_D);
/* RSHUTDOWN */
#define php_output_deactivate() _php_output_deactivate(TSRMLS_C)
PHPAPI void _php_output_deactivate(TSRMLS_D);


#define php_output_get_default_handler_name() _php_output_get_default_handler_name(TSRMLS_C)
PHPAPI zval *_php_output_get_default_handler_name(TSRMLS_D);

#define php_output_get_devnull_handler_name() _php_output_get_devnull_handler_name(TSRMLS_C)
PHPAPI zval *_php_output_get_devnull_handler_name(TSRMLS_D);


#define php_output_set_status(s) _php_output_set_status((s) TSRMLS_CC)
PHPAPI void _php_output_set_status(int status TSRMLS_DC);

#define php_output_get_status() _php_output_get_status(TSRMLS_C)
PHPAPI int _php_output_get_status(TSRMLS_D);

#define php_output_write_unbuffered(s, l) _php_output_write_unbuffered((s), (l) TSRMLS_CC)
PHPAPI int _php_output_write_unbuffered(const char *str, size_t len TSRMLS_DC);

#define php_output_write(s, l) _php_output_write((s), (l) TSRMLS_CC)
PHPAPI int _php_output_write(const char *str, size_t len TSRMLS_DC);

#define php_output_flush() _php_output_flush(TSRMLS_C)
PHPAPI void _php_output_flush(TSRMLS_D);

#define php_output_flush_all() _php_output_flush_all(TSRMLS_C)
PHPAPI void _php_output_flush_all(TSRMLS_D);

#define php_output_clean() _php_output_clean(TSRMLS_C)
PHPAPI int _php_output_clean(TSRMLS_D);

#define php_output_clean_all() _php_output_clean_all(TSRMLS_C)
PHPAPI void _php_output_clean_all(TSRMLS_D);

#define php_output_end() _php_output_end(TSRMLS_C)
PHPAPI int _php_output_end(TSRMLS_D);

#define php_output_end_all() _php_output_end_all(TSRMLS_C)
PHPAPI void _php_output_end_all(TSRMLS_D);

#define php_output_discard() _php_output_discard(TSRMLS_C)
PHPAPI int _php_output_discard(TSRMLS_D);

#define php_output_discard_all() _php_output_discard_all(TSRMLS_C)
PHPAPI void _php_output_discard_all(TSRMLS_D);


#define php_output_get_contents(p) _php_output_get_contents((p) TSRMLS_CC)
PHPAPI int _php_output_get_contents(zval *p TSRMLS_DC);

#define php_output_get_length(p) _php_output_get_length((p) TSRMLS_CC)
PHPAPI int _php_output_get_length(zval *TSRMLS_DC);

#define php_output_get_level() _php_output_get_level(TSRMLS_C)
PHPAPI int _php_output_get_level(TSRMLS_D);


#define php_output_start_default() _php_output_start_default(TSRMLS_C)
PHPAPI int _php_output_start_default(TSRMLS_D);

#define php_output_start_devnull() _php_output_start_devnull(TSRMLS_C)
PHPAPI int _php_output_start_devnull(TSRMLS_D);

#define php_output_start_user(h, s, f) _php_output_start_user((h), (s), (f) TSRMLS_CC)
PHPAPI int _php_output_start_user(zval *output_handler, size_t chunk_size, int flags TSRMLS_DC);

#define php_output_start_internal(n, h, s, f) _php_output_start_internal((n), (h), (s), (f) TSRMLS_CC)
PHPAPI int _php_output_start_internal(zval *name, php_output_handler_func_t output_handler, size_t chunk_size, int flags TSRMLS_DC);

#define php_output_handler_create_user(h, s, f) _php_output_handler_create_user((h), (s), (f) TSRMLS_CC)
PHPAPI php_output_handler *_php_output_handler_create_user(zval *handler, size_t chunk_size, int flags TSRMLS_DC);

#define php_output_handler_create_internal(n, h, s, f) _php_output_handler_create_internal((n), (h), (s), (f) TSRMLS_CC)
PHPAPI php_output_handler *_php_output_handler_create_internal(zval *name, php_output_handler_context_func_t handler, size_t chunk_size, int flags TSRMLS_DC);

#define php_output_handler_set_context(h, c, d) _php_output_handler_set_context((h), (c), (d) TSRMLS_CC)
PHPAPI void _php_output_handler_set_context(php_output_handler *handler, void *opaq, void (*dtor)(void* TSRMLS_DC) TSRMLS_DC);

#define php_output_handler_start(h) _php_output_handler_start((h) TSRMLS_CC)
PHPAPI int _php_output_handler_start(php_output_handler *handler TSRMLS_DC);

#define php_output_handler_started(n) _php_output_handler_started((n) TSRMLS_CC)
PHPAPI int _php_output_handler_started(zval *name TSRMLS_DC);

#define php_output_handler_hook(t, a) _php_output_handler_hook((t), (a) TSRMLS_CC)
PHPAPI int _php_output_handler_hook(int type, void *arg TSRMLS_DC);

#define php_output_handler_dtor(h) _php_output_handler_dtor((h) TSRMLS_CC)
PHPAPI void _php_output_handler_dtor(php_output_handler *handler TSRMLS_DC);

#define php_output_handler_free(h) _php_output_handler_free((h) TSRMLS_CC)
PHPAPI void _php_output_handler_free(php_output_handler **handler TSRMLS_DC);


#define php_output_set_implicit_flush(f) _php_output_set_implicit_flush((f) TSRMLS_CC)
PHPAPI void _php_output_set_implicit_flush(int flush TSRMLS_DC);

#define php_output_get_start_filename() _php_output_get_start_filename(TSRMLS_C)
PHPAPI char *_php_output_get_start_filename(TSRMLS_D);

#define php_output_get_start_lineno() _php_output_get_start_lineno(TSRMLS_C)
PHPAPI int _php_output_get_start_lineno(TSRMLS_D);


#define php_output_handler_conflict(n, s) _php_output_handler_conflict((n), (s) TSRMLS_CC)
PHPAPI int _php_output_handler_conflict(zval *handler_new, zval *handler_set TSRMLS_DC);

#define php_output_handler_conflict_register(n, f) _php_output_handler_conflict_register((n), (f) TSRMLS_CC)
PHPAPI int _php_output_handler_conflict_register(zval *handler_name, php_output_handler_conflict_check_t check_func TSRMLS_DC);

#define php_output_handler_reverse_conflict_register(n, f) _php_output_handler_reverse_conflict_register((n), (f) TSRMLS_CC)
PHPAPI int _php_output_handler_reverse_conflict_register(zval *handler_name, php_output_handler_conflict_check_t check_func TSRMLS_DC);

#define php_output_handler_alias(n) _php_output_handler_alias((n) TSRMLS_CC)
PHPAPI php_output_handler_context_func_t *_php_output_handler_alias(zval *handler_name TSRMLS_DC);

#define php_output_handler_alias_register(n, f) _php_output_handler_alias_register((n), (f) TSRMLS_CC)
PHPAPI int _php_output_handler_alias_register(zval *handler_name, php_output_handler_context_func_t func TSRMLS_DC);

END_EXTERN_C()


PHP_FUNCTION(ob_start);
PHP_FUNCTION(ob_flush);
PHP_FUNCTION(ob_clean);
PHP_FUNCTION(ob_end_flush);
PHP_FUNCTION(ob_end_clean);
PHP_FUNCTION(ob_get_flush);
PHP_FUNCTION(ob_get_clean);
PHP_FUNCTION(ob_get_contents);
PHP_FUNCTION(ob_get_length);
PHP_FUNCTION(ob_get_level);
PHP_FUNCTION(ob_get_status);
PHP_FUNCTION(ob_implicit_flush);
PHP_FUNCTION(ob_list_handlers);

PHP_FUNCTION(output_add_rewrite_var);
PHP_FUNCTION(output_reset_rewrite_vars);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
