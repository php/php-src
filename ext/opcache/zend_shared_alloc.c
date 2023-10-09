/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#if defined(__linux__) && defined(HAVE_MEMFD_CREATE)
# ifndef _GNU_SOURCE
#  define _GNU_SOURCE
# endif
# include <sys/mman.h>
#endif

#include <errno.h>
#include "ZendAccelerator.h"
#include "zend_shared_alloc.h"
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <fcntl.h>
#ifndef ZEND_WIN32
# include <sys/types.h>
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
int lock_file = -1;
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

#if defined(__linux__) && defined(HAVE_MEMFD_CREATE)
	/* on Linux, we can use a memfd instead of a "real" file, so
	 * we can do this without a writable filesystem and without
	 * needing to clean up */
	/* note: FreeBSD has memfd_create(), too, but fcntl(F_SETLKW)
	 * on it fails with EBADF, therefore we use this only on
	 * Linux */
	lock_file = memfd_create("opcache_lock", MFD_CLOEXEC);
	if (lock_file >= 0)
		return;
#endif

#ifdef O_TMPFILE
	lock_file = open(lockfile_path, O_RDWR | O_TMPFILE | O_EXCL | O_CLOEXEC, 0666);
	/* lack of O_TMPFILE support results in many possible errors
	 * use it only when open returns a non-negative integer */
	if (lock_file >= 0) {
		return;
	}
#endif

	snprintf(lockfile_name, sizeof(lockfile_name), "%s/%sXXXXXX", lockfile_path, SEM_FILENAME_PREFIX);
	lock_file = mkstemp(lockfile_name);
	if (lock_file == -1) {
		zend_accel_error_noreturn(ACCEL_LOG_FATAL, "Unable to create opcache lock file in %s: %s (%d)", lockfile_path, strerror(errno), errno);
	}

	fchmod(lock_file, 0666);

	val = fcntl(lock_file, F_GETFD, 0);
	val |= FD_CLOEXEC;
	fcntl(lock_file, F_SETFD, val);

	unlink(lockfile_name);
}
#endif

