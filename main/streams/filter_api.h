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
   | Author: Wez Furlong (wez@thebrainroom.com)                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

typedef struct _php_stream_filter_ops {
	size_t (*write)(php_stream *stream, php_stream_filter *thisfilter,
			const char *buf, size_t count TSRMLS_DC);
	size_t (*read)(php_stream *stream, php_stream_filter *thisfilter,
			char *buf, size_t count TSRMLS_DC);
	int (*flush)(php_stream *stream, php_stream_filter *thisfilter, int closing TSRMLS_DC);
	int (*eof)(php_stream *stream, php_stream_filter *thisfilter TSRMLS_DC);
	void (*dtor)(php_stream_filter *thisfilter TSRMLS_DC);
	const char *label;
} php_stream_filter_ops;

struct _php_stream_filter {
	php_stream_filter_ops *fops;
	void *abstract; /* for use by filter implementation */
	php_stream_filter *next;
	php_stream_filter *prev;
	int is_persistent;
	php_stream *stream;
};

#define php_stream_filter_write_next(stream, thisfilter, buf, size) \
	(thisfilter)->next ? (thisfilter)->next->fops->write((stream), (thisfilter)->next, (buf), (size) TSRMLS_CC) \
	: (stream)->ops->write((stream), (buf), (size) TSRMLS_CC)

#define php_stream_filter_read_next(stream, thisfilter, buf, size) \
	(thisfilter)->next ? (thisfilter)->next->fops->read((stream), (thisfilter)->next, (buf), (size) TSRMLS_CC) \
	: (stream)->ops->read((stream), (buf), (size) TSRMLS_CC)

#define php_stream_filter_flush_next(stream, thisfilter, closing) \
	(thisfilter)->next ? (thisfilter)->next->fops->flush((stream), (thisfilter)->next, (closing) TSRMLS_CC) \
	: (stream)->ops->flush((stream) TSRMLS_CC)

#define php_stream_filter_eof_next(stream, thisfilter) \
	(thisfilter)->next ? (thisfilter)->next->fops->eof((stream), (thisfilter)->next TSRMLS_CC) \
	: (stream)->ops->read((stream), NULL, 0 TSRMLS_CC) == EOF ? 1 : 0

/* stack filter onto a stream */
PHPAPI void php_stream_filter_prepend(php_stream *stream, php_stream_filter *filter);
PHPAPI void php_stream_filter_append(php_stream *stream, php_stream_filter *filter);
PHPAPI php_stream_filter *php_stream_filter_remove(php_stream *stream, php_stream_filter *filter, int call_dtor TSRMLS_DC);
PHPAPI void php_stream_filter_free(php_stream_filter *filter TSRMLS_DC);
PHPAPI php_stream_filter *_php_stream_filter_alloc(php_stream_filter_ops *fops, void *abstract, int persistent STREAMS_DC TSRMLS_DC);
PHPAPI char *php_stream_get_record(php_stream *stream, size_t maxlen, size_t *returned_len, char *delim, size_t delim_len TSRMLS_DC);
#define php_stream_filter_alloc(fops, thisptr, persistent) _php_stream_filter_alloc((fops), (thisptr), (persistent) STREAMS_CC TSRMLS_CC)
#define php_stream_filter_alloc_rel(fops, thisptr, persistent) _php_stream_filter_alloc((fops), (thisptr), (persistent) STREAMS_REL_CC TSRMLS_CC)

#define php_stream_filter_remove_head(stream, call_dtor)	php_stream_filter_remove((stream), (stream)->filterhead, (call_dtor) TSRMLS_CC)
#define php_stream_filter_remove_tail(stream, call_dtor)	php_stream_filter_remove((stream), (stream)->filtertail, (call_dtor) TSRMLS_CC)

typedef struct _php_stream_filter_factory {
	php_stream_filter *(*create_filter)(const char *filtername, const char *filterparams, int filterparamslen, int persistent TSRMLS_DC);
} php_stream_filter_factory;

PHPAPI int php_stream_filter_register_factory(const char *filterpattern, php_stream_filter_factory *factory TSRMLS_DC);
PHPAPI int php_stream_filter_unregister_factory(const char *filterpattern TSRMLS_DC);
PHPAPI php_stream_filter *php_stream_filter_create(const char *filtername, const char *filterparams, int filterparamslen, int persistent TSRMLS_DC);

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
