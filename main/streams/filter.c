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
   | Authors: Wez Furlong <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_globals.h"
#include "php_network.h"
#include "php_open_temporary_file.h"
#include "ext/standard/file.h"
#include <stddef.h>
#include <fcntl.h>
#include <string.h>

#include "php_streams_int.h"

/* Global filter hash, copied to FG(stream_filters) on registration of volatile filter */
static HashTable stream_filters_hash;

/* Should only be used during core initialization */
PHPAPI HashTable *php_get_stream_filters_hash_global(void)
{
    return &stream_filters_hash;
}

/* Normal hash selection/retrieval call */
PHPAPI HashTable *_php_get_stream_filters_hash(void)
{
    return FG(stream_filters) ?: &stream_filters_hash;
}

/* API for registering GLOBAL filters */
PHPAPI int php_stream_filter_register_factory(const char *filterpattern, const php_stream_filter_factory *factory)
{
    zend_string *str = zend_string_init_interned(filterpattern, strlen(filterpattern), 1);
    int ret = zend_hash_add_ptr(&stream_filters_hash, str, (void*)factory) ? SUCCESS : FAILURE;
    zend_string_release_ex(str, 1);
    return ret;
}

PHPAPI int php_stream_filter_unregister_factory(const char *filterpattern)
{
    return zend_hash_str_del(&stream_filters_hash, filterpattern, strlen(filterpattern));
}

/* API for registering VOLATILE wrappers */
PHPAPI int php_stream_filter_register_factory_volatile(zend_string *filterpattern, const php_stream_filter_factory *factory)
{
    if (UNEXPECTED(!FG(stream_filters))) {
        ALLOC_HASHTABLE(FG(stream_filters));
        zend_hash_init(FG(stream_filters), zend_hash_num_elements(&stream_filters_hash) + 1, NULL, NULL, 0);
        zend_hash_copy(FG(stream_filters), &stream_filters_hash, NULL);
    }

    return zend_hash_add_ptr(FG(stream_filters), filterpattern, (void*)factory) ? SUCCESS : FAILURE;
}

/* Buckets */

PHPAPI php_stream_bucket *php_stream_bucket_new(php_stream *stream, char *buf, size_t buflen, uint8_t own_buf, uint8_t buf_persistent)
{
    bool is_persistent = php_stream_is_persistent(stream);
    php_stream_bucket *bucket = pemalloc(sizeof(php_stream_bucket), is_persistent);
    
    bucket->next = bucket->prev = NULL;
    bucket->brigade = NULL;
    bucket->refcount = 1;
    bucket->is_persistent = is_persistent;

    if (is_persistent && !buf_persistent) {
        /* all data in a persistent bucket must also be persistent */
        bucket->buf = pemalloc(buflen, 1);
        memcpy(bucket->buf, buf, buflen);
        bucket->buflen = buflen;
        bucket->own_buf = 1;
    } else {
        bucket->buf = buf;
        bucket->buflen = buflen;
        bucket->own_buf = own_buf;
    }

    return bucket;
}

PHPAPI php_stream_bucket *php_stream_bucket_make_writeable(php_stream_bucket *bucket)
{
    php_stream_bucket_unlink(bucket);

    if (bucket->refcount == 1 && bucket->own_buf) {
        return bucket;
    }

    php_stream_bucket *retval = pemalloc(sizeof(*retval), bucket->is_persistent);
    memcpy(retval, bucket, sizeof(*retval));

    retval->buf = pemalloc(retval->buflen, retval->is_persistent);
    memcpy(retval->buf, bucket->buf, retval->buflen);
    retval->refcount = 1;
    retval->own_buf = 1;

    php_stream_bucket_delref(bucket);
    return retval;
}

