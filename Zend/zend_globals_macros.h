/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2000 Zend Technologies Ltd. (http://www.zend.com) |
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
# define CLS_D	zend_compiler_globals *compiler_globals
# define CLS_DC	, CLS_D
# define CLS_C	compiler_globals
# define CLS_CC , CLS_C
# define CG(v) (((zend_compiler_globals *) compiler_globals)->v)
# define CLS_FETCH()	zend_compiler_globals *compiler_globals = (zend_compiler_globals *) ts_resource(compiler_globals_id)
BEGIN_EXTERN_C()
int zendparse(void *compiler_globals);
END_EXTERN_C()
#else
# define CLS_D	void
# define CLS_DC
# define CLS_C
# define CLS_CC
# define CG(v) (compiler_globals.v)
# define CLS_FETCH()
extern ZEND_API struct _zend_compiler_globals compiler_globals;
int zendparse(void);
#endif


/* Executor */
#ifdef ZTS
# define ELS_D	zend_executor_globals *executor_globals
# define ELS_DC	, ELS_D
# define ELS_C	executor_globals
# define ELS_CC , ELS_C
# define EG(v) (executor_globals->v)
# define ELS_FETCH()	zend_executor_globals *executor_globals = (zend_executor_globals *) ts_resource(executor_globals_id)
#else
# define ELS_D	void
# define ELS_DC
# define ELS_C
# define ELS_CC
# define EG(v) (executor_globals.v)
# define ELS_FETCH()
extern ZEND_API zend_executor_globals executor_globals;
#endif


/* Memory Manager */
#ifdef ZTS
# define ALS_D	zend_alloc_globals *alloc_globals
# define ALS_DC	, ALS_D
# define ALS_C	alloc_globals
# define ALS_CC , ALS_C
# define AG(v) (((zend_alloc_globals *) alloc_globals)->v)
# define ALS_FETCH()	zend_alloc_globals *alloc_globals = (zend_alloc_globals *) ts_resource(alloc_globals_id)
#else
# define ALS_D	void
# define ALS_DC
# define ALS_C
# define ALS_CC
# define AG(v) (alloc_globals.v)
# define ALS_FETCH()
extern ZEND_API zend_alloc_globals alloc_globals;
#endif

#endif /* ZEND_GLOBALS_MACROS_H */

