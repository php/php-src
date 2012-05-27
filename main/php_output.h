/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
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
#define PHP_OUTPUT_HANDLER_FLUSHABLE	0x0020
#define PHP_OUTPUT_HANDLER_REMOVABLE	0x0040
#define PHP_OUTPUT_HANDLER_STDFLAGS		0x0070

/* handler status flags */
#define PHP_OUTPUT_HANDLER_STARTED		0x1000
#define PHP_OUTPUT_HANDLER_DISABLED		0x2000
#define PHP_OUTPUT_HANDLER_PROCESSED	0x4000

/* handler op return values */
typedef enum _php_output_handler_status_t {
	PHP_OUTPUT_HANDLER_FAILURE,
	PHP_OUTPUT_HANDLER_SUCCESS,
	PHP_OUTPUT_HANDLER_NO_DATA
} php_output_handler_status_t;

/* php_output_stack_pop() flags */
#define PHP_OUTPUT_POP_TRY			0x000
#define PHP_OUTPUT_POP_FORCE		0x001
#define PHP_OUTPUT_POP_DISCARD		0x010
#define PHP_OUTPUT_POP_SILENT		0x100

/* real global flags */
#define PHP_OUTPUT_IMPLICITFLUSH		0x01
#define PHP_OUTPUT_DISABLED				0x02
#define PHP_OUTPUT_WRITTEN				0x04
#define PHP_OUTPUT_SENT					0x08
/* supplementary flags for php_output_get_status() */
#define PHP_OUTPUT_ACTIVE				0x10
#define PHP_OUTPUT_LOCKED				0x20
/* output layer is ready to use */
#define PHP_OUTPUT_ACTIVATED		0x100000

/* handler hooks */
typedef enum _php_output_handler_hook_t {
	PHP_OUTPUT_HANDLER_HOOK_GET_OPAQ,
	PHP_OUTPUT_HANDLER_HOOK_GET_FLAGS,
	PHP_OUTPUT_HANDLER_HOOK_GET_LEVEL,
	PHP_OUTPUT_HANDLER_HOOK_IMMUTABLE,
	PHP_OUTPUT_HANDLER_HOOK_DISABLE,
	/* unused */
	PHP_OUTPUT_HANDLER_HOOK_LAST
} php_output_handler_hook_t;

#define PHP_OUTPUT_HANDLER_INITBUF_SIZE(s) \
( ((s) > 1) ? \
	(s) + PHP_OUTPUT_HANDLER_ALIGNTO_SIZE - ((s) % (PHP_OUTPUT_HANDLER_ALIGNTO_SIZE)) : \
	PHP_OUTPUT_HANDLER_DEFAULT_SIZE \
)
#define PHP_OUTPUT_HANDLER_ALIGNTO_SIZE		0x1000
#define PHP_OUTPUT_HANDLER_DEFAULT_SIZE		0x4000

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
/* output handler context dtor */
typedef void (*php_output_handler_context_dtor_t)(void *opaq TSRMLS_DC);
/* conflict check callback */
typedef int (*php_output_handler_conflict_check_t)(const char *handler_name, size_t handler_name_len TSRMLS_DC);
/* ctor for aliases */
typedef struct _php_output_handler *(*php_output_handler_alias_ctor_t)(const char *handler_name, size_t handler_name_len, size_t chunk_size, int flags TSRMLS_DC);

typedef struct _php_output_handler_user_func_t {
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zval *zoh;
} php_output_handler_user_func_t;

typedef struct _php_output_handler {
	char *name;
	size_t name_len;
	int flags;
	int level;
	size_t size;
	php_output_buffer buffer;
	
	void *opaq;
	void (*dtor)(void *opaq TSRMLS_DC);
	
	union {
		php_output_handler_user_func_t *user;
		php_output_handler_context_func_t internal;
	} func;
} php_output_handler;

ZEND_BEGIN_MODULE_GLOBALS(output)
	int flags;
	zend_stack handlers;
	php_output_handler *active;
	php_output_handler *running;
	const char *output_start_filename;
	int output_start_lineno;
ZEND_END_MODULE_GLOBALS(output)

/* there should not be a need to use OG() from outside of output.c */
#ifdef ZTS
# define OG(v) TSRMG(output_globals_id, zend_output_globals *, v)
#else
# define OG(v) (output_globals.v)
#endif

/* convenience macros */
#define PHPWRITE(str, str_len)		php_output_write((str), (str_len) TSRMLS_CC)
#define PHPWRITE_H(str, str_len)	php_output_write_unbuffered((str), (str_len) TSRMLS_CC)

#define PUTC(c)						(php_output_write(&(c), 1 TSRMLS_CC), (c))
#define PUTC_H(c)					(php_output_write_unbuffered(&(c), 1 TSRMLS_CC), (c))

