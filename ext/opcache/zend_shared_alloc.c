/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

#include <errno.h>
#include "ZendAccelerator.h"
#include "zend_shared_alloc.h"
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <fcntl.h>
#ifndef ZEND_WIN32
# include <sys/types.h>
# include <dirent.h>
# include <signal.h>
# include <sys/stat.h>
# include <stdio.h>
#endif

#ifdef HAVE_MPROTECT
# include "sys/mman.h"
#endif

#define SEM_FILENAME_PREFIX ".ZendSem."
#define S_H(s) g_shared_alloc_handler->s

/* True globals */
/* old/new mapping. We can use true global even for ZTS because its usage
   is wrapped with exclusive lock anyway */
static const zend_shared_memory_handlers *g_shared_alloc_handler = NULL;
static const char *g_shared_model;
/* pointer to globals allocated in SHM and shared across processes */
zend_smm_shared_globals *smm_shared_globals;

#ifndef ZEND_WIN32
#ifdef ZTS
static MUTEX_T zts_lock;
#endif
int lock_file;
static char lockfile_name[MAXPATHLEN];
#endif

static const zend_shared_memory_handler_entry handler_table[] = {
#ifdef USE_MMAP
	{ "mmap", &zend_alloc_mmap_handlers },
#endif
#ifdef USE_SHM
	{ "shm", &zend_alloc_shm_handlers },
#endif
#ifdef USE_SHM_OPEN
	{ "posix", &zend_alloc_posix_handlers },
#endif
#ifdef ZEND_WIN32
	{ "win32", &zend_alloc_win32_handlers },
#endif
	{ NULL, NULL}
};

#ifndef ZEND_WIN32
void zend_shared_alloc_create_lock(char *lockfile_path)
{
	int val;

#ifdef ZTS
    zts_lock = tsrm_mutex_alloc();
#endif

	snprintf(lockfile_name, sizeof(lockfile_name), "%s/%sXXXXXX", lockfile_path, SEM_FILENAME_PREFIX);
	lock_file = mkstemp(lockfile_name);
	fchmod(lock_file, 0666);

	if (lock_file == -1) {
		zend_accel_error(ACCEL_LOG_FATAL, "Unable to create lock file: %s (%d)", strerror(errno), errno);
	}
	val = fcntl(lock_file, F_GETFD, 0);
	val |= FD_CLOEXEC;
	fcntl(lock_file, F_SETFD, val);

	unlink(lockfile_name);
}
#endif

static void no_memory_bailout(size_t allocate_size, char *error)
{
	zend_accel_error(ACCEL_LOG_FATAL, "Unable to allocate shared memory segment of %ld bytes: %s: %s (%d)", allocate_size, error?error:"unknown", strerror(errno), errno );
}

static void copy_shared_segments(void *to, void *from, int count, int size)
{
	zend_shared_segment **shared_segments_v = (zend_shared_segment **)to;
	void *shared_segments_to_p = ((char *)to + count*(sizeof(void *)));
	void *shared_segments_from_p = from;
	int i;

	for (i = 0; i < count; i++) {
		shared_segments_v[i] = 	shared_segments_to_p;
		memcpy(shared_segments_to_p, shared_segments_from_p, size);
		shared_segments_to_p = ((char *)shared_segments_to_p + size);
		shared_segments_from_p = ((char *)shared_segments_from_p + size);
	}
}

static int zend_shared_alloc_try(const zend_shared_memory_handler_entry *he, size_t requested_size, zend_shared_segment ***shared_segments_p, int *shared_segments_count, char **error_in)
{
	int res;
	g_shared_alloc_handler = he->handler;
	g_shared_model = he->name;
	ZSMMG(shared_segments) = NULL;
	ZSMMG(shared_segments_count) = 0;

	res = S_H(create_segments)(requested_size, shared_segments_p, shared_segments_count, error_in);

	if (res) {
		/* this model works! */
		return res;
	}
	if (*shared_segments_p) {
		int i;
		/* cleanup */
		for (i = 0; i < *shared_segments_count; i++) {
			if ((*shared_segments_p)[i]->p && (*shared_segments_p)[i]->p != (void *)-1) {
				S_H(detach_segment)((*shared_segments_p)[i]);
			}
		}
		free(*shared_segments_p);
		*shared_segments_p = NULL;
	}
	g_shared_alloc_handler = NULL;
	return ALLOC_FAILURE;
}

