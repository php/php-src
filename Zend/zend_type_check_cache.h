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
   | Authors: Dik Takken <d.h.j.takken@freedom.nl>                        |
   +----------------------------------------------------------------------+
*/

ZEND_API int zend_set_tcc_memory_limit(zend_long memory_limit);
ZEND_API int zend_set_tcc_class_slots(zend_long num_classes);
ZEND_API uint8_t *tcc_get_row (zend_type *type);
ZEND_API void tcc_assign_ce_columns();

// Below macro encapsulates how type checks that are not covered
// by the type check cache are identified: The row is the dummy
// cache row or their column index is zero. This row / column
// contains only zeros (no match) and must not be written into
// in case of a cache miss. This macro is used to achieve that.
#define TCC_CONTAINS(row, ce) (ce->tcc_column_index > 0 && row != CG(tcc_dummy_row))
