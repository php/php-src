/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef SPL_ITERATORS_H
#define SPL_ITERATORS_H

#include "php.h"
#include "php_spl.h"
#if HAVE_PCRE || HAVE_BUNDLED_PCRE
#include "ext/pcre/php_pcre.h"
#endif

#define spl_ce_Traversable   zend_ce_traversable
#define spl_ce_Iterator      zend_ce_iterator
#define spl_ce_Aggregate     zend_ce_aggregate
#define spl_ce_ArrayAccess   zend_ce_arrayaccess
#define spl_ce_Serializable  zend_ce_serializable

extern PHPAPI zend_class_entry *spl_ce_RecursiveIterator;
extern PHPAPI zend_class_entry *spl_ce_RecursiveIteratorIterator;
extern PHPAPI zend_class_entry *spl_ce_RecursiveTreeIterator;
extern PHPAPI zend_class_entry *spl_ce_FilterIterator;
extern PHPAPI zend_class_entry *spl_ce_RecursiveFilterIterator;
extern PHPAPI zend_class_entry *spl_ce_ParentIterator;
extern PHPAPI zend_class_entry *spl_ce_SeekableIterator;
extern PHPAPI zend_class_entry *spl_ce_LimitIterator;
extern PHPAPI zend_class_entry *spl_ce_CachingIterator;
extern PHPAPI zend_class_entry *spl_ce_RecursiveCachingIterator;
extern PHPAPI zend_class_entry *spl_ce_OuterIterator;
extern PHPAPI zend_class_entry *spl_ce_IteratorIterator;
extern PHPAPI zend_class_entry *spl_ce_NoRewindIterator;
extern PHPAPI zend_class_entry *spl_ce_InfiniteIterator;
extern PHPAPI zend_class_entry *spl_ce_EmptyIterator;
extern PHPAPI zend_class_entry *spl_ce_AppendIterator;
extern PHPAPI zend_class_entry *spl_ce_RegexIterator;
extern PHPAPI zend_class_entry *spl_ce_RecursiveRegexIterator;
extern PHPAPI zend_class_entry *spl_ce_Countable;
extern PHPAPI zend_class_entry *spl_ce_CallbackFilterIterator;
extern PHPAPI zend_class_entry *spl_ce_RecursiveCallbackFilterIterator;

PHP_MINIT_FUNCTION(spl_iterators);

PHP_FUNCTION(iterator_to_array);
PHP_FUNCTION(iterator_count);
PHP_FUNCTION(iterator_apply);

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
#if HAVE_PCRE || HAVE_BUNDLED_PCRE
	DIT_RegexIterator,
	DIT_RecursiveRegexIterator,
#endif
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

typedef struct _spl_cbfilter_it_intern {
	zend_fcall_info       fci;
	zend_fcall_info_cache fcc;
	zend_object           *object;
} _spl_cbfilter_it_intern;

typedef struct _spl_dual_it_object {
	struct {
		zval                 zobject;
		zend_class_entry     *ce;
		zend_object          *object;
		zend_object_iterator *iterator;
	} inner;
	struct {
		zval                 data;
		zval                 key;
		zend_long            pos;
	} current;
	dual_it_type             dit_type;
	union {
		struct {
			zend_long             offset;
			zend_long             count;
		} limit;
		struct {
			zend_long             flags; /* CIT_* */
			zval             zstr;
			zval             zchildren;
			zval             zcache;
		} caching;
		struct {
			zval                  zarrayit;
			zend_object_iterator *iterator;
		} append;
#if HAVE_PCRE || HAVE_BUNDLED_PCRE
		struct {
			zend_long        flags;
			zend_long        preg_flags;
			pcre_cache_entry *pce;
			zend_string      *regex;
			regex_mode       mode;
			int              use_flags;
		} regex;
#endif
		_spl_cbfilter_it_intern *cbfilter;
	} u;
	zend_object              std;
} spl_dual_it_object;

static inline spl_dual_it_object *spl_dual_it_from_obj(zend_object *obj) /* {{{ */ {
	return (spl_dual_it_object*)((char*)(obj) - XtOffsetOf(spl_dual_it_object, std));
} /* }}} */

#define Z_SPLDUAL_IT_P(zv)  spl_dual_it_from_obj(Z_OBJ_P((zv)))

typedef int (*spl_iterator_apply_func_t)(zend_object_iterator *iter, void *puser);

PHPAPI int spl_iterator_apply(zval *obj, spl_iterator_apply_func_t apply_func, void *puser);

#endif /* SPL_ITERATORS_H */

/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
