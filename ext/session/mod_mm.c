/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
 */


#include "php.h"

#ifdef HAVE_LIBMM

#include <mm.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "php_session.h"
#include "mod_mm.h"

#define PS_MM_PATH "/tmp/session_mm"

/*
 * this list holds all data associated with one session 
 */

typedef struct ps_sd {
	struct ps_sd *next, *prev;
	time_t ctime;
	char *key;
	void *data;
	size_t datalen;
} ps_sd;

typedef struct {
	MM *mm;
	ps_sd **hash;
} ps_mm;

static ps_mm *ps_mm_instance = NULL;

/* should be a prime */
#define HASH_SIZE 577

#if 0
#define ps_mm_debug(a...) fprintf(stderr, a)
#else
#define ps_mm_debug(a...)
#endif

#define BITS_IN_int (sizeof(int) * CHAR_BIT)
#define THREE_QUARTERS ((int) ((BITS_IN_int * 3) / 4))
#define ONE_EIGTH ((int) (BITS_IN_int / 8))
#define HIGH_BITS (~((unsigned int)(~0) >> ONE_EIGTH))

/*
 * Weinberger's generic hash algorithm, adapted by Holub
 * (published in [Holub 1990])
 */

static unsigned int ps_sd_hash(const char *data)
{
	unsigned int val, i;
	
	for (val = 0; *data; data++) {
		val = (val << ONE_EIGTH) + *data;
		if ((i = val & HIGH_BITS) != 0)
			val = (val ^ (i >> THREE_QUARTERS)) & ~HIGH_BITS;
	}
	
	return val;
}


static ps_sd *ps_sd_new(ps_mm *data, const char *key, const void *sdata, size_t sdatalen)
{
	unsigned int h;
	ps_sd *sd;

	h = ps_sd_hash(key) % HASH_SIZE;
	
	sd = mm_malloc(data->mm, sizeof(*sd));
	if (!sd)
		return NULL;
	sd->ctime = 0;
	
	sd->data = mm_malloc(data->mm, sdatalen);
	if (!sd->data) {
		mm_free(data->mm, sd);
		return NULL;
	}

	sd->datalen = sdatalen;
	
	sd->key = mm_strdup(data->mm, key);
	if (!sd->key) {
		mm_free(data->mm, sd->data);
		mm_free(data->mm, sd);
		return NULL;
	}
	
	memcpy(sd->data, sdata, sdatalen);
	
	if ((sd->next = data->hash[h]))
		sd->next->prev = sd;
	sd->prev = NULL;
	
	ps_mm_debug("inserting %s(%p) into %d\n", key, sd, h);
	
	data->hash[h] = sd;

	return sd;
}

static void ps_sd_destroy(ps_mm *data, ps_sd *sd)
{
	unsigned int h;

	h = ps_sd_hash(sd->key) % HASH_SIZE;
	
	if (sd->next)
		sd->next->prev = sd->prev;
	if (sd->prev)
		sd->prev->next = sd->next;
	
	if (data->hash[h] == sd)
		data->hash[h] = sd->next;
		
	mm_free(data->mm, sd->key);
	if (sd->data) 
		mm_free(data->mm, sd->data);
	mm_free(data->mm, sd);
}

static ps_sd *ps_sd_lookup(ps_mm *data, const char *key, int rw)
{
	unsigned int h;
	ps_sd *ret;

	h = ps_sd_hash(key) % HASH_SIZE;

	for (ret = data->hash[h]; ret; ret = ret->next)
		if (!strcmp(ret->key, key)) 
			break;

	if (ret && rw && ret != data->hash[h]) {
		data->hash[h]->prev = ret;
		ret->next = data->hash[h];
		data->hash[h] = ret;
		ps_mm_debug("optimizing\n");
	}

	ps_mm_debug(stderr, "lookup(%s): ret=%p,h=%d\n", key, ret, h);
	
	return ret;
}

ps_module ps_mod_mm = {
	PS_MOD(mm)
};

#define PS_MM_DATA ps_mm *data = PS_GET_MOD_DATA()

static int ps_mm_initialize(ps_mm *data, const char *path)
{
	data->mm = mm_create(0, path);
	if (!data->mm) {
		return FAILURE;
	}

	data->hash = mm_calloc(data->mm, HASH_SIZE, sizeof(*data->hash));
	if (!data->hash) {
		mm_destroy(data->mm);
		return FAILURE;
	}

	return SUCCESS;
}