static void no_memory_bailout(size_t allocate_size, const char *error)
{
	zend_accel_error_noreturn(ACCEL_LOG_FATAL, "Unable to allocate shared memory segment of %zu bytes: %s: %s (%d)", allocate_size, error?error:"unknown", strerror(errno), errno );
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

static int zend_shared_alloc_try(const zend_shared_memory_handler_entry *he, size_t requested_size, zend_shared_segment ***shared_segments_p, int *shared_segments_count, const char **error_in)
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

int zend_shared_alloc_startup(size_t requested_size, size_t reserved_size)
{
	zend_shared_segment **tmp_shared_segments;
	size_t shared_segments_array_size;
	zend_smm_shared_globals tmp_shared_globals, *p_tmp_shared_globals;
	const char *error_in = NULL;
	const zend_shared_memory_handler_entry *he;
	int res = ALLOC_FAILURE;
	int i;

	/* shared_free must be valid before we call zend_shared_alloc()
	 * - make it temporarily point to a local variable
	 */
	smm_shared_globals = &tmp_shared_globals;
	ZSMMG(shared_free) = requested_size - reserved_size; /* goes to tmp_shared_globals.shared_free */

#ifndef ZEND_WIN32
	zend_shared_alloc_create_lock(ZCG(accel_directives).lockfile_path);
#else
	zend_shared_alloc_create_lock();
#endif

	if (ZCG(accel_directives).memory_model && ZCG(accel_directives).memory_model[0]) {
		const char *model = ZCG(accel_directives).memory_model;
		/* "cgi" is really "shm"... */
		if (strncmp(ZCG(accel_directives).memory_model, "cgi", sizeof("cgi")) == 0) {
			model = "shm";
		}

		for (he = handler_table; he->name; he++) {
			if (strcmp(model, he->name) == 0) {
				res = zend_shared_alloc_try(he, requested_size, &ZSMMG(shared_segments), &ZSMMG(shared_segments_count), &error_in);
				if (res) {
					/* this model works! */
					break;
				}
			}
		}
	}

	if (res == FAILED_REATTACHED) {
		smm_shared_globals = NULL;
		return res;
	}
#if ENABLE_FILE_CACHE_FALLBACK
	if (ALLOC_FALLBACK == res) {
		smm_shared_globals = NULL;
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
		smm_shared_globals = NULL;
		return ALLOC_FALLBACK;
	}
#endif

	for (i = 0; i < ZSMMG(shared_segments_count); i++) {
		ZSMMG(shared_segments)[i]->end = ZSMMG(shared_segments)[i]->size;
	}

	shared_segments_array_size = ZSMMG(shared_segments_count) * S_H(segment_type_size)();

	/* move shared_segments and shared_free to shared memory */
	ZCG(locked) = 1; /* no need to perform a real lock at this point */

	p_tmp_shared_globals = (zend_smm_shared_globals *) zend_shared_alloc(sizeof(zend_smm_shared_globals));
	if (!p_tmp_shared_globals) {
		zend_accel_error_noreturn(ACCEL_LOG_FATAL, "Insufficient shared memory!");
		return ALLOC_FAILURE;
	}
	memset(p_tmp_shared_globals, 0, sizeof(zend_smm_shared_globals));

	tmp_shared_segments = zend_shared_alloc(shared_segments_array_size + ZSMMG(shared_segments_count) * sizeof(void *));
	if (!tmp_shared_segments) {
		zend_accel_error_noreturn(ACCEL_LOG_FATAL, "Insufficient shared memory!");
		return ALLOC_FAILURE;
	}

	copy_shared_segments(tmp_shared_segments, ZSMMG(shared_segments)[0], ZSMMG(shared_segments_count), S_H(segment_type_size)());

	*p_tmp_shared_globals = tmp_shared_globals;
	smm_shared_globals = p_tmp_shared_globals;

	free(ZSMMG(shared_segments));
	ZSMMG(shared_segments) = tmp_shared_segments;

	ZSMMG(shared_memory_state).positions = (int *)zend_shared_alloc(sizeof(int) * ZSMMG(shared_segments_count));
	if (!ZSMMG(shared_memory_state).positions) {
		zend_accel_error_noreturn(ACCEL_LOG_FATAL, "Insufficient shared memory!");
		return ALLOC_FAILURE;
	}

	if (reserved_size) {
		i = ZSMMG(shared_segments_count) - 1;
		if (ZSMMG(shared_segments)[i]->size - ZSMMG(shared_segments)[i]->pos >= reserved_size) {
			ZSMMG(shared_segments)[i]->end = ZSMMG(shared_segments)[i]->size - reserved_size;
			ZSMMG(reserved) = (char*)ZSMMG(shared_segments)[i]->p + ZSMMG(shared_segments)[i]->end;
			ZSMMG(reserved_size) = reserved_size;
		} else {
			zend_accel_error_noreturn(ACCEL_LOG_FATAL, "Insufficient shared memory!");
			return ALLOC_FAILURE;
		}
	}

	ZCG(locked) = 0;

	return res;
}

void zend_shared_alloc_shutdown(void)
{
	zend_shared_segment **tmp_shared_segments;
	zend_shared_segment *shared_segments_buf[16];
	size_t shared_segments_array_size;
	zend_smm_shared_globals tmp_shared_globals;
	int i;

	tmp_shared_globals = *smm_shared_globals;
	smm_shared_globals = &tmp_shared_globals;
	shared_segments_array_size = ZSMMG(shared_segments_count) * (S_H(segment_type_size)() + sizeof(void *));
	if (shared_segments_array_size > 16) {
		tmp_shared_segments = malloc(shared_segments_array_size);
	} else {
		tmp_shared_segments = shared_segments_buf;
	}
	copy_shared_segments(tmp_shared_segments, ZSMMG(shared_segments)[0], ZSMMG(shared_segments_count), S_H(segment_type_size)());
	ZSMMG(shared_segments) = tmp_shared_segments;

	for (i = 0; i < ZSMMG(shared_segments_count); i++) {
		S_H(detach_segment)(ZSMMG(shared_segments)[i]);
	}
	if (shared_segments_array_size > 16) {
		free(ZSMMG(shared_segments));
	}
	ZSMMG(shared_segments) = NULL;
	g_shared_alloc_handler = NULL;
#ifndef ZEND_WIN32
	close(lock_file);

# ifdef ZTS
	tsrm_mutex_free(zts_lock);
# endif
#endif
}

static size_t zend_shared_alloc_get_largest_free_block(void)
{
	int i;
	size_t largest_block_size = 0;

	for (i = 0; i < ZSMMG(shared_segments_count); i++) {
		size_t block_size = ZSMMG(shared_segments)[i]->end - ZSMMG(shared_segments)[i]->pos;

		if (block_size>largest_block_size) {
			largest_block_size = block_size;
		}
	}
	return largest_block_size;
}

#define MIN_FREE_MEMORY 64*1024

#define SHARED_ALLOC_FAILED() do {		\
		zend_accel_error(ACCEL_LOG_WARNING, "Not enough free shared space to allocate %zu bytes (%zu bytes free)", size, ZSMMG(shared_free)); \
		if (zend_shared_alloc_get_largest_free_block() < MIN_FREE_MEMORY) { \
			ZSMMG(memory_exhausted) = 1; \
		} \
	} while (0)

