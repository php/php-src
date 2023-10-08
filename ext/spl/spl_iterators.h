/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifndef SPL_ITERATORS_H
#define SPL_ITERATORS_H

#include "php.h"
#include "php_spl.h"

extern PHPAPI zend_class_entry *spl_ce_AppendIterator;
extern PHPAPI zend_class_entry *spl_ce_CachingIterator;
extern PHPAPI zend_class_entry *spl_ce_CallbackFilterIterator;
extern PHPAPI zend_class_entry *spl_ce_EmptyIterator;
extern PHPAPI zend_class_entry *spl_ce_FilterIterator;
extern PHPAPI zend_class_entry *spl_ce_InfiniteIterator;
extern PHPAPI zend_class_entry *spl_ce_IteratorIterator;
extern PHPAPI zend_class_entry *spl_ce_LimitIterator;
extern PHPAPI zend_class_entry *spl_ce_NoRewindIterator;
extern PHPAPI zend_class_entry *spl_ce_OuterIterator;
extern PHPAPI zend_class_entry *spl_ce_ParentIterator;
extern PHPAPI zend_class_entry *spl_ce_RecursiveCachingIterator;
extern PHPAPI zend_class_entry *spl_ce_RecursiveCallbackFilterIterator;
extern PHPAPI zend_class_entry *spl_ce_RecursiveFilterIterator;
extern PHPAPI zend_class_entry *spl_ce_RecursiveIterator;
extern PHPAPI zend_class_entry *spl_ce_RecursiveIteratorIterator;
extern PHPAPI zend_class_entry *spl_ce_RecursiveRegexIterator;
extern PHPAPI zend_class_entry *spl_ce_RecursiveTreeIterator;
extern PHPAPI zend_class_entry *spl_ce_RegexIterator;
extern PHPAPI zend_class_entry *spl_ce_SeekableIterator;

PHP_MINIT_FUNCTION(spl_iterators);


typedef enum {
	RIT_LEAVES_ONLY = 0,
	RIT_SELF_FIRST  = 1,
	RIT_CHILD_FIRST = 2
} RecursiveIteratorMode;

#define RIT_CATCH_GET_CHILD CIT_CATCH_GET_CHILD

typedef enum {
	RTIT_BYPASS_CURRENT = 4,
	RTIT_BYPASS_KEY	    = 8
} RecursiveTreeIteratorFlags;

typedef enum {
	DIT_Default = 0,
	DIT_FilterIterator = DIT_Default,
	DIT_RecursiveFilterIterator = DIT_Default,
	DIT_ParentIterator = DIT_Default,
	DIT_LimitIterator,
	DIT_CachingIterator,
	DIT_RecursiveCachingIterator,
	DIT_IteratorIterator,
	DIT_NoRewindIterator,
	DIT_InfiniteIterator,
	DIT_AppendIterator,
	DIT_RegexIterator,
	DIT_RecursiveRegexIterator,
	DIT_CallbackFilterIterator,
	DIT_RecursiveCallbackFilterIterator,
	DIT_Unknown = ~0
} dual_it_type;

typedef enum {
	RIT_Default = 0,
	RIT_RecursiveIteratorIterator = RIT_Default,
	RIT_RecursiveTreeIterator,
	RIT_Unknow = ~0
} recursive_it_it_type;

enum {
	/* public */
	CIT_CALL_TOSTRING        = 0x00000001,
	CIT_TOSTRING_USE_KEY     = 0x00000002,
	CIT_TOSTRING_USE_CURRENT = 0x00000004,
	CIT_TOSTRING_USE_INNER   = 0x00000008,
	CIT_CATCH_GET_CHILD      = 0x00000010,
	CIT_FULL_CACHE           = 0x00000100,
	CIT_PUBLIC               = 0x0000FFFF,
	/* private */
	CIT_VALID                = 0x00010000,
	CIT_HAS_CHILDREN         = 0x00020000
};

enum {
	/* public */
	REGIT_USE_KEY            = 0x00000001,
	REGIT_INVERTED           = 0x00000002
};

typedef enum {
	REGIT_MODE_MATCH,
	REGIT_MODE_GET_MATCH,
	REGIT_MODE_ALL_MATCHES,
	REGIT_MODE_SPLIT,
	REGIT_MODE_REPLACE,
	REGIT_MODE_MAX
} regex_mode;

typedef int (*spl_iterator_apply_func_t)(zend_object_iterator *iter, void *puser);

PHPAPI zend_result spl_iterator_apply(zval *obj, spl_iterator_apply_func_t apply_func, void *puser);

#endif /* SPL_ITERATORS_H */