#define PUTS(str)					do {				\
	const char *__str = (str);							\
	php_output_write(__str, strlen(__str) TSRMLS_CC);	\
} while (0)
#define PUTS_H(str)					do {							\
	const char *__str = (str);										\
	php_output_write_unbuffered(__str, strlen(__str) TSRMLS_CC);	\
} while (0)


BEGIN_EXTERN_C()

extern const char php_output_default_handler_name[sizeof("default output handler")];
extern const char php_output_devnull_handler_name[sizeof("null output handler")];

#define php_output_tearup() \
	php_output_startup(); \
	php_output_activate(TSRMLS_C)
#define php_output_teardown() \
	php_output_end_all(TSRMLS_C); \
	php_output_deactivate(TSRMLS_C); \
	php_output_shutdown()

/* MINIT */
PHPAPI void php_output_startup(void);
/* MSHUTDOWN */
PHPAPI void php_output_shutdown(void);

PHPAPI void php_output_register_constants(TSRMLS_D);

/* RINIT */
PHPAPI int php_output_activate(TSRMLS_D);
/* RSHUTDOWN */
PHPAPI void php_output_deactivate(TSRMLS_D);

PHPAPI void php_output_set_status(int status TSRMLS_DC);
PHPAPI int php_output_get_status(TSRMLS_D);
PHPAPI void php_output_set_implicit_flush(int flush TSRMLS_DC);
PHPAPI const char *php_output_get_start_filename(TSRMLS_D);
PHPAPI int php_output_get_start_lineno(TSRMLS_D);

PHPAPI int php_output_write_unbuffered(const char *str, size_t len TSRMLS_DC);
PHPAPI int php_output_write(const char *str, size_t len TSRMLS_DC);

PHPAPI int php_output_flush(TSRMLS_D);
PHPAPI void php_output_flush_all(TSRMLS_D);
PHPAPI int php_output_clean(TSRMLS_D);
PHPAPI void php_output_clean_all(TSRMLS_D);
PHPAPI int php_output_end(TSRMLS_D);
PHPAPI void php_output_end_all(TSRMLS_D);
PHPAPI int php_output_discard(TSRMLS_D);
PHPAPI void php_output_discard_all(TSRMLS_D);

PHPAPI int php_output_get_contents(zval *p TSRMLS_DC);
PHPAPI int php_output_get_length(zval *p TSRMLS_DC);
PHPAPI int php_output_get_level(TSRMLS_D);
PHPAPI php_output_handler* php_output_get_active_handler(TSRMLS_D);

PHPAPI int php_output_start_default(TSRMLS_D);
PHPAPI int php_output_start_devnull(TSRMLS_D);

PHPAPI int php_output_start_user(zval *output_handler, size_t chunk_size, int flags TSRMLS_DC);
PHPAPI int php_output_start_internal(const char *name, size_t name_len, php_output_handler_func_t output_handler, size_t chunk_size, int flags TSRMLS_DC);

PHPAPI php_output_handler *php_output_handler_create_user(zval *handler, size_t chunk_size, int flags TSRMLS_DC);
PHPAPI php_output_handler *php_output_handler_create_internal(const char *name, size_t name_len, php_output_handler_context_func_t handler, size_t chunk_size, int flags TSRMLS_DC);

PHPAPI void php_output_handler_set_context(php_output_handler *handler, void *opaq, void (*dtor)(void* TSRMLS_DC) TSRMLS_DC);
PHPAPI int php_output_handler_start(php_output_handler *handler TSRMLS_DC);
PHPAPI int php_output_handler_started(const char *name, size_t name_len TSRMLS_DC);
PHPAPI int php_output_handler_hook(php_output_handler_hook_t type, void *arg TSRMLS_DC);
PHPAPI void php_output_handler_dtor(php_output_handler *handler TSRMLS_DC);
PHPAPI void php_output_handler_free(php_output_handler **handler TSRMLS_DC);

PHPAPI int php_output_handler_conflict(const char *handler_new, size_t handler_new_len, const char *handler_set, size_t handler_set_len TSRMLS_DC);
PHPAPI int php_output_handler_conflict_register(const char *handler_name, size_t handler_name_len, php_output_handler_conflict_check_t check_func TSRMLS_DC);
PHPAPI int php_output_handler_reverse_conflict_register(const char *handler_name, size_t handler_name_len, php_output_handler_conflict_check_t check_func TSRMLS_DC);

PHPAPI php_output_handler_alias_ctor_t *php_output_handler_alias(const char *handler_name, size_t handler_name_len TSRMLS_DC);
PHPAPI int php_output_handler_alias_register(const char *handler_name, size_t handler_name_len, php_output_handler_alias_ctor_t func TSRMLS_DC);

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
