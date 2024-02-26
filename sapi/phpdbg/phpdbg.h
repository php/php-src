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
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#ifndef PHPDBG_H
#define PHPDBG_H

#ifdef PHP_WIN32
# define PHPDBG_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
# define PHPDBG_API __attribute__ ((visibility("default")))
#else
# define PHPDBG_API
#endif

#include <stdint.h>
#include <stddef.h>
#include "php.h"
#include "php_globals.h"
#include "php_variables.h"
#include "php_getopt.h"
#include "zend_builtin_functions.h"
#include "zend_extensions.h"
#include "zend_modules.h"
#include "zend_globals.h"
#include "zend_ini_scanner.h"
#include "zend_stream.h"
#include "zend_signal.h"
#if !defined(_WIN32) && !defined(ZEND_SIGNALS)
#	include <signal.h>
#elif defined(PHP_WIN32)
#	include "win32/signal.h"
#endif
#include "SAPI.h"
#include <fcntl.h>
#include <sys/types.h>
#if defined(_WIN32) && !defined(__MINGW32__)
#	include <windows.h>
#	include "config.w32.h"
#	undef  strcasecmp
#	undef  strncasecmp
#	define strcasecmp _stricmp
#	define strncasecmp _strnicmp
#else
#	include "php_config.h"
#endif
#ifndef O_BINARY
#	define O_BINARY 0
#endif
#include "php_main.h"

#ifdef ZTS
# include "TSRM.h"
#endif

#undef zend_hash_str_add
#ifdef PHP_WIN32
#define zend_hash_str_add(...) \
	zend_hash_str_add(__VA_ARGS__)
#else
#define zend_hash_str_add_tmp(ht, key, len, pData) \
	zend_hash_str_add(ht, key, len, pData)
#define zend_hash_str_add(...) zend_hash_str_add_tmp(__VA_ARGS__)
#endif

#ifdef HAVE_PHPDBG_READLINE
# ifdef HAVE_LIBREADLINE
#	 include <readline/readline.h>
#	 include <readline/history.h>
# endif
# ifdef HAVE_LIBEDIT
#	 include <editline/readline.h>
# endif
#endif

/* {{{ strings */
#define PHPDBG_NAME "phpdbg"
#define PHPDBG_AUTHORS "Felipe Pena, Joe Watkins and Bob Weinand" /* Ordered by last name */
#define PHPDBG_ISSUES "https://github.com/php/php-src/issues"
#define PHPDBG_VERSION PHP_VERSION
#define PHPDBG_INIT_FILENAME ".phpdbginit"
#define PHPDBG_DEFAULT_PROMPT "prompt>"
/* }}} */

#ifdef ZTS
# define PHPDBG_G(v) ZEND_TSRMG(phpdbg_globals_id, zend_phpdbg_globals *, v)
#else
# define PHPDBG_G(v) (phpdbg_globals.v)
#endif

#include "phpdbg_sigsafe.h"
#include "phpdbg_out.h"
#include "phpdbg_lexer.h"
#include "phpdbg_cmd.h"
#include "phpdbg_utils.h"
#include "phpdbg_btree.h"
#include "phpdbg_watch.h"
#include "phpdbg_bp.h"

int phpdbg_do_parse(phpdbg_param_t *stack, char *input);

#define PHPDBG_NEXT   2
#define PHPDBG_UNTIL  3
#define PHPDBG_FINISH 4
#define PHPDBG_LEAVE  5

/*
 BEGIN: DO NOT CHANGE DO NOT CHANGE DO NOT CHANGE
*/

/* {{{ flags */
#define PHPDBG_HAS_FILE_BP            (1ULL<<1)
#define PHPDBG_HAS_PENDING_FILE_BP    (1ULL<<2)
#define PHPDBG_HAS_SYM_BP             (1ULL<<3)
#define PHPDBG_HAS_OPLINE_BP          (1ULL<<4)
#define PHPDBG_HAS_METHOD_BP          (1ULL<<5)
#define PHPDBG_HAS_COND_BP            (1ULL<<6)
#define PHPDBG_HAS_OPCODE_BP          (1ULL<<7)
#define PHPDBG_HAS_FUNCTION_OPLINE_BP (1ULL<<8)
#define PHPDBG_HAS_METHOD_OPLINE_BP   (1ULL<<9)
#define PHPDBG_HAS_FILE_OPLINE_BP     (1ULL<<10) /* }}} */

/*
 END: DO NOT CHANGE DO NOT CHANGE DO NOT CHANGE
*/

#define PHPDBG_IN_COND_BP             (1ULL<<11)
#define PHPDBG_IN_EVAL                (1ULL<<12)