static void ps_mm_destroy(ps_mm *data)
{
	int h;
	ps_sd *sd, *next;

	for (h = 0; h < HASH_SIZE; h++)
		for (sd = data->hash[h]; sd; sd = next) {
			next = sd->next;
			ps_sd_destroy(data, sd);
		}
	
	mm_free(data->mm, data->hash);
	mm_destroy(data->mm);
}

PHP_GINIT_FUNCTION(ps_mm)
{
	ps_mm_instance = calloc(sizeof(*ps_mm_instance), 1);
	if (ps_mm_initialize(ps_mm_instance, PS_MM_PATH) != SUCCESS) {
		ps_mm_instance = NULL;
		return FAILURE;
	}
	return SUCCESS;
}

PHP_GSHUTDOWN_FUNCTION(ps_mm)
{
	if (ps_mm_instance) {
		ps_mm_destroy(ps_mm_instance);
		free(ps_mm_instance);
		return SUCCESS;
	}
	return FAILURE;
}

PS_OPEN_FUNC(mm)
{
	ps_mm_debug("open: ps_mm_instance=%p\n", ps_mm_instance);
	
	if (!ps_mm_instance)
		return FAILURE;
	
	PS_SET_MOD_DATA(ps_mm_instance);
	
	return SUCCESS;
}

PS_CLOSE_FUNC(mm)
{
	PS_SET_MOD_DATA(NULL);

	return SUCCESS;
}

PS_READ_FUNC(mm)
{
	PS_MM_DATA;
	ps_sd *sd;
	int ret = FAILURE;

	mm_lock(data->mm, MM_LOCK_RD);
	
	sd = ps_sd_lookup(data, key, 0);
	if (sd) {
		*vallen = sd->datalen;
		*val = emalloc(sd->datalen);
		memcpy(*val, sd->data, sd->datalen);
		ret = SUCCESS;
	}

	mm_unlock(data->mm);
	
	return ret;
}

PS_WRITE_FUNC(mm)
{
	PS_MM_DATA;
	ps_sd *sd;

	if (vallen == 0) return SUCCESS;
	
	mm_lock(data->mm, MM_LOCK_RW);

	sd = ps_sd_lookup(data, key, 1);
	if (!sd) {
		sd = ps_sd_new(data, key, val, vallen);
		ps_mm_debug(stderr, "new one for %s\n", key);
	} else {
		ps_mm_debug(stderr, "found existing one for %s\n", key);
		mm_free(data->mm, sd->data);
		sd->datalen = vallen;
		sd->data = mm_malloc(data->mm, vallen);
		if (!sd->data) {
			ps_sd_destroy(data, sd);
			sd = NULL;
		} else
			memcpy(sd->data, val, vallen);
	}

	if (sd)
		time(&sd->ctime);

	mm_unlock(data->mm);
	
	return sd ? SUCCESS : FAILURE;
}

PS_DESTROY_FUNC(mm)
{
	PS_MM_DATA;
	ps_sd *sd;
	
	mm_lock(data->mm, MM_LOCK_RW);
	
	sd = ps_sd_lookup(data, key, 0);
	if (sd)
		ps_sd_destroy(data, sd);
	
	mm_unlock(data->mm);
	
	return SUCCESS;
}

PS_GC_FUNC(mm) 
{
	PS_MM_DATA;
	int h;
	time_t now;
	ps_sd *sd, *next;
	
	*nrdels = 0;
	ps_mm_debug("gc\n");
	
	mm_lock(data->mm, MM_LOCK_RW);
	
	time(&now);

	for (h = 0; h < HASH_SIZE; h++)
		for (sd = data->hash[h]; sd; sd = next) {
			next = sd->next;
			ps_mm_debug("looking at %s\n", sd->key);
			if ((now - sd->ctime) > maxlifetime) {
				ps_sd_destroy(data, sd);
				*nrdels++;
			}
		}

	mm_unlock(data->mm);
	
	return SUCCESS;
}

zend_module_entry php_session_mm_module = {
	"Session MM",
	NULL,
	NULL, NULL,
	NULL, NULL,
	NULL,
	PHP_GINIT(ps_mm), PHP_GSHUTDOWN(ps_mm),
	STANDARD_MODULE_PROPERTIES_EX
};

#endif
