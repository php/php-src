/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend Technologies Ltd., a subsidiary company of          |
   |     Perforce Software, Inc., and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_USER_FUNCTIONS_H
#define ZEND_USER_FUNCTIONS_H

#include "zend_types.h"

typedef struct _zval_struct zval;

zend_result zend_startup_builtin_functions(void);

BEGIN_EXTERN_C()
typedef struct _zend_fcall_info {
   size_t size;
   zval function_name;
   zval *retval;
   zval *params;
   zend_object *object;
   uint32_t param_count;
   uint32_t consumed_args;
   /* This hashtable can also contain positional arguments (with integer keys),
    * which will be appended to the normal params[]. This makes it easier to
    * integrate APIs like call_user_func_array(). The usual restriction that
    * there may not be position arguments after named arguments applies. */
   HashTable *named_params;
} zend_fcall_info;

typedef struct _zend_fcall_info_cache {
   zend_function *function_handler;
   zend_class_entry *calling_scope;
   zend_class_entry *called_scope;
   zend_object *object; /* Instance of object for method calls */
   zend_object *closure; /* Closure reference, only if the callable *is* the object */
} zend_fcall_info_cache;
END_EXTERN_C()

#ifndef __cplusplus
# define empty_fcall_info (zend_fcall_info) {0}
# define empty_fcall_info_cache (zend_fcall_info_cache) {0}
#else
# define empty_fcall_info zend_fcall_info {}
# define empty_fcall_info_cache zend_fcall_info_cache {}
#endif

#define ZEND_FCI_INITIALIZED(fci) ((fci).size != 0)
#define ZEND_FCC_INITIALIZED(fcc) ((fcc).function_handler != NULL)

#endif /* ZEND_USER_FUNCTIONS_H */
