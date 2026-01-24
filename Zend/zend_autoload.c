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
   | Authors: Gina Peter Banyard <girgias@php.net>                        |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_API.h"
#include "zend_autoload.h"
#include "zend_hash.h"
#include "zend_types.h"
#include "zend_exceptions.h"
#include "zend_string.h"

ZEND_TLS HashTable *autoloader_class_autoload_functions;

ZEND_API void zend_autoload_callback_zval_destroy(zval *element)
{
   zend_fcall_info_cache *fcc = Z_PTR_P(element);
   zend_fcc_dtor(fcc);
   efree(fcc);
}

static Bucket *autoload_find_registered_function(const HashTable *autoloader_table, const zend_fcall_info_cache *function_entry)
{
   zend_fcall_info_cache *current_function_entry;
   ZEND_HASH_MAP_FOREACH_PTR(autoloader_table, current_function_entry) {
      if (zend_fcc_equals(current_function_entry, function_entry)) {
         return _p;
      }
   } ZEND_HASH_FOREACH_END();
   return NULL;
}

ZEND_API zend_class_entry *zend_perform_class_autoload(zend_string *class_name, zend_string *lc_name)
{
   if (!autoloader_class_autoload_functions) {
      return NULL;
   }

   zval zname;
   ZVAL_STR(&zname, class_name);

   const HashTable *class_autoload_functions = autoloader_class_autoload_functions;

   /* Cannot use ZEND_HASH_MAP_FOREACH_PTR here as autoloaders may be
    * added/removed during autoloading. */
   HashPosition pos;
   zend_hash_internal_pointer_reset_ex(class_autoload_functions, &pos);
   while (true) {
      zend_fcall_info_cache *func_info = zend_hash_get_current_data_ptr_ex(class_autoload_functions, &pos);
      if (!func_info) {
         break;
      }
      zend_call_known_fcc(func_info, /* retval */ NULL, /* param_count */ 1, /* params */ &zname, /* named_params */ NULL);

      if (EG(exception)) {
         return NULL;
      }
      if (ZSTR_HAS_CE_CACHE(class_name) && ZSTR_GET_CE_CACHE(class_name)) {
         return (zend_class_entry*)ZSTR_GET_CE_CACHE(class_name);
      }

      zend_class_entry *ce = zend_hash_find_ptr(EG(class_table), lc_name);
      if (ce) {
         return ce;
      }

      zend_hash_move_forward_ex(class_autoload_functions, &pos);
   }
   return NULL;
}
/* Needed for compatibility with spl_register_autoload() */
ZEND_API void zend_autoload_register_class_loader(zend_fcall_info_cache *fcc, bool prepend)
{
   ZEND_ASSERT(ZEND_FCC_INITIALIZED(*fcc));

   if (!autoloader_class_autoload_functions) {
      ALLOC_HASHTABLE(autoloader_class_autoload_functions);
      zend_hash_init(autoloader_class_autoload_functions, 1, NULL, zend_autoload_callback_zval_destroy, false);
      /* Initialize as non-packed hash table for prepend functionality. */
      zend_hash_real_init_mixed(autoloader_class_autoload_functions);
   }

   // TODO: Assertion for this
   //if (fcc->function_handler->type == ZEND_INTERNAL_FUNCTION &&
   //   fcc->function_handler->internal_function.handler == zif_autoload_call_class) {
   //   zend_argument_value_error(1, "must not be the autoload_call_class() function");
   //   return;
   //}

   /* If function is already registered, don't do anything */
   if (autoload_find_registered_function(autoloader_class_autoload_functions, fcc)) {
      /* Release potential call trampoline */
      zend_release_fcall_info_cache(fcc);
      return;
   }

   zend_fcc_addref(fcc);
   zend_hash_next_index_insert_mem(autoloader_class_autoload_functions, fcc, sizeof(zend_fcall_info_cache));
   if (prepend && zend_hash_num_elements(autoloader_class_autoload_functions) > 1) {
      /* Move the newly created element to the head of the hashtable */
      ZEND_ASSERT(!HT_IS_PACKED(autoloader_class_autoload_functions));
      Bucket tmp = autoloader_class_autoload_functions->arData[autoloader_class_autoload_functions->nNumUsed-1];
      memmove(autoloader_class_autoload_functions->arData + 1, autoloader_class_autoload_functions->arData, sizeof(Bucket) * (autoloader_class_autoload_functions->nNumUsed - 1));
      autoloader_class_autoload_functions->arData[0] = tmp;
      zend_hash_rehash(autoloader_class_autoload_functions);
   }
}

ZEND_API bool zend_autoload_unregister_class_loader(const zend_fcall_info_cache *fcc) {
   if (autoloader_class_autoload_functions) {
      Bucket *p = autoload_find_registered_function(autoloader_class_autoload_functions, fcc);
      if (p) {
         zend_hash_del_bucket(autoloader_class_autoload_functions, p);
         return true;
      }
   }
   return false;
}

ZEND_API void zend_autoload_fcc_map_to_callable_zval_map(zval *return_value) {
   if (autoloader_class_autoload_functions) {
      zend_fcall_info_cache *fcc;

      array_init_size(return_value, zend_hash_num_elements(autoloader_class_autoload_functions));
      ZEND_HASH_MAP_FOREACH_PTR(autoloader_class_autoload_functions, fcc) {
         zval tmp;
         zend_get_callable_zval_from_fcc(fcc, &tmp);
         add_next_index_zval(return_value, &tmp);
      } ZEND_HASH_FOREACH_END();
   } else {
      RETURN_EMPTY_ARRAY();
   }
}

/* Only for deprecated strange behaviour of spl_autoload_unregister() */
ZEND_API void zend_autoload_drop_autoload_map(void)
{
   if (autoloader_class_autoload_functions) {
      /* Don't destroy the hash table, as we might be iterating over it right now. */
      zend_hash_clean(autoloader_class_autoload_functions);
   }
}

void zend_autoload_shutdown(void)
{
   if (autoloader_class_autoload_functions) {
      zend_hash_destroy(autoloader_class_autoload_functions);
      FREE_HASHTABLE(autoloader_class_autoload_functions);
      autoloader_class_autoload_functions = NULL;
   }
}