int zend_shared_alloc_startup(size_t requested_size)
{
	zend_shared_segment **tmp_shared_segments;
	size_t shared_segments_array_size;
	zend_smm_shared_globals tmp_shared_globals, *p_tmp_shared_globals;
	char *error_in = NULL;
	const zend_shared_memory_handler_entry *he;
	int res = ALLOC_FAILURE;


	/* shared_free must be valid before we call zend_shared_alloc()
	 * - make it temporarily point to a local variable
	 */
	smm_shared_globals = &tmp_shared_globals;
	ZSMMG(shared_free) = requested_size; /* goes to tmp_shared_globals.shared_free */

#ifndef ZEND_WIN32
	zend_shared_alloc_create_lock(ZCG(accel_directives).lockfile_path);
#else
	zend_shared_alloc_create_lock();
#endif

	if (ZCG(accel_directives).memory_model && ZCG(accel_directives).memory_model[0]) {
		char *model = ZCG(accel_directives).memory_model;
		/* "cgi" is really "shm"... */
		if (strncmp(ZCG(accel_directives).memory_model, "cgi", sizeof("cgi")) == 0) {
			model = "shm";
		}

		for (he = handler_table; he->name; he++) {
			if (strcmp(model, he->name) == 0) {
				res = zend_shared_alloc_try(he, requested_size, &ZSMMG(shared_segments), &ZSMMG(shared_segments_count), &error_in);
				if (res) {
					/* this model works! */
				}
				break;
			}
		}
	}

	if (res == FAILED_REATTACHED) {
		smm_shared_globals = NULL;
		return res;
	}
#if ENABLE_FILE_CACHE_FALLBACK
	if (ALLOC_FALLBACK == res) {
		return ALLOC_FALLBACK;
	}
#endif

	if (!g_shared_alloc_handler) {
		/* try memory handlers in order */
		for (he = handler_table; he->name; he++) {
			res = zend_shared_alloc_try(he, requested_size, &ZSMMG(shared_segments), &ZSMMG(shared_segments_count), &error_in);
			if (res) {
				/* this model works! */
				break;
			}
		}
	}

	if (!g_shared_alloc_handler) {
		no_memory_bailout(requested_size, error_in);
		return ALLOC_FAILURE;
	}

	if (res == SUCCESSFULLY_REATTACHED) {
		return res;
	}
#if ENABLE_FILE_CACHE_FALLBACK
	if (ALLOC_FALLBACK == res) {
		return ALLOC_FALLBACK;
	}
#endif

	shared_segments_array_size = ZSMMG(shared_segments_count) * S_H(segment_type_size)();

	/* move shared_segments and shared_free to shared memory */
	ZCG(locked) = 1; /* no need to perform a real lock at this point */
	p_tmp_shared_globals = (zend_smm_shared_globals *) zend_shared_alloc(sizeof(zend_smm_shared_globals));
	if (!p_tmp_shared_globals) {
		zend_accel_error(ACCEL_LOG_FATAL, "Insufficient shared memory!");
		return ALLOC_FAILURE;;
	}

	tmp_shared_segments = zend_shared_alloc(shared_segments_array_size + ZSMMG(shared_segments_count) * sizeof(void *));
	if (!tmp_shared_segments) {
		zend_accel_error(ACCEL_LOG_FATAL, "Insufficient shared memory!");
		return ALLOC_FAILURE;;
	}

	copy_shared_segments(tmp_shared_segments, ZSMMG(shared_segments)[0], ZSMMG(shared_segments_count), S_H(segment_type_size)());

	*p_tmp_shared_globals = tmp_shared_globals;
	smm_shared_globals = p_tmp_shared_globals;

	free(ZSMMG(shared_segments));
	ZSMMG(shared_segments) = tmp_shared_segments;

	ZSMMG(shared_memory_state).positions = (int *)zend_shared_alloc(sizeof(int) * ZSMMG(shared_segments_count));
	if (!ZSMMG(shared_memory_state).positions) {
		zend_accel_error(ACCEL_LOG_FATAL, "Insufficient shared memory!");
		return ALLOC_FAILURE;;
	}

	ZCG(locked) = 0;

	return res;
}

