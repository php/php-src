/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2001 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.92 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_92.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#ifndef ZEND_GLOBALS_MACROS_H
#define ZEND_GLOBALS_MACROS_H

typedef struct _zend_compiler_globals zend_compiler_globals;
typedef struct _zend_executor_globals zend_executor_globals;
typedef struct _zend_alloc_globals zend_alloc_globals;

/* Compiler */
#ifdef ZTS
# define CG(v) TSRMG(compiler_globals_id, zend_compiler_globals *, v)
BEGIN_EXTERN_C()
int zendparse(void *compiler_globals);
END_EXTERN_C()
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


/* Memory Manager */
#ifdef ZTS
# define AG(v) TSRMG(alloc_globals_id, zend_alloc_globals *, v)
#else
# define AG(v) (alloc_globals.v)
extern ZEND_API zend_alloc_globals alloc_globals;
#endif


/* For limited downwards source compatibility */
#define CLS_FETCH()
#define ELS_FETCH()
#define ALS_FETCH()

#endif /* ZEND_GLOBALS_MACROS_H */

