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
   | With suggestions from:                                               |
   |      Moriyoshi Koizumi <moriyoshi@at.wakwak.com>                     |
   |      Sara Golemon      <pollita@php.net>                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* The filter API works on the principle of "Bucket-Brigades".  This is
 * partially inspired by the Apache 2 method of doing things, although
 * it is intentially a light-weight implementation.
 *
 * Each stream can have a chain of filters for reading and another for writing.
 * 
 * When data is written to the stream, is is placed into a bucket and placed at
 * the start of the input brigade.
 *
 * The first filter in the chain is invoked on the brigade and (depending on
 * it's return value), the next filter is invoked and so on.
 * */

typedef struct _php_stream_bucket			php_stream_bucket;
typedef struct _php_stream_bucket_brigade	php_stream_bucket_brigade;

struct _php_stream_bucket {
	php_stream_bucket *next, *prev;
	php_stream_bucket_brigade *brigade;

	char *buf;
	size_t buflen;
	/* if non-zero, buf should be pefreed when the bucket is destroyed */
	int own_buf;
	int is_persistent;
	
	/* destroy this struct when refcount falls to zero */
	int refcount;
};

struct _php_stream_bucket_brigade {
	php_stream_bucket *head, *tail;
};

typedef enum {
	PSFS_ERR_FATAL,	/* error in data stream */
	PSFS_FEED_ME,	/* filter needs more data; stop processing chain until more is available */
	PSFS_PASS_ON,	/* filter generated output buckets; pass them on to next in chain */
} php_stream_filter_status_t;

/* Buckets API. */
PHPAPI php_stream_bucket *php_stream_bucket_new(php_stream *stream, char *buf, size_t buflen, int own_buf, int buf_persistent TSRMLS_DC);
PHPAPI int php_stream_bucket_split(php_stream_bucket *in, php_stream_bucket **left, php_stream_bucket **right, size_t length TSRMLS_DC);
PHPAPI void php_stream_bucket_delref(php_stream_bucket *bucket TSRMLS_DC);
#define php_stream_bucket_addref(bucket)	(bucket)->refcount++
PHPAPI void php_stream_bucket_prepend(php_stream_bucket_brigade *brigade, php_stream_bucket *bucket TSRMLS_DC);
PHPAPI void php_stream_bucket_append(php_stream_bucket_brigade *brigade, php_stream_bucket *bucket TSRMLS_DC);
PHPAPI void php_stream_bucket_unlink(php_stream_bucket *bucket TSRMLS_DC);
PHPAPI php_stream_bucket *php_stream_bucket_make_writeable(php_stream_bucket *bucket TSRMLS_DC);

#define PSFS_FLAG_NORMAL		0	/* regular read/write */
#define PSFS_FLAG_FLUSH_INC		1	/* an incremental flush */
#define PSFS_FLAG_FLUSH_CLOSE	2	/* final flush prior to closing */

typedef struct _php_stream_filter_ops {

	php_stream_filter_status_t (*filter)(
			php_stream *stream,
			php_stream_filter *thisfilter,
			php_stream_bucket_brigade *buckets_in,
			php_stream_bucket_brigade *buckets_out,
			size_t *bytes_consumed,
			int flags
			TSRMLS_DC);
	
	void (*dtor)(php_stream_filter *thisfilter TSRMLS_DC);
	
	const char *label;
	
} php_stream_filter_ops;

typedef struct _php_stream_filter_chain {
	php_stream_filter *head, *tail;
} php_stream_filter_chain;

struct _php_stream_filter {
	php_stream_filter_ops *fops;
	void *abstract; /* for use by filter implementation */
	php_stream_filter *next;
	php_stream_filter *prev;
	int is_persistent;

	/* link into stream and chain */
	php_stream_filter_chain *chain;

	/* buffered buckets */
	php_stream_bucket_brigade buffer;
};

/* stack filter onto a stream */
PHPAPI void php_stream_filter_prepend(php_stream_filter_chain *chain, php_stream_filter *filter);
PHPAPI void php_stream_filter_append(php_stream_filter_chain *chain, php_stream_filter *filter);
PHPAPI php_stream_filter *php_stream_filter_remove(php_stream_filter *filter, int call_dtor TSRMLS_DC);
PHPAPI void php_stream_filter_free(php_stream_filter *filter TSRMLS_DC);
PHPAPI php_stream_filter *_php_stream_filter_alloc(php_stream_filter_ops *fops, void *abstract, int persistent STREAMS_DC TSRMLS_DC);
#define php_stream_filter_alloc(fops, thisptr, persistent) _php_stream_filter_alloc((fops), (thisptr), (persistent) STREAMS_CC TSRMLS_CC)
#define php_stream_filter_alloc_rel(fops, thisptr, persistent) _php_stream_filter_alloc((fops), (thisptr), (persistent) STREAMS_REL_CC TSRMLS_CC)

#define php_stream_is_filtered(stream)	((stream)->readfilters.head || (stream)->writefilters.head)

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
