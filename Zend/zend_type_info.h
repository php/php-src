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
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_TYPE_INFO_H
#define ZEND_TYPE_INFO_H

#include "zend_types.h"

#define MAY_BE_UNDEF                (1 << IS_UNDEF)
#define MAY_BE_NULL		            (1 << IS_NULL)
#define MAY_BE_FALSE	            (1 << IS_FALSE)
#define MAY_BE_TRUE		            (1 << IS_TRUE)
#define MAY_BE_BOOL                 (MAY_BE_FALSE|MAY_BE_TRUE)
#define MAY_BE_LONG		            (1 << IS_LONG)
#define MAY_BE_DOUBLE	            (1 << IS_DOUBLE)
#define MAY_BE_STRING	            (1 << IS_STRING)
#define MAY_BE_ARRAY	            (1 << IS_ARRAY)
#define MAY_BE_OBJECT	            (1 << IS_OBJECT)
#define MAY_BE_RESOURCE	            (1 << IS_RESOURCE)
#define MAY_BE_ANY                  (MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)
#define MAY_BE_REF                  (1 << IS_REFERENCE) /* may be reference */

/* These are used in zend_type, but not for type inference.
 * They are allowed to overlap with types used during inference. */
#define MAY_BE_CALLABLE             (1 << IS_CALLABLE)
#define MAY_BE_VOID                 (1 << IS_VOID)
#define MAY_BE_NEVER                (1 << IS_NEVER)
#define MAY_BE_STATIC               (1 << IS_STATIC)

#define MAY_BE_ARRAY_SHIFT          (IS_REFERENCE)

#define MAY_BE_ARRAY_OF_NULL		(MAY_BE_NULL     << MAY_BE_ARRAY_SHIFT)
#define MAY_BE_ARRAY_OF_FALSE		(MAY_BE_FALSE    << MAY_BE_ARRAY_SHIFT)
#define MAY_BE_ARRAY_OF_TRUE		(MAY_BE_TRUE     << MAY_BE_ARRAY_SHIFT)
#define MAY_BE_ARRAY_OF_LONG		(MAY_BE_LONG     << MAY_BE_ARRAY_SHIFT)
#define MAY_BE_ARRAY_OF_DOUBLE		(MAY_BE_DOUBLE   << MAY_BE_ARRAY_SHIFT)
#define MAY_BE_ARRAY_OF_STRING		(MAY_BE_STRING   << MAY_BE_ARRAY_SHIFT)
#define MAY_BE_ARRAY_OF_ARRAY		(MAY_BE_ARRAY    << MAY_BE_ARRAY_SHIFT)
#define MAY_BE_ARRAY_OF_OBJECT		(MAY_BE_OBJECT   << MAY_BE_ARRAY_SHIFT)
#define MAY_BE_ARRAY_OF_RESOURCE	(MAY_BE_RESOURCE << MAY_BE_ARRAY_SHIFT)
#define MAY_BE_ARRAY_OF_ANY			(MAY_BE_ANY      << MAY_BE_ARRAY_SHIFT)
#define MAY_BE_ARRAY_OF_REF			(MAY_BE_REF      << MAY_BE_ARRAY_SHIFT)

#define MAY_BE_ARRAY_PACKED         (1<<21)
#define MAY_BE_ARRAY_NUMERIC_HASH   (1<<22) /* hash with numeric keys */
#define MAY_BE_ARRAY_STRING_HASH    (1<<23) /* hash with string keys */
#define MAY_BE_ARRAY_EMPTY          (1<<29)

#define MAY_BE_ARRAY_KEY_LONG       (MAY_BE_ARRAY_PACKED | MAY_BE_ARRAY_NUMERIC_HASH)
#define MAY_BE_ARRAY_KEY_STRING     MAY_BE_ARRAY_STRING_HASH
#define MAY_BE_ARRAY_KEY_ANY        (MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_EMPTY)

#define MAY_BE_PACKED(t)            ((t) & MAY_BE_ARRAY_PACKED)
#define MAY_BE_HASH(t)              ((t) & (MAY_BE_ARRAY_NUMERIC_HASH | MAY_BE_ARRAY_KEY_STRING))
#define MAY_BE_PACKED_ONLY(t)       (((t) & MAY_BE_ARRAY_KEY_ANY) == MAY_BE_ARRAY_PACKED)
#define MAY_BE_HASH_ONLY(t)         (MAY_BE_HASH(t) && !((t) & (MAY_BE_ARRAY_PACKED|MAY_BE_ARRAY_EMPTY)))
#define MAY_BE_EMPTY_ONLY(t)        (((t) & MAY_BE_ARRAY_KEY_ANY) == MAY_BE_ARRAY_EMPTY)

#define MAY_BE_CLASS                (1<<24)
#define MAY_BE_INDIRECT             (1<<25)

#define MAY_BE_RC1                  (1<<30) /* may be non-reference with refcount == 1 */
#define MAY_BE_RCN                  (1u<<31) /* may be non-reference with refcount > 1  */


#define MAY_BE_ANY_ARRAY \
	(MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_ANY|MAY_BE_ARRAY_OF_ANY|MAY_BE_ARRAY_OF_REF)

#endif /* ZEND_TYPE_INFO_H */