void *zend_shared_alloc(size_t size)
{
	ZEND_ASSERT(ZCG(locked));

	int i;
	unsigned int block_size = ZEND_ALIGNED_SIZE(size);

	if (UNEXPECTED(block_size < size)) {
		zend_accel_error_noreturn(ACCEL_LOG_ERROR, "Possible integer overflow in shared memory allocation (%zu + %zu)", size, PLATFORM_ALIGNMENT);
	}

	if (block_size > ZSMMG(shared_free)) { /* No hope to find a big-enough block */
		SHARED_ALLOC_FAILED();
		return NULL;
	}
	for (i = 0; i < ZSMMG(shared_segments_count); i++) {
		if (ZSMMG(shared_segments)[i]->end - ZSMMG(shared_segments)[i]->pos >= block_size) { /* found a valid block */
			void *retval = (void *) (((char *) ZSMMG(shared_segments)[i]->p) + ZSMMG(shared_segments)[i]->pos);

			ZSMMG(shared_segments)[i]->pos += block_size;
			ZSMMG(shared_free) -= block_size;
			ZEND_ASSERT(((uintptr_t)retval & 0x7) == 0); /* should be 8 byte aligned */
			return retval;
		}
	}
	SHARED_ALLOC_FAILED();
	return NULL;
}

static zend_always_inline zend_ulong zend_rotr3(zend_ulong key)
{
	return (key >> 3) | (key << ((sizeof(key) * 8) - 3));
}

int zend_shared_memdup_size(void *source, size_t size)
{
	void *old_p;
	zend_ulong key = (zend_ulong)source;

	key = zend_rotr3(key);
	if ((old_p = zend_hash_index_find_ptr(&ZCG(xlat_table), key)) != NULL) {
		/* we already duplicated this pointer */
		return 0;
	}
	zend_hash_index_add_new_ptr(&ZCG(xlat_table), key, source);
	return ZEND_ALIGNED_SIZE(size);
}

static zend_always_inline void *_zend_shared_memdup(void *source, size_t size, bool get_xlat, bool set_xlat, bool free_source)
{
	void *old_p, *retval;
	zend_ulong key;

	if (get_xlat) {
		key = (zend_ulong)source;
		key = zend_rotr3(key);
		if ((old_p = zend_hash_index_find_ptr(&ZCG(xlat_table), key)) != NULL) {
			/* we already duplicated this pointer */
			return old_p;
		}
	}
	retval = ZCG(mem);
	ZCG(mem) = (void*)(((char*)ZCG(mem)) + ZEND_ALIGNED_SIZE(size));
	memcpy(retval, source, size);
	if (set_xlat) {
		if (!get_xlat) {
			key = (zend_ulong)source;
			key = zend_rotr3(key);
		}
		zend_hash_index_add_new_ptr(&ZCG(xlat_table), key, retval);
	}
	if (free_source) {
		efree(source);
	}
	return retval;
}

void *zend_shared_memdup_get_put_free(void *source, size_t size)
{
	return _zend_shared_memdup(source, size, true, true, true);
}

void *zend_shared_memdup_put_free(void *source, size_t size)
{
	return _zend_shared_memdup(source, size, false, true, true);
}

void *zend_shared_memdup_free(void *source, size_t size)
{
	return _zend_shared_memdup(source, size, false, false, true);
}

