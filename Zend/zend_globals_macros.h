/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_GLOBALS_MACROS_H
#define ZEND_GLOBALS_MACROS_H

typedef struct _zend_compiler_globals zend_compiler_globals;
typedef struct _zend_executor_globals zend_executor_globals;
typedef struct _zend_php_scanner_globals zend_php_scanner_globals;
typedef struct _zend_ini_scanner_globals zend_ini_scanner_globals;

BEGIN_EXTERN_C()

/* Compiler */
#ifdef ZTS
# define CG(v) ZEND_TSRMG_FAST(compiler_globals_offset, zend_compiler_globals *, v)
#else
# define CG(v) (compiler_globals.v)
extern ZEND_API struct _zend_compiler_globals compiler_globals;
#endif
ZEND_API int zendparse(void);


/* Executor */
#ifdef ZTS
# define EG(v) ZEND_TSRMG_FAST(executor_globals_offset, zend_executor_globals *, v)
#else
# define EG(v) (executor_globals.v)
extern ZEND_API zend_executor_globals executor_globals;
#endif

/* Language Scanner */
#ifdef ZTS
# define LANG_SCNG(v) ZEND_TSRMG_FAST(language_scanner_globals_offset, zend_php_scanner_globals *, v)
extern ZEND_API ts_rsrc_id language_scanner_globals_id;
extern ZEND_API size_t language_scanner_globals_offset;
#else
# define LANG_SCNG(v) (language_scanner_globals.v)
extern ZEND_API zend_php_scanner_globals language_scanner_globals;
#endif


/* INI Scanner */
#ifdef ZTS
# define INI_SCNG(v) ZEND_TSRMG_FAST(ini_scanner_globals_offset, zend_ini_scanner_globals *, v)
extern ZEND_API ts_rsrc_id ini_scanner_globals_id;
extern ZEND_API size_t ini_scanner_globals_offset;
#else
# define INI_SCNG(v) (ini_scanner_globals.v)
extern ZEND_API zend_ini_scanner_globals ini_scanner_globals;
#endif

END_EXTERN_C()

#endif /* ZEND_GLOBALS_MACROS_H */