PHPAPI int php_stream_bucket_split(php_stream_bucket *in, php_stream_bucket **left, php_stream_bucket **right, size_t length)
{
    *left = pecalloc(1, sizeof(**left), in->is_persistent);
    *right = pecalloc(1, sizeof(**right), in->is_persistent);

    (*left)->buf = pemalloc(length, in->is_persistent);
    (*left)->buflen = length;
    memcpy((*left)->buf, in->buf, length);
    (*left)->refcount = 1;
    (*left)->own_buf = 1;
    (*left)->is_persistent = in->is_persistent;

    size_t right_len = in->buflen - length;
    (*right)->buf = pemalloc(right_len, in->is_persistent);
    (*right)->buflen = right_len;
    memcpy((*right)->buf, in->buf + length, right_len);
    (*right)->refcount = 1;
    (*right)->own_buf = 1;
    (*right)->is_persistent = in->is_persistent;

    return SUCCESS;
}

PHPAPI void php_stream_bucket_delref(php_stream_bucket *bucket)
{
    if (--bucket->refcount == 0) {
        if (bucket->own_buf) {
            pefree(bucket->buf, bucket->is_persistent);
        }
        pefree(bucket, bucket->is_persistent);
    }
}

PHPAPI void php_stream_bucket_prepend(php_stream_bucket_brigade *brigade, php_stream_bucket *bucket)
{
    bucket->next = brigade->head;
    bucket->prev = NULL;

    if (brigade->head) {
        brigade->head->prev = bucket;
    } else {
        brigade->tail = bucket;
    }
    brigade->head = bucket;
    bucket->brigade = brigade;
}

PHPAPI void php_stream_bucket_append(php_stream_bucket_brigade *brigade, php_stream_bucket *bucket)
{
    if (UNEXPECTED(brigade->tail == bucket)) {
        return;
    }

    bucket->prev = brigade->tail;
    bucket->next = NULL;

    if (brigade->tail) {
        brigade->tail->next = bucket;
    } else {
        brigade->head = bucket;
    }
    brigade->tail = bucket;
    bucket->brigade = brigade;
}

PHPAPI void php_stream_bucket_unlink(php_stream_bucket *bucket)
{
    if (bucket->prev) {
        bucket->prev->next = bucket->next;
    } else if (bucket->brigade) {
        bucket->brigade->head = bucket->next;
    }
    
    if (bucket->next) {
        bucket->next->prev = bucket->prev;
    } else if (bucket->brigade) {
        bucket->brigade->tail = bucket->prev;
    }
    
    bucket->brigade = NULL;
    bucket->next = bucket->prev = NULL;
}

PHPAPI php_stream_filter *php_stream_filter_create(const char *filtername, zval *filterparams, uint8_t persistent)
{
    HashTable *filter_hash = _php_get_stream_filters_hash();
    const php_stream_filter_factory *factory = NULL;
    size_t n = strlen(filtername);
    
    if ((factory = zend_hash_str_find_ptr(filter_hash, filtername, n))) {
        return factory->create_filter(filtername, filterparams, persistent);
    }

    char *period = strrchr(filtername, '.');
    if (!period) {
        php_error_docref(NULL, E_WARNING, "Unable to locate filter \"%s\"", filtername);
        return NULL;
    }

    /* try a wildcard */
    size_t wild_len = n + 2; /* original + '*' + '\0' */
    char *wildname = do_alloca(wild_len);
    memcpy(wildname, filtername, n + 1);
    period = wildname + (period - filtername);
    
    php_stream_filter *filter = NULL;
    while (period && !filter) {
        ZEND_ASSERT(period[0] == '.');
        period[1] = '*';
        period[2] = '\0';
        
        if ((factory = zend_hash_str_find_ptr(filter_hash, wildname, strlen(wildname)))) {
            filter = factory->create_filter(filtername, filterparams, persistent);
        }

        *period = '\0';
        period = strrchr(wildname, '.');
    }
    
    free_alloca(wildname, wild_len);

    if (!filter) {
        php_error_docref(NULL, E_WARNING, "Unable to create or locate filter \"%s\"", filtername);
    }

    return filter;
}