#define PHPDBG_IS_STEPPING            (1ULL<<13)
#define PHPDBG_STEP_OPCODE            (1ULL<<14)
#define PHPDBG_IS_QUIET               (1ULL<<15)
#define PHPDBG_IS_QUITTING            (1ULL<<16)
#define PHPDBG_IS_COLOURED            (1ULL<<17)
#define PHPDBG_IS_CLEANING            (1ULL<<18)
#define PHPDBG_IS_RUNNING             (1ULL<<19)

#define PHPDBG_IN_UNTIL               (1ULL<<20)
#define PHPDBG_IN_FINISH              (1ULL<<21)
#define PHPDBG_IN_LEAVE               (1ULL<<22)

#define PHPDBG_IS_REGISTERED          (1ULL<<23)
#define PHPDBG_IS_STEPONEVAL          (1ULL<<24)
#define PHPDBG_IS_INITIALIZING        (1ULL<<25)
#define PHPDBG_IS_SIGNALED            (1ULL<<26)
#define PHPDBG_IS_INTERACTIVE         (1ULL<<27)
#define PHPDBG_PREVENT_INTERACTIVE    (1ULL<<28)
#define PHPDBG_IS_BP_ENABLED          (1ULL<<29)
#define PHPDBG_SHOW_REFCOUNTS         (1ULL<<30)
#define PHPDBG_IN_SIGNAL_HANDLER      (1ULL<<31)
#define PHPDBG_DISCARD_OUTPUT         (1ULL<<32)
#define PHPDBG_HAS_PAGINATION         (1ULL<<33)

#define PHPDBG_SEEK_MASK              (PHPDBG_IN_UNTIL | PHPDBG_IN_FINISH | PHPDBG_IN_LEAVE)
#define PHPDBG_BP_RESOLVE_MASK	      (PHPDBG_HAS_FUNCTION_OPLINE_BP | PHPDBG_HAS_METHOD_OPLINE_BP | PHPDBG_HAS_FILE_OPLINE_BP)
#define PHPDBG_BP_MASK                (PHPDBG_HAS_FILE_BP | PHPDBG_HAS_SYM_BP | PHPDBG_HAS_METHOD_BP | PHPDBG_HAS_OPLINE_BP | PHPDBG_HAS_COND_BP | PHPDBG_HAS_OPCODE_BP | PHPDBG_HAS_FUNCTION_OPLINE_BP | PHPDBG_HAS_METHOD_OPLINE_BP | PHPDBG_HAS_FILE_OPLINE_BP)
#define PHPDBG_IS_STOPPING            (PHPDBG_IS_QUITTING | PHPDBG_IS_CLEANING)

#define PHPDBG_PRESERVE_FLAGS_MASK    \
    (PHPDBG_SHOW_REFCOUNTS | \
     PHPDBG_IS_STEPONEVAL | \
     PHPDBG_IS_BP_ENABLED | \
     PHPDBG_STEP_OPCODE | \
     PHPDBG_IS_QUIET | \
     PHPDBG_IS_COLOURED | \
     PHPDBG_HAS_PAGINATION)

#ifndef _WIN32
#	define PHPDBG_DEFAULT_FLAGS (PHPDBG_IS_QUIET | PHPDBG_IS_COLOURED | PHPDBG_IS_BP_ENABLED | PHPDBG_HAS_PAGINATION)
#else
#	define PHPDBG_DEFAULT_FLAGS (PHPDBG_IS_QUIET | PHPDBG_IS_BP_ENABLED | PHPDBG_HAS_PAGINATION)
#endif /* }}} */

/* {{{ output descriptors */
#define PHPDBG_STDIN 			0
#define PHPDBG_STDOUT			1
#define PHPDBG_STDERR			2
#define PHPDBG_IO_FDS 			3 /* }}} */

#define phpdbg_try_access \
	{                                                            \
		JMP_BUF *__orig_bailout = PHPDBG_G(sigsegv_bailout); \
		JMP_BUF __bailout;                                   \
                                                                     \
		PHPDBG_G(sigsegv_bailout) = &__bailout;              \
		if (SETJMP(__bailout) == 0) {
#define phpdbg_catch_access \
		} else {                                             \
			PHPDBG_G(sigsegv_bailout) = __orig_bailout;
#define phpdbg_end_try_access() \
		}                                                    \
			PHPDBG_G(sigsegv_bailout) = __orig_bailout;  \
	}


void phpdbg_register_file_handles(void);

typedef struct _phpdbg_oplog_entry phpdbg_oplog_entry;
struct _phpdbg_oplog_entry {
	phpdbg_oplog_entry *next;
	zend_string *function_name;
	zend_class_entry *scope;
	zend_string *filename;
	zend_op *opcodes;
	zend_op *op;
};

