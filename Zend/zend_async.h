/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:          |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_ASYNC_H
#define ZEND_ASYNC_H

#include "zend_types.h"
#include "zend_ast.h"

BEGIN_EXTERN_C()

extern ZEND_API zend_class_entry *zend_ce_future;

void zend_register_async_ce(void);
void zend_async_init(void);
void zend_async_shutdown(void);

/* Parser helpers: "await expr" and "async function ..." are desugared into
 * calls to Async\wait() and Async\async(). */
zend_ast *zend_ast_create_await(zend_ast *expr);
zend_ast *zend_ast_create_async_function(zend_ast *decl);

END_EXTERN_C()

#endif /* ZEND_ASYNC_H */