void *zend_shared_memdup_get_put(void *source, size_t size)
{
	return _zend_shared_memdup(source, size, true, true, false);
}

void *zend_shared_memdup_put(void *source, size_t size)
{
	return _zend_shared_memdup(source, size, false, true, false);
}

void *zend_shared_memdup(void *source, size_t size)
{
	return _zend_shared_memdup(source, size, false, false, false);
}

void zend_shared_alloc_safe_unlock(void)
{
	if (ZCG(locked)) {
		zend_shared_alloc_unlock();
	}
}

void zend_shared_alloc_lock(void)
{
	ZEND_ASSERT(!ZCG(locked));

#ifndef ZEND_WIN32
	struct flock mem_write_lock;

	mem_write_lock.l_type = F_WRLCK;
	mem_write_lock.l_whence = SEEK_SET;
	mem_write_lock.l_start = 0;
	mem_write_lock.l_len = 1;

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
			zend_accel_error_noreturn(ACCEL_LOG_ERROR, "Cannot create lock - %s (%d)", strerror(errno), errno);
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
	ZEND_ASSERT(ZCG(locked));

#ifndef ZEND_WIN32
	struct flock mem_write_unlock;

	mem_write_unlock.l_type = F_UNLCK;
	mem_write_unlock.l_whence = SEEK_SET;
	mem_write_unlock.l_start = 0;
	mem_write_unlock.l_len = 1;
#endif

	ZCG(locked) = 0;

#ifndef ZEND_WIN32
	if (fcntl(lock_file, F_SETLK, &mem_write_unlock) == -1) {
		zend_accel_error_noreturn(ACCEL_LOG_ERROR, "Cannot remove lock - %s (%d)", strerror(errno), errno);
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
	/* Prepare translation table */
	zend_hash_init(&ZCG(xlat_table), 128, NULL, NULL, 0);
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

uint32_t zend_shared_alloc_checkpoint_xlat_table(void)
{
	return ZCG(xlat_table).nNumUsed;
}

void zend_shared_alloc_restore_xlat_table(uint32_t checkpoint)
{
	zend_hash_discard(&ZCG(xlat_table), checkpoint);
}

void zend_shared_alloc_register_xlat_entry(const void *key_pointer, const void *value)
{
	zend_ulong key = (zend_ulong)key_pointer;

	key = zend_rotr3(key);
	zend_hash_index_add_new_ptr(&ZCG(xlat_table), key, (void*)value);
}

void *zend_shared_alloc_get_xlat_entry(const void *key_pointer)
{
	void *retval;
	zend_ulong key = (zend_ulong)key_pointer;

	key = zend_rotr3(key);
	if ((retval = zend_hash_index_find_ptr(&ZCG(xlat_table), key)) == NULL) {
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

void zend_accel_shared_protect(bool protected)
{
#ifdef HAVE_MPROTECT
	int i;

	if (!smm_shared_globals) {
		return;
	}

	const int mode = protected ? PROT_READ : PROT_READ|PROT_WRITE;

	for (i = 0; i < ZSMMG(shared_segments_count); i++) {
		mprotect(ZSMMG(shared_segments)[i]->p, ZSMMG(shared_segments)[i]->end, mode);
	}
#elif defined(ZEND_WIN32)
	int i;

	if (!smm_shared_globals) {
		return;
	}

	const int mode = protected ? PAGE_READONLY : PAGE_READWRITE;

	for (i = 0; i < ZSMMG(shared_segments_count); i++) {
		DWORD oldProtect;
		if (!VirtualProtect(ZSMMG(shared_segments)[i]->p, ZSMMG(shared_segments)[i]->end, mode, &oldProtect)) {
			zend_accel_error_noreturn(ACCEL_LOG_ERROR, "Failed to protect memory");
		}
	}
#endif
}

bool zend_accel_in_shm(void *ptr)
{
	int i;

	if (!smm_shared_globals) {
		return false;
	}

	for (i = 0; i < ZSMMG(shared_segments_count); i++) {
		if ((char*)ptr >= (char*)ZSMMG(shared_segments)[i]->p &&
		    (char*)ptr < (char*)ZSMMG(shared_segments)[i]->p + ZSMMG(shared_segments)[i]->end) {
			return true;
		}
	}
	return false;
}