typedef struct _phpdbg_oplog_list phpdbg_oplog_list;
struct _phpdbg_oplog_list {
	phpdbg_oplog_list *prev;
	phpdbg_oplog_entry start; /* Only "next" member used. */
};


/* {{{ structs */
ZEND_BEGIN_MODULE_GLOBALS(phpdbg)
	HashTable bp[PHPDBG_BREAK_TABLES];           /* break points */
	HashTable registered;                        /* registered */
	HashTable seek;                              /* seek oplines */
	zend_execute_data *seek_ex;                  /* call frame of oplines to seek to */
	zend_object *handled_exception;              /* last handled exception (prevent multiple handling of same exception) */
	phpdbg_frame_t frame;                        /* frame */
	uint32_t last_line;                          /* last executed line */

	char *cur_command;                           /* current command */
	phpdbg_lexer_data lexer;                     /* lexer data */
	phpdbg_param_t *parser_stack;                /* param stack during lexer / parser phase */

#ifndef _WIN32
	struct sigaction old_sigsegv_signal;         /* segv signal handler */
#endif
#ifdef HAVE_USERFAULTFD_WRITEFAULT
    int watch_userfaultfd;                       /* userfaultfd(2) handler, 0 if unused */
    pthread_t watch_userfault_thread;            /* thread for watch fault handling */
#endif
	phpdbg_btree watchpoint_tree;                /* tree with watchpoints */
	phpdbg_btree watch_HashTables;               /* tree with original dtors of watchpoints */
	HashTable watch_elements;                    /* user defined watch elements */
	HashTable watch_collisions;                  /* collision table to check if multiple watches share the same recursive watchpoint */
	HashTable watch_recreation;                  /* watch elements pending recreation of their respective watchpoints */
	HashTable watch_free;                        /* pointers to watch for being freed */
	HashTable *watchlist_mem;                    /* triggered watchpoints */
	HashTable *watchlist_mem_backup;             /* triggered watchpoints backup table while iterating over it */
	bool watchpoint_hit;                    /* a watchpoint was hit */
	void (*original_free_function)(void * ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);      /* the original AG(mm_heap)->_free function */
	phpdbg_watch_element *watch_tmp;             /* temporary pointer for a watch element */

	char *exec;                                  /* file to execute */
	size_t exec_len;                             /* size of exec */
	zend_op_array *ops;                 	     /* op_array */
	zval retval;                                 /* return value */
	int bp_count;                                /* breakpoint count */
	int vmret;                                   /* return from last opcode handler execution */
	bool in_execution;                      /* in execution? */
	bool unclean_eval;                      /* do not check for memory leaks when we needed to bail out during eval */

	zend_op_array *(*compile_file)(zend_file_handle *file_handle, int type);
	zend_op_array *(*init_compile_file)(zend_file_handle *file_handle, int type);
	zend_op_array *(*compile_string)(zend_string *source_string, const char *filename, zend_compile_position position);
	HashTable file_sources;

	zend_arena *oplog_arena;                     /* arena for storing oplog */
	phpdbg_oplog_list *oplog_list;               /* list of oplog starts */
	phpdbg_oplog_entry *oplog_cur;               /* current oplog entry */

	struct {
		int fd;
	} io[PHPDBG_IO_FDS];                         /* io */
	ssize_t (*php_stdiop_write)(php_stream *, const char *, size_t);
	struct {
		bool active;
		int type;
		int fd;
		char *msg;
		int msglen;
	} err_buf;                                   /* error buffer */
	zend_ulong req_id;                           /* "request id" to keep track of commands */

	char *prompt[2];                             /* prompt */
	const phpdbg_color_t *colors[PHPDBG_COLORS]; /* colors */
	char *buffer;                                /* buffer */
	bool last_was_newline;                  /* check if we don't need to output a newline upon next phpdbg_error or phpdbg_notice */

	FILE *stdin_file;                            /* FILE pointer to stdin source file */
	const php_stream_wrapper *orig_url_wrap_php;

	char input_buffer[PHPDBG_MAX_CMD];           /* stdin input buffer */
	int input_buflen;                            /* length of stdin input buffer */
	phpdbg_signal_safe_mem sigsafe_mem;          /* memory to use in async safe environment (only once!) */

	JMP_BUF *sigsegv_bailout;                    /* bailout address for accessibility probing */

	uint64_t flags;                              /* phpdbg flags */

	char *sapi_name_ptr;                         /* store sapi name to free it if necessary to not leak memory */
	zend_ulong lines;                                  /* max number of lines to display */
ZEND_END_MODULE_GLOBALS(phpdbg) /* }}} */

#endif /* PHPDBG_H */
