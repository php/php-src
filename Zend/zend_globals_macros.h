/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_GLOBALS_MACROS_H
#define ZEND_GLOBALS_MACROS_H

typedef struct _zend_compiler_globals zend_compiler_globals;
typedef struct _zend_executor_globals zend_executor_globals;
typedef struct _zend_php_scanner_globals zend_php_scanner_globals;
typedef struct _zend_ini_scanner_globals zend_ini_scanner_globals;

BEGIN_EXTERN_C()

/* Compiler */
#ifdef ZTS
# define CG(v) TSRMG(compiler_globals_id, zend_compiler_globals *, v)
int zendparse(void *compiler_globals);
#else
# define CG(v) (compiler_globals.v)
extern ZEND_API struct _zend_compiler_globals compiler_globals;
int zendparse(void);
#endif


/* Executor */
#ifdef ZTS
# define EG(v) TSRMG(executor_globals_id, zend_executor_globals *, v)
#else
# define EG(v) (executor_globals.v)
extern ZEND_API zend_executor_globals executor_globals;
#endif

/* Language Scanner */
#ifdef ZTS
# define LANG_SCNG(v) TSRMG(language_scanner_globals_id, zend_php_scanner_globals *, v)
extern ZEND_API ts_rsrc_id language_scanner_globals_id;
#else
# define LANG_SCNG(v) (language_scanner_globals.v)
extern ZEND_API zend_php_scanner_globals language_scanner_globals;
#endif


/* INI Scanner */
#ifdef ZTS
# define INI_SCNG(v) TSRMG(ini_scanner_globals_id, zend_ini_scanner_globals *, v)
extern ZEND_API ts_rsrc_id ini_scanner_globals_id;
#else
# define INI_SCNG(v) (ini_scanner_globals.v)
extern ZEND_API zend_ini_scanner_globals ini_scanner_globals;
#endif

END_EXTERN_C()

/* For limited downwards source compatibility */
#define CLS_FETCH()
#define ELS_FETCH()
#define ALS_FETCH()
#define PLS_FETCH()
#define SLS_FETCH()
#define CLS_D
#define ELS_D
#define ALS_D
#define PLS_D
#define SLS_D
#define CLS_DC
#define ELS_DC
#define ALS_DC
#define PLS_DC
#define SLS_DC
#define CLS_C
#define ELS_C
#define ALS_C
#define PLS_C
#define SLS_C
#define CLS_CC
#define ELS_CC
#define ALS_CC
#define PLS_CC
#define SLS_CC


#endif /* ZEND_GLOBALS_MACROS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