PHPAPI php_stream_filter *_php_stream_filter_alloc(const php_stream_filter_ops *fops, void *abstract, uint8_t persistent STREAMS_DC)
{
    php_stream_filter *filter = pemalloc_rel_orig(sizeof(*filter), persistent);
    memset(filter, 0, sizeof(*filter));

    filter->fops = fops;
    Z_PTR(filter->abstract) = abstract;
    filter->is_persistent = persistent;

    return filter;
}

PHPAPI void php_stream_filter_free(php_stream_filter *filter)
{
    if (filter->fops->dtor) {
        filter->fops->dtor(filter);
    }
    pefree(filter, filter->is_persistent);
}

PHPAPI int php_stream_filter_prepend_ex(php_stream_filter_chain *chain, php_stream_filter *filter)
{
    filter->next = chain->head;
    filter->prev = NULL;

    if (chain->head) {
        chain->head->prev = filter;
    } else {
        chain->tail = filter;
    }
    chain->head = filter;
    filter->chain = chain;

    return SUCCESS;
}

PHPAPI void _php_stream_filter_prepend(php_stream_filter_chain *chain, php_stream_filter *filter)
{
    php_stream_filter_prepend_ex(chain, filter);
}

PHPAPI int php_stream_filter_append_ex(php_stream_filter_chain *chain, php_stream_filter *filter)
{
    php_stream *stream = chain->stream;

    filter->prev = chain->tail;
    filter->next = NULL;
    
    if (chain->tail) {
        chain->tail->next = filter;
    } else {
        chain->head = filter;
    }
    chain->tail = filter;
    filter->chain = chain;

    if (&stream->readfilters == chain && stream->writepos > stream->readpos) {
        php_stream_bucket_brigade brig_in = {0}, brig_out = {0};
        php_stream_bucket *bucket = php_stream_bucket_new(stream, 
            (char*)stream->readbuf + stream->readpos, 
            stream->writepos - stream->readpos, 0, 0);
            
        php_stream_bucket_append(&brig_in, bucket);
        
        size_t consumed = 0;
        php_stream_filter_status_t status = filter->fops->filter(
            stream, filter, &brig_in, &brig_out, &consumed, PSFS_FLAG_NORMAL);

        if (UNEXPECTED(stream->readpos + consumed > (uint32_t)stream->writepos)) {
            status = PSFS_ERR_FATAL;
        }

        switch (status) {
            case PSFS_ERR_FATAL:
                php_stream_bucket_brigade_cleanup(&brig_in);
                php_stream_bucket_brigade_cleanup(&brig_out);
                php_error_docref(NULL, E_WARNING, "Filter failed to process pre-buffered data");
                return FAILURE;
                
            case PSFS_FEED_ME:
                stream->readpos = stream->writepos = 0;
                break;
                
            case PSFS_PASS_ON:
                stream->writepos = stream->readpos = 0;
                
                while (brig_out.head) {
                    bucket = brig_out.head;
                    size_t needed = bucket->buflen;
                    
                    if (stream->readbuflen - stream->writepos < needed) {
                        stream->readbuflen += needed;
                        stream->readbuf = perealloc(stream->readbuf, stream->readbuflen, stream->is_persistent);
                    }
                    
                    memcpy(stream->readbuf + stream->writepos, bucket->buf, needed);
                    stream->writepos += needed;
                    
                    php_stream_bucket_unlink(bucket);
                    php_stream_bucket_delref(bucket);
                }
                break;
        }
    }

    return SUCCESS;
}

PHPAPI void _php_stream_filter_append(php_stream_filter_chain *chain, php_stream_filter *filter)
{
    if (php_stream_filter_append_ex(chain, filter) != SUCCESS) {
        if (chain->head == filter) {
            chain->head = chain->tail = NULL;
        } else {
            filter->prev->next = NULL;
            chain->tail = filter->prev;
        }
    }
}