void zend_shared_alloc_shutdown(void)
{
	zend_shared_segment **tmp_shared_segments;
	size_t shared_segments_array_size;
	zend_smm_shared_globals tmp_shared_globals;
	int i;

	tmp_shared_globals = *smm_shared_globals;
	smm_shared_globals = &tmp_shared_globals;
	shared_segments_array_size = ZSMMG(shared_segments_count) * (S_H(segment_type_size)() + sizeof(void *));
	tmp_shared_segments = emalloc(shared_segments_array_size);
	copy_shared_segments(tmp_shared_segments, ZSMMG(shared_segments)[0], ZSMMG(shared_segments_count), S_H(segment_type_size)());
	ZSMMG(shared_segments) = tmp_shared_segments;

	for (i = 0; i < ZSMMG(shared_segments_count); i++) {
		S_H(detach_segment)(ZSMMG(shared_segments)[i]);
	}
	efree(ZSMMG(shared_segments));
	ZSMMG(shared_segments) = NULL;
	g_shared_alloc_handler = NULL;
#ifndef ZEND_WIN32
	close(lock_file);
#endif
}

static size_t zend_shared_alloc_get_largest_free_block(void)
{
	int i;
	size_t largest_block_size = 0;

	for (i = 0; i < ZSMMG(shared_segments_count); i++) {
		size_t block_size = ZSMMG(shared_segments)[i]->size - ZSMMG(shared_segments)[i]->pos;

		if (block_size>largest_block_size) {
			largest_block_size = block_size;
		}
	}
	return largest_block_size;
}

#define MIN_FREE_MEMORY 64*1024

#define SHARED_ALLOC_FAILED() do {		\
		zend_accel_error(ACCEL_LOG_WARNING, "Not enough free shared space to allocate %pd bytes (%pd bytes free)", (zend_long)size, (zend_long)ZSMMG(shared_free)); \
		if (zend_shared_alloc_get_largest_free_block() < MIN_FREE_MEMORY) { \
			ZSMMG(memory_exhausted) = 1; \
		} \
	} while (0)

void *zend_shared_alloc(size_t size)
{
	int i;
	unsigned int block_size = ZEND_ALIGNED_SIZE(size);

#if 1
	if (!ZCG(locked)) {
		zend_accel_error(ACCEL_LOG_ERROR, "Shared memory lock not obtained");
	}
#endif
	if (block_size > ZSMMG(shared_free)) { /* No hope to find a big-enough block */
		SHARED_ALLOC_FAILED();
		return NULL;
	}
	for (i = 0; i < ZSMMG(shared_segments_count); i++) {
		if (ZSMMG(shared_segments)[i]->size - ZSMMG(shared_segments)[i]->pos >= block_size) { /* found a valid block */
			void *retval = (void *) (((char *) ZSMMG(shared_segments)[i]->p) + ZSMMG(shared_segments)[i]->pos);

			ZSMMG(shared_segments)[i]->pos += block_size;
			ZSMMG(shared_free) -= block_size;
			memset(retval, 0, block_size);
			ZEND_ASSERT(((zend_uintptr_t)retval & 0x7) == 0); /* should be 8 byte aligned */
			return retval;
		}
	}
	SHARED_ALLOC_FAILED();
	return NULL;
}

int zend_shared_memdup_size(void *source, size_t size)
{
	void *old_p;

	if ((old_p = zend_hash_index_find_ptr(&ZCG(xlat_table), (zend_ulong)source)) != NULL) {
		/* we already duplicated this pointer */
		return 0;
	}
	zend_shared_alloc_register_xlat_entry(source, source);
	return ZEND_ALIGNED_SIZE(size);
}

void *_zend_shared_memdup(void *source, size_t size, zend_bool free_source)
{
	void *old_p, *retval;

	if ((old_p = zend_hash_index_find_ptr(&ZCG(xlat_table), (zend_ulong)source)) != NULL) {
		/* we already duplicated this pointer */
		return old_p;
	}
	retval = ZCG(mem);
	ZCG(mem) = (void*)(((char*)ZCG(mem)) + ZEND_ALIGNED_SIZE(size));
	memcpy(retval, source, size);
	zend_shared_alloc_register_xlat_entry(source, retval);
	if (free_source) {
		efree(source);
	}
	return retval;
}

void zend_shared_alloc_safe_unlock(void)
{
	if (ZCG(locked)) {
		zend_shared_alloc_unlock();
	}
}

#ifndef ZEND_WIN32
/* name l_type l_whence l_start l_len */
static FLOCK_STRUCTURE(mem_write_lock, F_WRLCK, SEEK_SET, 0, 1);
static FLOCK_STRUCTURE(mem_write_unlock, F_UNLCK, SEEK_SET, 0, 1);
#endif

