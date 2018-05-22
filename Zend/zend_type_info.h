/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2018 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_TYPE_INFO_H
#define ZEND_TYPE_INFO_H

#include "zend_types.h"

#define MAY_BE_UNDEF                (1 << IS_UNDEF)
#define MAY_BE_NULL		            (1 << IS_NULL)
#define MAY_BE_FALSE	            (1 << IS_FALSE)
#define MAY_BE_TRUE		            (1 << IS_TRUE)
#define MAY_BE_LONG		            (1 << IS_LONG)
#define MAY_BE_DOUBLE	            (1 << IS_DOUBLE)
#define MAY_BE_STRING	            (1 << IS_STRING)
#define MAY_BE_ARRAY	            (1 << IS_ARRAY)
#define MAY_BE_OBJECT	            (1 << IS_OBJECT)
#define MAY_BE_RESOURCE	            (1 << IS_RESOURCE)
#define MAY_BE_ANY                  (MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)
#define MAY_BE_REF                  (1 << IS_REFERENCE) /* may be reference */

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

#define MAY_BE_ARRAY_KEY_LONG       (1<<21)
#define MAY_BE_ARRAY_KEY_STRING     (1<<22)
#define MAY_BE_ARRAY_KEY_ANY        (MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_KEY_STRING)

#define MAY_BE_ERROR                (1<<23)
#define MAY_BE_CLASS                (1<<24)

#endif /* ZEND_TYPE_INFO_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
