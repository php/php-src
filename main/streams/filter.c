/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   | Wez Furlong (wez@thebrainroom.com)                                   |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include "php_globals.h"
#include "php_network.h"
#include "php_open_temporary_file.h"
#include "ext/standard/file.h"
#include <stddef.h>
#include <fcntl.h>

#include "php_streams_int.h"

static HashTable stream_filters_hash;

PHPAPI HashTable *php_get_stream_filters_hash()
{
	return &stream_filters_hash;
}

PHPAPI int php_stream_filter_register_factory(const char *filterpattern, php_stream_filter_factory *factory TSRMLS_DC)
{
	return zend_hash_add(&stream_filters_hash, (char*)filterpattern, strlen(filterpattern), factory, sizeof(*factory), NULL);
}

PHPAPI int php_stream_filter_unregister_factory(const char *filterpattern TSRMLS_DC)
{
	return zend_hash_del(&stream_filters_hash, (char*)filterpattern, strlen(filterpattern));
}

/* We allow very simple pattern matching for filter factories:
 * if "charset.utf-8/sjis" is requested, we search first for an exact
 * match. If that fails, we try "charset.*".
 * This means that we don't need to clog up the hashtable with a zillion
 * charsets (for example) but still be able to provide them all as filters */
PHPAPI php_stream_filter *php_stream_filter_create(const char *filtername, const char *filterparams, int filterparamslen, int persistent TSRMLS_DC)
{
	php_stream_filter_factory *factory;
	php_stream_filter *filter = NULL;
	int n;
	char *period;

	n = strlen(filtername);
	
	if (SUCCESS == zend_hash_find(&stream_filters_hash, (char*)filtername, n, (void**)&factory)) {
		filter = factory->create_filter(filtername, filterparams, filterparamslen, persistent TSRMLS_CC);
	} else if ((period = strchr(filtername, '.'))) {
		/* try a wildcard */
		char wildname[128];

		PHP_STRLCPY(wildname, filtername, sizeof(wildname) - 1, period-filtername + 1);
		strcat(wildname, "*");
		
		if (SUCCESS == zend_hash_find(&stream_filters_hash, wildname, strlen(wildname), (void**)&factory)) {
			filter = factory->create_filter(filtername, filterparams, filterparamslen, persistent TSRMLS_CC);
		}
	}

	if (filter == NULL) {
		/* TODO: these need correct docrefs */
		if (factory == NULL)
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to locate filter \"%s\"", filtername);
		else
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to create or locate filter \"%s\"", filtername);
	}
	
	return filter;
}

PHPAPI php_stream_filter *_php_stream_filter_alloc(php_stream_filter_ops *fops, void *abstract, int persistent STREAMS_DC TSRMLS_DC)
{
	php_stream_filter *filter;

	filter = (php_stream_filter*) pemalloc_rel_orig(sizeof(php_stream_filter), persistent);
	memset(filter, 0, sizeof(php_stream_filter));

	filter->fops = fops;
	filter->abstract = abstract;
	filter->is_persistent = persistent;
	
	return filter;
}

PHPAPI void php_stream_filter_free(php_stream_filter *filter TSRMLS_DC)
{
	if (filter->fops->dtor)
		filter->fops->dtor(filter TSRMLS_CC);
	pefree(filter, filter->is_persistent);
}

PHPAPI void php_stream_filter_prepend(php_stream *stream, php_stream_filter *filter)
{
	filter->next = stream->filterhead;
	filter->prev = NULL;

	if (stream->filterhead) {
		stream->filterhead->prev = filter;
	} else {
		stream->filtertail = filter;
	}
	stream->filterhead = filter;
	filter->stream = stream;
}

PHPAPI void php_stream_filter_append(php_stream *stream, php_stream_filter *filter)
{
	filter->prev = stream->filtertail;
	filter->next = NULL;
	if (stream->filtertail) {
		stream->filtertail->next = filter;
	} else {
		stream->filterhead = filter;
	}
	stream->filtertail = filter;
	filter->stream = stream;
}

PHPAPI php_stream_filter *php_stream_filter_remove(php_stream *stream, php_stream_filter *filter, int call_dtor TSRMLS_DC)
{
	assert(stream == filter->stream);
	
	if (filter->prev) {
		filter->prev->next = filter->next;
	} else {
		stream->filterhead = filter->next;
	}
	if (filter->next) {
		filter->next->prev = filter->prev;
	} else {
		stream->filtertail = filter->prev;
	}
	if (call_dtor) {
		php_stream_filter_free(filter TSRMLS_CC);
		return NULL;
	}
	return filter;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