PHPAPI int _php_stream_filter_flush(php_stream_filter *filter, int finish)
{
    if (UNEXPECTED(!filter->chain || !filter->chain->stream)) {
        return FAILURE;
    }

    php_stream_filter_chain *chain = filter->chain;
    php_stream *stream = chain->stream;
    php_stream_bucket_brigade brig_a = {0}, brig_b = {0};
    php_stream_bucket_brigade *inp = &brig_a, *outp = &brig_b;
    long flags = finish ? PSFS_FLAG_FLUSH_CLOSE : PSFS_FLAG_FLUSH_INC;
    size_t flushed_size = 0;

    for (php_stream_filter *current = filter; current; current = current->next) {
        php_stream_filter_status_t status = current->fops->filter(
            stream, current, inp, outp, NULL, flags);
            
        if (status == PSFS_FEED_ME) {
            return SUCCESS;
        }
        if (status == PSFS_ERR_FATAL) {
            php_stream_bucket_brigade_cleanup(inp);
            return FAILURE;
        }
        
        /* Swap brigades for next iteration */
        php_stream_bucket_brigade *temp = inp;
        inp = outp;
        outp = temp;
        outp->head = outp->tail = NULL;
        flags = PSFS_FLAG_NORMAL;
    }

    /* Calculate total flushed size */
    for (php_stream_bucket *bucket = inp->head; bucket; bucket = bucket->next) {
        flushed_size += bucket->buflen;
    }

    if (flushed_size == 0) {
        return SUCCESS;
    }

    if (chain == &stream->readfilters) {
        /* Compact read buffer if needed */
        if (stream->readpos > 0) {
            size_t valid_data = stream->writepos - stream->readpos;
            memmove(stream->readbuf, stream->readbuf + stream->readpos, valid_data);
            stream->readpos = 0;
            stream->writepos = valid_data;
        }
        
        /* Ensure enough space */
        if (flushed_size > stream->readbuflen - stream->writepos) {
            stream->readbuflen = stream->writepos + flushed_size + stream->chunk_size;
            stream->readbuf = perealloc(stream->readbuf, stream->readbuflen, stream->is_persistent);
        }
        
        /* Append all buckets */
        while (inp->head) {
            php_stream_bucket *bucket = inp->head;
            memcpy(stream->readbuf + stream->writepos, bucket->buf, bucket->buflen);
            stream->writepos += bucket->buflen;
            php_stream_bucket_unlink(bucket);
            php_stream_bucket_delref(bucket);
        }
    } 
    else if (chain == &stream->writefilters) {
        /* Write all buckets to stream */
        while (inp->head) {
            php_stream_bucket *bucket = inp->head;
            ssize_t count = stream->ops->write(stream, bucket->buf, bucket->buflen);
            if (count > 0) {
                stream->position += count;
            }
            php_stream_bucket_unlink(bucket);
            php_stream_bucket_delref(bucket);
        }
    }

    return SUCCESS;
}

PHPAPI php_stream_filter *php_stream_filter_remove(php_stream_filter *filter, int call_dtor)
{
    if (filter->prev) {
        filter->prev->next = filter->next;
    } else {
        filter->chain->head = filter->next;
    }
    
    if (filter->next) {
        filter->next->prev = filter->prev;
    } else {
        filter->chain->tail = filter->prev;
    }

    if (filter->res) {
        zend_list_delete(filter->res);
    }

    if (call_dtor) {
        php_stream_filter_free(filter);
        return NULL;
    }
    return filter;
}

/* Helper function to clean up a brigade */
static void php_stream_bucket_brigade_cleanup(php_stream_bucket_brigade *brigade)
{
    while (brigade->head) {
        php_stream_bucket *bucket = brigade->head;
        php_stream_bucket_unlink(bucket);
        php_stream_bucket_delref(bucket);
    }
}
