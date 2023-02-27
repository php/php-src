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
	/* Regular data types */
	IS_UNDEF,
	IS_NULL,
	IS_FALSE,
	IS_TRUE,
	IS_LONG,
	IS_DOUBLE,
	IS_STRING,
	IS_ARRAY,
	IS_OBJECT,
	IS_RESOURCE,
	IS_REFERENCE,
	IS_CONSTANT_AST, /* Constant expressions */

	/**
	 * One after the largest regular data type; used internally
	 * for overlapping ranges below.
	 */
	_IS_REGULAR_END,

	/* Fake types used only for type hinting.
	 * These are allowed to overlap with the types below. */
	IS_CALLABLE = _IS_REGULAR_END,
	IS_ITERABLE,
	IS_VOID,
	IS_STATIC,
	IS_MIXED,
	IS_NEVER,

	_IS_FAKE_END,

	/* internal types */
	IS_INDIRECT = _IS_REGULAR_END,
	IS_PTR,
	IS_ALIAS_PTR,
	_IS_ERROR,

	_IS_INTERNAL_END,

	/* used for casts */
	_IS_BOOL = _IS_FAKE_END > _IS_INTERNAL_END ? _IS_FAKE_END : _IS_INTERNAL_END,
	_IS_NUMBER,
};

#endif /* ZEND_TYPE_CODE_H */
