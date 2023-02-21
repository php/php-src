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
*/

#ifndef ZEND_TYPE_CODE_H
#define ZEND_TYPE_CODE_H

enum {
	/* Regular data types: Must be in sync with zend_variables.c. */
	IS_UNDEF = 0,
	IS_NULL = 1,
	IS_FALSE = 2,
	IS_TRUE = 3,
	IS_LONG = 4,
	IS_DOUBLE = 5,
	IS_STRING = 6,
	IS_ARRAY = 7,
	IS_OBJECT = 8,
	IS_RESOURCE = 9,
	IS_REFERENCE = 10,
	IS_CONSTANT_AST = 11, /* Constant expressions */

	/* Fake types used only for type hinting.
	 * These are allowed to overlap with the types below. */
	IS_CALLABLE = 12,
	IS_ITERABLE = 13,
	IS_VOID = 14,
	IS_STATIC = 15,
	IS_MIXED = 16,
	IS_NEVER = 17,

	/* internal types */
	IS_INDIRECT = 12,
	IS_PTR = 13,
	IS_ALIAS_PTR = 14,
	_IS_ERROR = 15,

	/* used for casts */
	_IS_BOOL = 18,
	_IS_NUMBER = 19,
};

#define ZEND_SAME_FAKE_TYPE(faketype, realtype) ( \
	(faketype) == (realtype) \
	|| ((faketype) == _IS_BOOL && ((realtype) == IS_TRUE || (realtype) == IS_FALSE)) \
)

#endif /* ZEND_TYPE_CODE_H */