void zend_shared_alloc_lock(void)
{
#ifndef ZEND_WIN32

#ifdef ZTS
	tsrm_mutex_lock(zts_lock);
#endif

#if 0
	/* this will happen once per process, and will un-globalize mem_write_lock */
	if (mem_write_lock.l_pid == -1) {
		mem_write_lock.l_pid = getpid();
	}
#endif

	while (1) {
		if (fcntl(lock_file, F_SETLKW, &mem_write_lock) == -1) {
			if (errno == EINTR) {
				continue;
			}
			zend_accel_error(ACCEL_LOG_ERROR, "Cannot create lock - %s (%d)", strerror(errno), errno);
		}
		break;
	}
#else
	zend_shared_alloc_lock_win32();
#endif

	ZCG(locked) = 1;
}

void zend_shared_alloc_unlock(void)
{
	ZCG(locked) = 0;

#ifndef ZEND_WIN32
	if (fcntl(lock_file, F_SETLK, &mem_write_unlock) == -1) {
		zend_accel_error(ACCEL_LOG_ERROR, "Cannot remove lock - %s (%d)", strerror(errno), errno);
	}
#ifdef ZTS
	tsrm_mutex_unlock(zts_lock);
#endif
#else
	zend_shared_alloc_unlock_win32();
#endif
}

void zend_shared_alloc_init_xlat_table(void)
{

	/* Prepare translation table
	 *
	 * Make it persistent so that it uses malloc() and allocated blocks
	 * won't be taken from space which is freed by efree in memdup.
	 * Otherwise it leads to false matches in memdup check.
	 */
	zend_hash_init(&ZCG(xlat_table), 128, NULL, NULL, 1);
}

void zend_shared_alloc_destroy_xlat_table(void)
{
	/* Destroy translation table */
	zend_hash_destroy(&ZCG(xlat_table));
}

void zend_shared_alloc_clear_xlat_table(void)
{
	zend_hash_clean(&ZCG(xlat_table));
}

void zend_shared_alloc_register_xlat_entry(const void *old, const void *new)
{
	zend_hash_index_add_new_ptr(&ZCG(xlat_table), (zend_ulong)old, (void*)new);
}

void *zend_shared_alloc_get_xlat_entry(const void *old)
{
	void *retval;

	if ((retval = zend_hash_index_find_ptr(&ZCG(xlat_table), (zend_ulong)old)) == NULL) {
		return NULL;
	}
	return retval;
}

size_t zend_shared_alloc_get_free_memory(void)
{
	return ZSMMG(shared_free);
}

void zend_shared_alloc_save_state(void)
{
	int i;

	for (i = 0; i < ZSMMG(shared_segments_count); i++) {
		ZSMMG(shared_memory_state).positions[i] = ZSMMG(shared_segments)[i]->pos;
	}
	ZSMMG(shared_memory_state).shared_free = ZSMMG(shared_free);
}

void zend_shared_alloc_restore_state(void)
{
	int i;

	for (i = 0; i < ZSMMG(shared_segments_count); i++) {
		ZSMMG(shared_segments)[i]->pos = ZSMMG(shared_memory_state).positions[i];
	}
	ZSMMG(shared_free) = ZSMMG(shared_memory_state).shared_free;
	ZSMMG(memory_exhausted) = 0;
	ZSMMG(wasted_shared_memory) = 0;
}

const char *zend_accel_get_shared_model(void)
{
	return g_shared_model;
}

void zend_accel_shared_protect(int mode)
{
#ifdef HAVE_MPROTECT
	int i;

	if (!smm_shared_globals) {
		return;
	}

	if (mode) {
		mode = PROT_READ;
	} else {
		mode = PROT_READ|PROT_WRITE;
	}

	for (i = 0; i < ZSMMG(shared_segments_count); i++) {
		mprotect(ZSMMG(shared_segments)[i]->p, ZSMMG(shared_segments)[i]->size, mode);
	}
#endif
}

int zend_accel_in_shm(void *ptr)
{
	int i;

	if (!smm_shared_globals) {
		return 0;
	}

	for (i = 0; i < ZSMMG(shared_segments_count); i++) {
		if ((char*)ptr >= (char*)ZSMMG(shared_segments)[i]->p &&
		    (char*)ptr < (char*)ZSMMG(shared_segments)[i]->p + ZSMMG(shared_segments)[i]->size) {
			return 1;
		}
	}
	return 0;
}
