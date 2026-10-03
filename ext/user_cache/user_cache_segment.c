/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Go Kudo <zeriyoshi@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#include "user_cache_storage.h"

#include "ext/hash/php_hash.h"
#include "ext/hash/php_hash_sha.h"
#include "ext/random/php_random_csprng.h"

#ifdef UCACHE_HAVE_BOUNDARY_SHM
# define UCACHE_BOUNDARY_DIR_PREFIX			".PhpUserCacheBnd."
# define UCACHE_BOUNDARY_SALT_NAME			"salt"
# define UCACHE_BOUNDARY_SHM_PREFIX			"/PUC."
# define UCACHE_BOUNDARY_LOCK_SUFFIX		".lock"
# define UCACHE_BOUNDARY_TAG_PREFIX			"PhpUserCache.boundary-tag|"
# define UCACHE_BOUNDARY_NAME_SIZE			32U
# define UCACHE_BOUNDARY_ATTACH_BYTE		1
# define UCACHE_BOUNDARY_ATTACH_ATTEMPTS	8U
# define UCACHE_BOUNDARY_DEADLOCK_RETRY_US	1000U
# define UCACHE_BOUNDARY_IDLE_SEG_MAX		UCACHE_MAX_BOUNDARY_PARTITIONS
#endif

#ifdef ZEND_WIN32
# define UCACHE_WIN32_SALT_SIZE				32U
# define UCACHE_WIN32_SALT_READ_ATTEMPTS	1000U
# define UCACHE_WIN32_PRIVATE_DIR_PREFIX	L"PhpUserCache."
# define UCACHE_WIN32_SALT_NAME			L"salt"
# define UCACHE_WIN32_OWNER_ONLY_SDDL		L"D:P(A;;GA;;;%ls)"
#endif

#ifdef ZEND_WIN32
typedef struct {
	union {
		TOKEN_USER info;
		uint8_t bytes[sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE];
	} user;
	union {
		TOKEN_OWNER info;
		uint8_t bytes[sizeof(TOKEN_OWNER) + SECURITY_MAX_SID_SIZE];
	} owner;
} ucache_win32_token_sids;
#endif

#ifdef UCACHE_HAVE_BOUNDARY_SHM
typedef struct {
	uint8_t boundary_tag[32];
	uint64_t last_used_us;
	uint64_t superseded_at_us;
} ucache_boundary_lock_record;

typedef struct {
	char id[UCACHE_BOUNDARY_ID_LEN + 1];
	uint64_t last_used_us;
} ucache_boundary_idle_seg;
#endif

#ifdef UCACHE_HAVE_BOUNDARY_SHM
static bool ucache_boundary_dir_failure_logged = false;
static atomic_bool ucache_boundary_dir_failure_pending = false;
static char ucache_boundary_dir_failure_msg[MAXPATHLEN + 192];
static ucache_boundary_seg *ucache_boundary_segs = NULL;
#endif

#ifdef ZEND_WIN32
static uint8_t ucache_win32_salt[UCACHE_WIN32_SALT_SIZE];
static bool ucache_win32_salt_loaded = false;
#endif

#ifdef UCACHE_USE_MMAP
static int ucache_wrap_mapped_seg(
		void *mapping,
		size_t requested_size,
		ucache_shm_seg **shared_seg_p,
		const char **err_in)
{
	ucache_shm_seg *seg;

	seg = (ucache_shm_seg *) calloc(1, sizeof(*seg));
	if (seg == NULL) {
		munmap(mapping, requested_size);

		*err_in = "calloc";

		return UCACHE_ALLOC_FAILURE;
	}

	seg->p = mapping;
	seg->size = requested_size;
	seg->zero_filled = true;
	*shared_seg_p = seg;

	return UCACHE_ALLOC_SUCCESS;
}

static void ucache_munmap_detach_seg(ucache_shm_seg *shared_seg)
{
	munmap(shared_seg->p, shared_seg->size);
}

static int ucache_mmap_create_seg(
		size_t requested_size,
		ucache_shm_seg **shared_seg_p,
		const char **err_in)
{
	void *mapping;

	mapping = mmap(NULL, requested_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (mapping == MAP_FAILED) {
		*err_in = "mmap";

		return UCACHE_ALLOC_FAILURE;
	}

	return ucache_wrap_mapped_seg(mapping, requested_size, shared_seg_p, err_in);
}
#endif /* UCACHE_USE_MMAP */

#if defined(UCACHE_HAVE_BOUNDARY_SHM) || defined(ZEND_WIN32)
static void ucache_sha256_update_build_identity(PHP_SHA256_CTX *sha_ctx)
{
	const uint32_t *node_sizes;
	const size_t layout[] = {
		sizeof(ucache_hdr),
		offsetof(ucache_hdr, pool_bucket_heads),
		offsetof(ucache_hdr, boundary_identity_digest),
		offsetof(ucache_hdr, graph_pin_slots),
		offsetof(ucache_hdr, reader_slots),
		offsetof(ucache_hdr, commit_failure_count),
		offsetof(ucache_hdr, stale_tail_end),
		sizeof(ucache_entry),
		sizeof(ucache_pool_links),
		sizeof(ucache_entry_lock_record),
		UCACHE_OFFSET_SHIFT,
	};
	uint32_t node_count;

	node_sizes = ucache_sgraph_node_sizes(&node_count);

	PHP_SHA256Update(sha_ctx, (const uint8_t *) zend_system_id, sizeof(zend_system_id));
	PHP_SHA256Update(sha_ctx, (const uint8_t *) layout, sizeof(layout));
	PHP_SHA256Update(sha_ctx, (const uint8_t *) node_sizes, node_count * sizeof(*node_sizes));

	if (UCACHE_DEBUG_FAULT("SIMULATE_OTHER_BUILD")) {
		PHP_SHA256Update(sha_ctx, (const uint8_t *) "other-build", sizeof("other-build") - 1);
	}
}
#endif

#ifdef UCACHE_HAVE_BOUNDARY_SHM
static void ucache_shared_boundary_digest_to_identity(
		const uint8_t digest[32],
		uint64_t *identity_high,
		uint32_t *identity_low)
{
	*identity_high =
		((uint64_t) digest[0] << 56) |
		((uint64_t) digest[1] << 48) |
		((uint64_t) digest[2] << 40) |
		((uint64_t) digest[3] << 32) |
		((uint64_t) digest[4] << 24) |
		((uint64_t) digest[5] << 16) |
		((uint64_t) digest[6] << 8) |
		(uint64_t) digest[7]
	;
	*identity_low =
		((uint32_t) digest[8] << 24) |
		((uint32_t) digest[9] << 16) |
		((uint32_t) digest[10] << 8) |
		(uint32_t) digest[11]
	;
}

static void ucache_shared_boundary_identity(
		ucache_ctx *ctx,
		size_t requested_size,
		uint64_t *identity_high,
		uint32_t *identity_low)
{
	ucache_shared_boundary_digest_to_identity(
		ucache_shared_boundary_digest_memo(ctx, requested_size),
		identity_high,
		identity_low
	);
}

static void ucache_shared_boundary_format_id(char id[UCACHE_BOUNDARY_ID_LEN + 1], size_t requested_size)
{
	uint64_t identity_high;
	uint32_t identity_low;

	ucache_shared_boundary_identity(
		ucache_active_ctx(),
		requested_size,
		&identity_high,
		&identity_low
	);

	snprintf(id, UCACHE_BOUNDARY_ID_LEN + 1, "%016" PRIx64 "%08" PRIx32, identity_high, identity_low);
}

static void ucache_shared_boundary_shm_name(char name[UCACHE_BOUNDARY_NAME_SIZE], const char *id)
{
	snprintf(name, UCACHE_BOUNDARY_NAME_SIZE, UCACHE_BOUNDARY_SHM_PREFIX "%s", id);
}

static void ucache_shared_boundary_lock_name(char name[UCACHE_BOUNDARY_NAME_SIZE], const char *id)
{
	snprintf(name, UCACHE_BOUNDARY_NAME_SIZE, "%s" UCACHE_BOUNDARY_LOCK_SUFFIX, id);
}

static bool ucache_shared_boundary_lock_name_to_id(const char *name, char id[UCACHE_BOUNDARY_ID_LEN + 1])
{
	uint32_t i;

	if (strlen(name) != UCACHE_BOUNDARY_ID_LEN + sizeof(UCACHE_BOUNDARY_LOCK_SUFFIX) - 1 ||
		strcmp(name + UCACHE_BOUNDARY_ID_LEN, UCACHE_BOUNDARY_LOCK_SUFFIX) != 0
	) {
		return false;
	}

	for (i = 0; i < UCACHE_BOUNDARY_ID_LEN; i++) {
		if (!((name[i] >= '0' && name[i] <= '9') || (name[i] >= 'a' && name[i] <= 'f'))) {
			return false;
		}

		id[i] = name[i];
	}

	id[UCACHE_BOUNDARY_ID_LEN] = '\0';

	return true;
}

static uint64_t ucache_shared_boundary_now_us(void)
{
	struct timeval tv;

	if (gettimeofday(&tv, NULL) != 0) {
		return 0;
	}

	return (uint64_t) tv.tv_sec * UINT64_C(1000000) + (uint64_t) tv.tv_usec;
}

static void ucache_shared_boundary_log_dir_failure(const char *dir_path, const char *reason)
{
	if (ucache_boundary_dir_failure_logged) {
		return;
	}

	ucache_boundary_dir_failure_logged = true;

	snprintf(
		ucache_boundary_dir_failure_msg,
		sizeof(ucache_boundary_dir_failure_msg),
		"UserCache: boundary directory %s is unusable (%s); it must be a directory owned by uid %lu "
		"with mode 0700 (see user_cache.lockfile_path)",
		dir_path,
		reason,
		(unsigned long) geteuid()
	);

	atomic_store(&ucache_boundary_dir_failure_pending, true);
}

static bool ucache_shared_boundary_fd_is_trusted(int fd, struct stat *st)
{
	if (fstat(fd, st) != 0) {
		return false;
	}

	return st->st_uid == geteuid() && (st->st_mode & 0077) == 0;
}

static int ucache_shared_boundary_open_private_dir(char *dir_path, size_t dir_path_size, const char **err_in)
{
	struct stat st;
	int fd, saved_errno, len;

	len = snprintf(
		dir_path,
		dir_path_size,
		"%s/" UCACHE_BOUNDARY_DIR_PREFIX "%lu",
		UC_G(lockfile_path),
		(unsigned long) geteuid()
	);
	if (len < 0 || (size_t) len >= dir_path_size) {
		errno = ENAMETOOLONG;

		*err_in = "lockfile_path";

		return -1;
	}

	if (mkdir(dir_path, 0700) != 0 && errno != EEXIST) {
		saved_errno = errno;

		ucache_shared_boundary_log_dir_failure(dir_path, strerror(saved_errno));

		errno = saved_errno;

		*err_in = "mkdir";

		return -1;
	}

	fd = open(dir_path, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
	if (fd < 0) {
		saved_errno = errno;

		ucache_shared_boundary_log_dir_failure(dir_path, strerror(saved_errno));

		errno = saved_errno;

		*err_in = "open directory";

		return -1;
	}

	if (!ucache_shared_boundary_fd_is_trusted(fd, &st) || !S_ISDIR(st.st_mode)) {
		close(fd);

		ucache_shared_boundary_log_dir_failure(dir_path, "not a private directory owned by this uid");

		errno = EACCES;

		*err_in = "directory ownership";

		return -1;
	}

	return fd;
}

static bool ucache_shared_boundary_read_salt(
		int dir_fd,
		const char *dir_path,
		uint8_t *salt,
		bool *missing,
		const char **err_in)
{
	struct stat st;
	size_t got = 0;
	ssize_t n;
	int fd;

	*missing = false;

	fd = openat(dir_fd, UCACHE_BOUNDARY_SALT_NAME, O_RDONLY | O_NOFOLLOW | O_CLOEXEC);
	if (fd < 0) {
		*missing = errno == ENOENT;
		*err_in = "open salt";

		return false;
	}

	if (!ucache_shared_boundary_fd_is_trusted(fd, &st) ||
		!S_ISREG(st.st_mode) ||
		st.st_size != UCACHE_BOUNDARY_SALT_SIZE
	) {
		close(fd);
		ucache_shared_boundary_log_dir_failure(
			dir_path,
			"its " UCACHE_BOUNDARY_SALT_NAME " file is not a private regular file of the expected size owned by this uid"
		);

		errno = EACCES;

		*err_in = "salt ownership";

		return false;
	}

	while (got < UCACHE_BOUNDARY_SALT_SIZE) {
		n = read(fd, salt + got, UCACHE_BOUNDARY_SALT_SIZE - got);
		if (n < 0 && errno == EINTR) {
			continue;
		}

		if (n <= 0) {
			close(fd);
			if (n == 0) {
				errno = EIO;
			}

			*err_in = "read salt";

			return false;
		}

		got += (size_t) n;
	}

	close(fd);

	return true;
}

static bool ucache_shared_boundary_create_salt(int dir_fd, const char **err_in)
{
	uint8_t salt[UCACHE_BOUNDARY_SALT_SIZE];
	size_t written = 0;
	ssize_t n;
	char tmp_name[64];
	int fd, saved_errno;

	snprintf(tmp_name, sizeof(tmp_name), UCACHE_BOUNDARY_SALT_NAME ".%lu", (unsigned long) getpid());

	fd = openat(dir_fd, tmp_name, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0600);
	if (fd < 0 && errno == EEXIST) {
		unlinkat(dir_fd, tmp_name, 0);
		fd = openat(dir_fd, tmp_name, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0600);
	}

	if (fd < 0) {
		*err_in = "create salt";

		return false;
	}

	if (php_random_bytes_silent(salt, sizeof(salt)) == FAILURE) {
		close(fd);
		unlinkat(dir_fd, tmp_name, 0);

		errno = EIO;

		*err_in = "php_random_bytes";

		return false;
	}

	while (written < sizeof(salt)) {
		n = write(fd, salt + written, sizeof(salt) - written);
		if (n < 0 && errno == EINTR) {
			continue;
		}

		if (n <= 0) {
			saved_errno = n == 0 ? EIO : errno;
			close(fd);
			unlinkat(dir_fd, tmp_name, 0);

			errno = saved_errno;

			*err_in = "write salt";

			return false;
		}

		written += (size_t) n;
	}

	close(fd);

	if (linkat(dir_fd, tmp_name, dir_fd, UCACHE_BOUNDARY_SALT_NAME, 0) != 0 && errno != EEXIST) {
		saved_errno = errno;
		unlinkat(dir_fd, tmp_name, 0);

		errno = saved_errno;

		*err_in = "linkat";

		return false;
	}

	unlinkat(dir_fd, tmp_name, 0);

	return true;
}

static bool ucache_shared_boundary_load_salt(const char **err_in)
{
	ucache_storage *storage = &ucache_active_ctx()->storage;
	uint32_t attempt;
	char dir_path[MAXPATHLEN];
	int dir_fd, saved_errno;
	bool missing, loaded = false;

	if (storage->boundary_salt_loaded) {
		return true;
	}

	dir_fd = ucache_shared_boundary_open_private_dir(dir_path, sizeof(dir_path), err_in);
	if (dir_fd < 0) {
		return false;
	}

	for (attempt = 0; attempt < 2 && !loaded; attempt++) {
		loaded = ucache_shared_boundary_read_salt(dir_fd, dir_path, storage->boundary_salt, &missing, err_in);
		if (loaded || !missing || !ucache_shared_boundary_create_salt(dir_fd, err_in)) {
			break;
		}
	}

	saved_errno = errno;

	close(dir_fd);

	errno = saved_errno;

	storage->boundary_salt_loaded = loaded;

	return loaded;
}

static int ucache_shared_boundary_open_shm_fd(
		const char *shm_name,
		size_t requested_size,
		bool *created,
		const char **err_in)
{
	struct stat st;
	uint32_t attempts, create_attempts;
	int fd;

	*created = false;

	for (create_attempts = 0; create_attempts < 2; create_attempts++) {
		fd = shm_open(shm_name, O_RDWR | O_CREAT | O_EXCL, 0600);
		if (fd >= 0) {
			if (ftruncate(fd, (off_t) requested_size) != 0) {
				close(fd);
				shm_unlink(shm_name);

				*err_in = "ftruncate";

				return -1;
			}

			UCACHE_DEBUG_SIMULATE_KILL("EXIT_IN_BOUNDARY_SEGMENT_CREATE");

			if (!ucache_shm_fd_reserve(fd, requested_size)) {
				close(fd);
				shm_unlink(shm_name);

				*err_in = "posix_fallocate";

				return -1;
			}

			*created = true;

			return fd;
		}

		if (errno != EEXIST) {
			*err_in = "shm_open";

			return -1;
		}

		fd = shm_open(shm_name, O_RDWR, 0600);
		if (fd < 0) {
			*err_in = "shm_open";

			return -1;
		}

		if (!ucache_shared_boundary_fd_is_trusted(fd, &st)) {
			close(fd);

			errno = EACCES;
			*err_in = "shm ownership";

			return -1;
		}

		for (attempts = 0; (size_t) st.st_size < requested_size && attempts < 1000; attempts++) {
			if (st.st_size != 0) {
				break;
			}

			usleep(1000);

			if (fstat(fd, &st) != 0) {
				break;
			}
		}

		if ((size_t) st.st_size >= requested_size) {
			if (!ucache_shm_fd_reserve(fd, requested_size)) {
				close(fd);

				*err_in = "posix_fallocate";

				return -1;
			}

			return fd;
		}

		close(fd);

		if (st.st_size == 0 && create_attempts == 0) {
			shm_unlink(shm_name);

			continue;
		}

		errno = EINVAL;
		*err_in = "shm size";

		return -1;
	}

	errno = EINVAL;
	*err_in = "shm size";

	return -1;
}

static void ucache_shared_boundary_tag(const ucache_ctx *ctx, uint8_t tag[32])
{
	PHP_SHA256_CTX sha_ctx;

	ZEND_ASSERT(ucache_ctx_is_boundary(ctx) && ctx->storage.boundary_salt_loaded);

	PHP_SHA256Init(&sha_ctx);
	PHP_SHA256Update(
		&sha_ctx,
		(const uint8_t *) UCACHE_BOUNDARY_TAG_PREFIX,
		sizeof(UCACHE_BOUNDARY_TAG_PREFIX) - 1
	);
	PHP_SHA256Update(&sha_ctx, (const uint8_t *) ctx->boundary_identity, ctx->boundary_identity_len);
	PHP_SHA256Update(&sha_ctx, ctx->storage.boundary_salt, sizeof(ctx->storage.boundary_salt));
	PHP_SHA256Final(tag, &sha_ctx);
}

static bool ucache_shared_boundary_set_attach_lock(int fd, short lock_type, bool wait)
{
	struct flock attach_lock;

	attach_lock.l_type = lock_type;
	attach_lock.l_whence = SEEK_SET;
	attach_lock.l_start = UCACHE_BOUNDARY_ATTACH_BYTE;
	attach_lock.l_len = 1;

	while (fcntl(fd, wait ? F_SETLKW : F_SETLK, &attach_lock) != 0) {
		if (wait && errno == EDEADLK) {
			ucache_platform.sleep_us(UCACHE_BOUNDARY_DEADLOCK_RETRY_US);
		} else if (errno != EINTR) {
			return false;
		}
	}

	return true;
}

static bool ucache_shared_boundary_name_is_file(int dir_fd, const char *name, const struct stat *st)
{
	struct stat named;

	return fstatat(dir_fd, name, &named, AT_SYMLINK_NOFOLLOW) == 0 &&
		named.st_dev == st->st_dev &&
		named.st_ino == st->st_ino
	;
}

static bool ucache_shared_boundary_pread(int fd, void *buf, size_t len, off_t offset)
{
	ssize_t n;

	do {
		n = pread(fd, buf, len, offset);
	} while (n < 0 && errno == EINTR);

	return n == (ssize_t) len;
}

static bool ucache_shared_boundary_pwrite(int fd, const void *buf, size_t len, off_t offset)
{
	ssize_t n;

	do {
		n = pwrite(fd, buf, len, offset);
	} while (n < 0 && errno == EINTR);

	return n == (ssize_t) len;
}

static bool ucache_shared_boundary_file_is_attached_here(const struct stat *st)
{
	const ucache_boundary_seg *seg;

	for (seg = ucache_boundary_segs; seg != NULL; seg = seg->next) {
		if (seg->lock_dev == (uint64_t) st->st_dev && seg->lock_ino == (uint64_t) st->st_ino) {
			return true;
		}
	}

	return false;
}

static bool ucache_shared_boundary_attach_lock_file(ucache_boundary_seg *seg, const char **err_in)
{
	struct stat st;
	char lock_name[UCACHE_BOUNDARY_NAME_SIZE];
	uint32_t attempt;
	int fd;

	ucache_shared_boundary_lock_name(lock_name, seg->id);

	for (attempt = 0; attempt < UCACHE_BOUNDARY_ATTACH_ATTEMPTS; attempt++) {
		fd = openat(seg->dir_fd, lock_name, O_RDWR | O_CREAT | O_CLOEXEC | O_NOFOLLOW, 0600);
		if (fd < 0) {
			*err_in = "open lock file";

			return false;
		}

		if (!ucache_shared_boundary_fd_is_trusted(fd, &st) || !S_ISREG(st.st_mode)) {
			close(fd);

			errno = EACCES;
			*err_in = "lock file ownership";

			return false;
		}

		if (!ucache_shared_boundary_set_attach_lock(fd, F_RDLCK, true)) {
			close(fd);

			*err_in = "fcntl";

			return false;
		}

		if (ucache_shared_boundary_name_is_file(seg->dir_fd, lock_name, &st)) {
			seg->lock_fd = fd;
			seg->lock_dev = (uint64_t) st.st_dev;
			seg->lock_ino = (uint64_t) st.st_ino;

			return true;
		}

		close(fd);
	}

	errno = EAGAIN;
	*err_in = "lock file";

	return false;
}

static void ucache_shared_boundary_write_record(const ucache_boundary_seg *seg, const uint8_t tag[32])
{
	ucache_boundary_lock_record record;

	memcpy(record.boundary_tag, tag, sizeof(record.boundary_tag));

	record.last_used_us = ucache_shared_boundary_now_us();
	record.superseded_at_us = 0;

	ucache_shared_boundary_pwrite(seg->lock_fd, &record, sizeof(record), 0);
}

static bool ucache_shared_boundary_seg_exists(const char *id)
{
	char shm_name[UCACHE_BOUNDARY_NAME_SIZE];
	int fd;

	ucache_shared_boundary_shm_name(shm_name, id);

	fd = shm_open(shm_name, O_RDONLY, 0);
	if (fd < 0) {
		return errno != ENOENT;
	}

	close(fd);

	return true;
}

static void ucache_shared_boundary_remove_idle_seg(
		int dir_fd,
		const char *id,
		const struct stat *lock_st)
{
	char shm_name[UCACHE_BOUNDARY_NAME_SIZE], lock_name[UCACHE_BOUNDARY_NAME_SIZE];

	ucache_shared_boundary_shm_name(shm_name, id);
	ucache_shared_boundary_lock_name(lock_name, id);

	if (!ucache_shared_boundary_name_is_file(dir_fd, lock_name, lock_st) ||
		(shm_unlink(shm_name) != 0 && errno != ENOENT)
	) {
		return;
	}

	unlinkat(dir_fd, lock_name, 0);
}

static int ucache_shared_boundary_open_foreign_lock_file(int dir_fd, const char *id, struct stat *st)
{
	char lock_name[UCACHE_BOUNDARY_NAME_SIZE];
	int fd;

	ucache_shared_boundary_lock_name(lock_name, id);

	if (fstatat(dir_fd, lock_name, st, AT_SYMLINK_NOFOLLOW) != 0 ||
		!S_ISREG(st->st_mode) ||
		ucache_shared_boundary_file_is_attached_here(st)
	) {
		return -1;
	}

	fd = openat(dir_fd, lock_name, O_RDWR | O_CLOEXEC | O_NOFOLLOW);
	if (fd < 0) {
		return -1;
	}

	if (!ucache_shared_boundary_fd_is_trusted(fd, st)) {
		close(fd);

		return -1;
	}

	return fd;
}

static bool ucache_shared_boundary_probe_idle_seg(
		int dir_fd,
		const char *id,
		const uint8_t tag[32],
		uint64_t *last_used_us)
{
	ucache_boundary_lock_record record;
	struct stat st;
	int fd;
	bool has_record, same_boundary, idle;

	fd = ucache_shared_boundary_open_foreign_lock_file(dir_fd, id, &st);
	if (fd < 0) {
		return false;
	}

	has_record = ucache_shared_boundary_pread(fd, &record, sizeof(record), 0);
	same_boundary = has_record && memcmp(record.boundary_tag, tag, sizeof(record.boundary_tag)) == 0;
	idle = ucache_shared_boundary_set_attach_lock(fd, F_WRLCK, false);

	if (!idle && same_boundary && record.superseded_at_us == 0) {
		record.superseded_at_us = ucache_shared_boundary_now_us();

		ucache_shared_boundary_pwrite(
			fd,
			&record.superseded_at_us,
			sizeof(record.superseded_at_us),
			offsetof(ucache_boundary_lock_record, superseded_at_us)
		);
	}

	if (idle && (same_boundary || !ucache_shared_boundary_seg_exists(id))) {
		ucache_shared_boundary_remove_idle_seg(dir_fd, id, &st);

		idle = false;
	}

	*last_used_us = has_record ? record.last_used_us : 0;

	close(fd);

	return idle;
}

static void ucache_shared_boundary_evict_idle_seg(int dir_fd, const char *id)
{
	struct stat st;
	int fd;

	fd = ucache_shared_boundary_open_foreign_lock_file(dir_fd, id, &st);
	if (fd < 0) {
		return;
	}

	if (ucache_shared_boundary_set_attach_lock(fd, F_WRLCK, false)) {
		ucache_shared_boundary_remove_idle_seg(dir_fd, id, &st);
	}

	close(fd);
}

static int ucache_shared_boundary_idle_seg_cmp(const void *a, const void *b)
{
	uint64_t a_used = ((const ucache_boundary_idle_seg *) a)->last_used_us;
	uint64_t b_used = ((const ucache_boundary_idle_seg *) b)->last_used_us;

	return (a_used > b_used) - (a_used < b_used);
}

static void ucache_shared_boundary_idle_seg_swap(void *a, void *b)
{
	ucache_boundary_idle_seg tmp = *(ucache_boundary_idle_seg *) a;

	*(ucache_boundary_idle_seg *) a = *(ucache_boundary_idle_seg *) b;
	*(ucache_boundary_idle_seg *) b = tmp;
}

static bool ucache_shared_boundary_append_idle_seg(
		ucache_boundary_idle_seg **idle,
		size_t *count,
		size_t *capacity,
		const char *id,
		uint64_t last_used_us)
{
	ucache_boundary_idle_seg *grown;
	size_t new_capacity;

	if (*count == *capacity) {
		new_capacity = *capacity != 0 ? *capacity * 2 : UCACHE_BOUNDARY_IDLE_SEG_MAX * 2;
		grown = realloc(*idle, new_capacity * sizeof(**idle));
		if (grown == NULL) {
			return false;
		}

		*idle = grown;
		*capacity = new_capacity;
	}

	memcpy((*idle)[*count].id, id, sizeof((*idle)[*count].id));

	(*idle)[*count].last_used_us = last_used_us;
	(*count)++;

	return true;
}

static void ucache_shared_boundary_sweep_idle_segs(const ucache_boundary_seg *self, const uint8_t tag[32])
{
	ucache_boundary_idle_seg *idle = NULL;
	struct dirent *dirent;
	uint64_t last_used_us;
	size_t count = 0, capacity = 0, i;
	char id[UCACHE_BOUNDARY_ID_LEN + 1];
	DIR *dir;
	int scan_fd;
	bool listed = true;

	scan_fd = openat(self->dir_fd, ".", O_RDONLY | O_DIRECTORY | O_CLOEXEC);
	if (scan_fd < 0) {
		return;
	}

	dir = fdopendir(scan_fd);
	if (dir == NULL) {
		close(scan_fd);

		return;
	}

	while (listed && (dirent = readdir(dir)) != NULL) {
		if (ucache_shared_boundary_lock_name_to_id(dirent->d_name, id) &&
			ucache_shared_boundary_probe_idle_seg(self->dir_fd, id, tag, &last_used_us)
		) {
			listed = ucache_shared_boundary_append_idle_seg(&idle, &count, &capacity, id, last_used_us);
		}
	}

	closedir(dir);

	if (listed && count > UCACHE_BOUNDARY_IDLE_SEG_MAX) {
		zend_sort(
			idle,
			count,
			sizeof(*idle),
			ucache_shared_boundary_idle_seg_cmp,
			ucache_shared_boundary_idle_seg_swap
		);

		for (i = 0; i < count - UCACHE_BOUNDARY_IDLE_SEG_MAX; i++) {
			ucache_shared_boundary_evict_idle_seg(self->dir_fd, idle[i].id);
		}
	}

	free(idle);
}

static void ucache_shared_boundary_close_seg(ucache_boundary_seg *seg)
{
	if (seg->lock_fd >= 0) {
		close(seg->lock_fd);
	}

	if (seg->dir_fd >= 0) {
		close(seg->dir_fd);
	}

	free(seg);
}

static int ucache_shared_boundary_create_seg(
		size_t requested_size,
		ucache_shm_seg **shared_seg_p,
		const char **err_in)
{
	ucache_boundary_seg *seg;
	struct stat shm_st;
	uint8_t tag[32];
	char dir_path[MAXPATHLEN], shm_name[UCACHE_BOUNDARY_NAME_SIZE];
	void *mapping;
	int shm_fd;
	bool created;

	if (requested_size > (size_t) SSIZE_MAX) {
		*err_in = "size overflow";

		return UCACHE_ALLOC_FAILURE;
	}

	if (!ucache_shared_boundary_load_salt(err_in)) {
		return UCACHE_ALLOC_FAILURE;
	}

	if (UCACHE_DEBUG_FAULT("FAIL_BOUNDARY_SEGMENT")) {
		*err_in = "mmap";

		return UCACHE_ALLOC_FAILURE;
	}

	seg = (ucache_boundary_seg *) calloc(1, sizeof(*seg));
	if (seg == NULL) {
		*err_in = "calloc";

		return UCACHE_ALLOC_FAILURE;
	}

	seg->lock_fd = -1;

	ucache_shared_boundary_format_id(seg->id, requested_size);
	ucache_shared_boundary_shm_name(shm_name, seg->id);

	seg->dir_fd = ucache_shared_boundary_open_private_dir(dir_path, sizeof(dir_path), err_in);
	if (seg->dir_fd < 0 || !ucache_shared_boundary_attach_lock_file(seg, err_in)) {
		ucache_shared_boundary_close_seg(seg);

		return UCACHE_ALLOC_FAILURE;
	}

	shm_fd = ucache_shared_boundary_open_shm_fd(shm_name, requested_size, &created, err_in);
	if (shm_fd < 0) {
		ucache_shared_boundary_close_seg(seg);

		return UCACHE_ALLOC_FAILURE;
	}

	if (fstat(shm_fd, &shm_st) == 0) {
		seg->shm_dev = (uint64_t) shm_st.st_dev;
		seg->shm_ino = (uint64_t) shm_st.st_ino;
	}

	mapping = mmap(NULL, requested_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	close(shm_fd);

	if (mapping == MAP_FAILED) {
		if (created) {
			shm_unlink(shm_name);
		}

		ucache_shared_boundary_close_seg(seg);

		*err_in = "mmap";

		return UCACHE_ALLOC_FAILURE;
	}

	seg->seg.p = mapping;
	seg->seg.size = requested_size;
	seg->next = ucache_boundary_segs;

	ucache_boundary_segs = seg;

	ucache_shared_boundary_tag(ucache_active_ctx(), tag);
	ucache_shared_boundary_write_record(seg, tag);
	ucache_shared_boundary_sweep_idle_segs(seg, tag);

	*shared_seg_p = &seg->seg;

	return UCACHE_ALLOC_SUCCESS;
}

static void ucache_shared_boundary_detach_seg(ucache_shm_seg *shared_seg)
{
	ucache_boundary_seg *seg = (ucache_boundary_seg *) shared_seg, **link;
	struct stat lock_st;
	uint64_t now_us, superseded_at_us;

	munmap(seg->seg.p, seg->seg.size);

	for (link = &ucache_boundary_segs; *link != NULL; link = &(*link)->next) {
		if (*link == seg) {
			*link = seg->next;

			break;
		}
	}

	now_us = ucache_shared_boundary_now_us();

	ucache_shared_boundary_pwrite(
		seg->lock_fd,
		&now_us,
		sizeof(now_us),
		offsetof(ucache_boundary_lock_record, last_used_us)
	);

	if (ucache_shared_boundary_pread(
			seg->lock_fd,
			&superseded_at_us,
			sizeof(superseded_at_us),
			offsetof(ucache_boundary_lock_record, superseded_at_us)
		) &&
		superseded_at_us != 0 &&
		ucache_shared_boundary_set_attach_lock(seg->lock_fd, F_WRLCK, false) &&
		fstat(seg->lock_fd, &lock_st) == 0
	) {
		ucache_shared_boundary_remove_idle_seg(seg->dir_fd, seg->id, &lock_st);
	}

	close(seg->lock_fd);
	close(seg->dir_fd);
}
#endif /* UCACHE_HAVE_BOUNDARY_SHM */

#ifdef ZEND_WIN32
static void ucache_win32_set_seg(
		ucache_win32_seg *seg,
		HANDLE memfile,
		void *mapping_base,
		size_t requested_size)
{
	seg->memfile = memfile;
	seg->seg.p = mapping_base;
	seg->seg.size = requested_size;
}

static bool ucache_win32_load_token_sids(ucache_win32_token_sids *sids)
{
	HANDLE token;
	DWORD len;
	bool loaded;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
		return false;
	}

	loaded = GetTokenInformation(token, TokenUser, sids->user.bytes, sizeof(sids->user.bytes), &len) &&
		GetTokenInformation(token, TokenOwner, sids->owner.bytes, sizeof(sids->owner.bytes), &len)
	;

	CloseHandle(token);

	return loaded;
}

static PSECURITY_DESCRIPTOR ucache_win32_owner_only_descriptor(void)
{
	ucache_win32_token_sids sids;
	PSECURITY_DESCRIPTOR descriptor = NULL;
	wchar_t *sid_str, sddl[256];
	int sddl_len;

	if (!ucache_win32_load_token_sids(&sids) ||
		!ConvertSidToStringSidW(sids.user.info.User.Sid, &sid_str)
	) {
		return NULL;
	}

	sddl_len = swprintf(sddl, sizeof(sddl) / sizeof(sddl[0]), UCACHE_WIN32_OWNER_ONLY_SDDL, sid_str);

	LocalFree(sid_str);

	if (sddl_len < 0 ||
		!ConvertStringSecurityDescriptorToSecurityDescriptorW(sddl, SDDL_REVISION_1, &descriptor, NULL)
	) {
		return NULL;
	}

	return descriptor;
}

static bool ucache_win32_private_dir_is_trusted(const wchar_t *dir)
{
	DWORD attributes = GetFileAttributesW(dir);
	HANDLE handle;
	bool owned;

	if (attributes == INVALID_FILE_ATTRIBUTES ||
		(attributes & FILE_ATTRIBUTE_DIRECTORY) == 0 ||
		(attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0
	) {
		SetLastError(ERROR_ACCESS_DENIED);

		return false;
	}

	handle = CreateFileW(
		dir,
		READ_CONTROL,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
		NULL
	);
	if (handle == INVALID_HANDLE_VALUE) {
		return false;
	}

	owned = ucache_win32_owned_by_cur_user(handle, SE_FILE_OBJECT);

	CloseHandle(handle);

	return owned;
}

static bool ucache_win32_open_private_dir(wchar_t *dir, size_t dir_size, SECURITY_ATTRIBUTES *attributes)
{
	ucache_win32_token_sids sids;
	wchar_t *sid_str;
	DWORD len;
	int written;

	len = GetTempPathW((DWORD) dir_size, dir);
	if (len == 0 || len >= dir_size ||
		!ucache_win32_load_token_sids(&sids) ||
		!ConvertSidToStringSidW(sids.user.info.User.Sid, &sid_str)
	) {
		return false;
	}

	written = swprintf(dir + len, dir_size - len, L"%ls%ls", UCACHE_WIN32_PRIVATE_DIR_PREFIX, sid_str);

	LocalFree(sid_str);

	if (written < 0 || (size_t) written >= dir_size - len) {
		return false;
	}

	if (!CreateDirectoryW(dir, attributes) && GetLastError() != ERROR_ALREADY_EXISTS) {
		return false;
	}

	return ucache_win32_private_dir_is_trusted(dir);
}

static bool ucache_win32_read_salt(const wchar_t *path, bool *missing)
{
	HANDLE file;
	DWORD read_len = 0;
	uint32_t attempt;
	bool loaded = false;

	*missing = false;

	for (attempt = 0; attempt < UCACHE_WIN32_SALT_READ_ATTEMPTS; attempt++) {
		file = CreateFileW(
			path,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OPEN_REPARSE_POINT,
			NULL
		);
		if (file != INVALID_HANDLE_VALUE) {
			break;
		}

		if (GetLastError() == ERROR_FILE_NOT_FOUND) {
			*missing = true;

			return false;
		}

		if (GetLastError() != ERROR_SHARING_VIOLATION) {
			return false;
		}

		Sleep(1);
	}

	if (file == INVALID_HANDLE_VALUE) {
		return false;
	}

	if (ucache_win32_owned_by_cur_user(file, SE_FILE_OBJECT) &&
		ReadFile(file, ucache_win32_salt, sizeof(ucache_win32_salt), &read_len, NULL) &&
		read_len == sizeof(ucache_win32_salt)
	) {
		loaded = true;
	}

	CloseHandle(file);

	return loaded;
}

static bool ucache_win32_create_salt(const wchar_t *dir, const wchar_t *path, SECURITY_ATTRIBUTES *attributes)
{
	HANDLE file;
	DWORD written_len = 0;
	uint8_t salt[UCACHE_WIN32_SALT_SIZE];
	wchar_t tmp_path[MAXPATHLEN];
	int written;
	bool created;

	written = swprintf(
		tmp_path,
		sizeof(tmp_path) / sizeof(tmp_path[0]),
		L"%ls\\%ls.%lu",
		dir,
		UCACHE_WIN32_SALT_NAME,
		(unsigned long) GetCurrentProcessId()
	);
	if (written < 0 || php_random_bytes_silent(salt, sizeof(salt)) == FAILURE) {
		return false;
	}

	file = CreateFileW(tmp_path, GENERIC_WRITE, 0, attributes, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE) {
		return false;
	}

	created = WriteFile(file, salt, sizeof(salt), &written_len, NULL) && written_len == sizeof(salt);

	CloseHandle(file);

	if (created && !MoveFileExW(tmp_path, path, MOVEFILE_WRITE_THROUGH) && GetLastError() != ERROR_ALREADY_EXISTS) {
		created = false;
	}

	DeleteFileW(tmp_path);

	return created;
}

static HANDLE ucache_win32_create_owned_mutex(const char *name, const char **err_in)
{
	SECURITY_ATTRIBUTES attributes;
	PSECURITY_DESCRIPTOR descriptor = ucache_win32_owner_only_descriptor();
	HANDLE mutex;

	if (descriptor == NULL) {
		*err_in = "owner-only descriptor";

		return NULL;
	}

	attributes.nLength = sizeof(attributes);
	attributes.lpSecurityDescriptor = descriptor;
	attributes.bInheritHandle = FALSE;

	mutex = CreateMutexA(&attributes, FALSE, name);

	LocalFree(descriptor);

	if (mutex == NULL) {
		*err_in = "CreateMutexA";

		return NULL;
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS && !ucache_win32_owned_by_cur_user(mutex, SE_KERNEL_OBJECT)) {
		CloseHandle(mutex);

		*err_in = "mutex owner";

		return NULL;
	}

	return mutex;
}

static bool ucache_win32_view_spans(void *mapping_base, size_t requested_size)
{
	MEMORY_BASIC_INFORMATION info;
	char *cursor = mapping_base;
	size_t spanned = 0;

	while (spanned < requested_size) {
		if (VirtualQuery(cursor, &info, sizeof(info)) == 0 ||
			info.AllocationBase != mapping_base ||
			info.Type != MEM_MAPPED ||
			info.RegionSize == 0
		) {
			return false;
		}

		spanned += info.RegionSize;
		cursor += info.RegionSize;
	}

	return true;
}

static int ucache_win32_reattach_seg(
		ucache_win32_seg *seg,
		HANDLE memfile,
		size_t requested_size,
		const char **err_in)
{
	void *mapping_base;

	mapping_base = MapViewOfFileEx(memfile, FILE_MAP_ALL_ACCESS, 0, 0, 0, NULL);
	if (mapping_base == NULL) {
		*err_in = "MapViewOfFileEx";

		return UCACHE_ALLOC_FAILURE;
	}

	if (!ucache_win32_view_spans(mapping_base, requested_size)) {
		UnmapViewOfFile(mapping_base);

		*err_in = "VirtualQuery";

		return UCACHE_ALLOC_FAILURE;
	}

	ucache_win32_set_seg(seg, memfile, mapping_base, requested_size);

	return UCACHE_ALLOC_SUCCESS;
}

static int ucache_win32_create_mapping(
		ucache_win32_seg *seg,
		const char *mapping_name,
		size_t requested_size,
		const char **err_in)
{
	SECURITY_ATTRIBUTES attributes;
	PSECURITY_DESCRIPTOR descriptor = NULL;
	HANDLE memfile;
	DWORD size_high, size_low, last_error;
	int result;
	void *mapping_base;

#if defined(_WIN64)
	size_high = (DWORD) (requested_size >> 32);
	size_low = (DWORD) (requested_size & 0xffffffff);
#else
	if (requested_size > UINT32_MAX) {
		*err_in = "size overflow";

		return UCACHE_ALLOC_FAILURE;
	}

	size_high = 0;
	size_low = (DWORD) requested_size;
#endif /* _WIN64 */

	if (mapping_name != NULL) {
		descriptor = ucache_win32_owner_only_descriptor();
		if (descriptor == NULL) {
			*err_in = "owner-only descriptor";

			return UCACHE_ALLOC_FAILURE;
		}

		attributes.nLength = sizeof(attributes);
		attributes.lpSecurityDescriptor = descriptor;
		attributes.bInheritHandle = FALSE;
	}

	memfile = CreateFileMappingA(
		INVALID_HANDLE_VALUE,
		descriptor != NULL ? &attributes : NULL,
		PAGE_READWRITE | SEC_RESERVE,
		size_high,
		size_low,
		mapping_name
	);
	last_error = GetLastError();

	if (descriptor != NULL) {
		LocalFree(descriptor);
	}

	if (memfile == NULL) {
		*err_in = "CreateFileMappingA";

		return UCACHE_ALLOC_FAILURE;
	}

	if (mapping_name != NULL && last_error == ERROR_ALREADY_EXISTS) {
		if (!ucache_win32_owned_by_cur_user(memfile, SE_KERNEL_OBJECT)) {
			CloseHandle(memfile);

			*err_in = "mapping owner";

			return UCACHE_ALLOC_FAILURE;
		}

		result = ucache_win32_reattach_seg(seg, memfile, requested_size, err_in);
		if (result != UCACHE_ALLOC_SUCCESS) {
			CloseHandle(memfile);
		}

		return result;
	}

	mapping_base = MapViewOfFileEx(memfile, FILE_MAP_ALL_ACCESS, 0, 0, 0, NULL);
	if (mapping_base == NULL) {
		CloseHandle(memfile);

		*err_in = "MapViewOfFileEx";

		return UCACHE_ALLOC_FAILURE;
	}

	ucache_win32_set_seg(seg, memfile, mapping_base, requested_size);

	seg->seg.zero_filled = mapping_name == NULL;

	return UCACHE_ALLOC_SUCCESS;
}

static int ucache_win32_create_seg(
		size_t requested_size,
		ucache_shm_seg **shared_seg_p,
		const char **err_in)
{
	ucache_win32_seg *seg;
	HANDLE mutex, memfile;
	DWORD wait_result;
	int result;
	char mapping_name[MAXPATHLEN], mutex_name[MAXPATHLEN];

	seg = (ucache_win32_seg *) calloc(1, sizeof(*seg));
	if (seg == NULL) {
		*err_in = "calloc";

		return UCACHE_ALLOC_FAILURE;
	}

	if (ucache_win32_ctx_is_proc_private(ucache_active_ctx())) {
		result = ucache_win32_create_mapping(seg, NULL, requested_size, err_in);
		if (result != UCACHE_ALLOC_SUCCESS) {
			goto bailout;
		}

		*shared_seg_p = &seg->seg;

		return UCACHE_ALLOC_SUCCESS;
	}

	if (!ucache_win32_load_salt(err_in)) {
		goto bailout;
	}

	ucache_win32_format_name(
		mapping_name,
		sizeof(mapping_name),
		UCACHE_WIN32_MAPPING_NAME,
		requested_size
	);
	ucache_win32_format_name(
		mutex_name,
		sizeof(mutex_name),
		UCACHE_WIN32_MAPPING_MUTEX_NAME,
		requested_size
	);

	mutex = ucache_win32_create_owned_mutex(mutex_name, err_in);
	if (mutex == NULL) {
		goto bailout;
	}

	wait_result = WaitForSingleObject(mutex, INFINITE);
	if (wait_result != WAIT_OBJECT_0 && wait_result != WAIT_ABANDONED) {
		CloseHandle(mutex);

		*err_in = "WaitForSingleObject";

		goto bailout;
	}

	memfile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, mapping_name);
	if (memfile != NULL && !ucache_win32_owned_by_cur_user(memfile, SE_KERNEL_OBJECT)) {
		CloseHandle(memfile);

		*err_in = "mapping owner";
		result = UCACHE_ALLOC_FAILURE;
	} else if (memfile != NULL) {
		result = ucache_win32_reattach_seg(seg, memfile, requested_size, err_in);
		if (result != UCACHE_ALLOC_SUCCESS) {
			CloseHandle(memfile);
		}
	} else {
		result = ucache_win32_create_mapping(seg, mapping_name, requested_size, err_in);
	}

	ReleaseMutex(mutex);
	CloseHandle(mutex);

	if (result != UCACHE_ALLOC_SUCCESS) {
		goto bailout;
	}

	*shared_seg_p = &seg->seg;

	return UCACHE_ALLOC_SUCCESS;

bailout:
	free(seg);

	return UCACHE_ALLOC_FAILURE;
}

static void ucache_win32_detach_seg(ucache_shm_seg *shared_seg)
{
	ucache_win32_seg *seg = (ucache_win32_seg *) shared_seg;

	UnmapViewOfFile(seg->seg.p);
	CloseHandle(seg->memfile);
}
#endif /* ZEND_WIN32 */

#ifdef UCACHE_HAVE_BOUNDARY_SHM
void ucache_shared_boundary_digest(
		const ucache_ctx *ctx,
		size_t requested_size,
		uint8_t digest[32])
{
	PHP_SHA256_CTX sha_ctx;
	uint32_t lock_capacity;
	size_t update_len = 0;
	bool clamped;
	int prefix_len;
	char prefix[128];

	ZEND_ASSERT(ucache_ctx_is_boundary(ctx));

	lock_capacity = ucache_calc_entry_lock_capacity(
		ucache_calc_capacity(requested_size, &clamped)
	);

	prefix_len = snprintf(
		prefix,
		sizeof(prefix),
		"PhpUserCache.boundary-shm|%zu|" ZEND_LONG_FMT "|%u|%zu|",
		requested_size,
		UC_G(entries_hint),
		lock_capacity,
		ctx->boundary_identity_len
	);

	PHP_SHA256Init(&sha_ctx);

	if (prefix_len > 0) {
		update_len = (size_t) prefix_len;
		if (update_len >= sizeof(prefix)) {
			update_len = sizeof(prefix) - 1;
		}

		PHP_SHA256Update(&sha_ctx, (const uint8_t *) prefix, update_len);
	}

	ucache_sha256_update_build_identity(&sha_ctx);

	PHP_SHA256Update(&sha_ctx, (const uint8_t *) ctx->boundary_identity, ctx->boundary_identity_len);

	ZEND_ASSERT(ctx->storage.boundary_salt_loaded);

	PHP_SHA256Update(&sha_ctx, ctx->storage.boundary_salt, sizeof(ctx->storage.boundary_salt));
	PHP_SHA256Final(digest, &sha_ctx);
}

void ucache_shared_boundary_retire_seg_name(void)
{
	ucache_boundary_seg *seg = ucache_boundary_seg_of(&ucache_active_ctx()->storage);
	struct stat st;
	char shm_name[UCACHE_BOUNDARY_NAME_SIZE];
	int fd;

	ucache_shared_boundary_shm_name(shm_name, seg->id);

	fd = shm_open(shm_name, O_RDONLY, 0);
	if (fd < 0) {
		return;
	}

	if (fstat(fd, &st) == 0 &&
		seg->shm_ino != 0 &&
		(uint64_t) st.st_dev == seg->shm_dev &&
		(uint64_t) st.st_ino == seg->shm_ino
	) {
		shm_unlink(shm_name);
	}

	close(fd);
}

void ucache_shared_boundary_flush_dir_failure_log(void)
{
	if (atomic_exchange(&ucache_boundary_dir_failure_pending, false)) {
		ucache_log_err(ucache_boundary_dir_failure_msg);
	}
}

void ucache_shared_boundary_segs_after_fork(void)
{
	const ucache_boundary_seg *seg;

	for (seg = ucache_boundary_segs; seg != NULL; seg = seg->next) {
		ucache_shared_boundary_set_attach_lock(seg->lock_fd, F_RDLCK, false);
	}
}

const ucache_shm_handler_entry *ucache_shared_boundary_handler_entry(void)
{
	static const ucache_shm_handlers handlers = {
		ucache_shared_boundary_create_seg,
		ucache_shared_boundary_detach_seg
	};
	static const ucache_shm_handler_entry entry = {
		"boundary-shm", &handlers
	};

	return &entry;
}
#endif /* UCACHE_HAVE_BOUNDARY_SHM */

#ifdef ZEND_WIN32
bool ucache_win32_ctx_is_proc_private(const ucache_ctx *ctx)
{
	return ctx == &ucache_ctx_state;
}

bool ucache_win32_owned_by_cur_user(HANDLE handle, SE_OBJECT_TYPE type)
{
	ucache_win32_token_sids sids;
	PSECURITY_DESCRIPTOR descriptor = NULL;
	PSID owner = NULL;
	bool owned;

	if (handle == NULL || handle == INVALID_HANDLE_VALUE || !ucache_win32_load_token_sids(&sids) ||
		GetSecurityInfo(handle, type, OWNER_SECURITY_INFORMATION, &owner, NULL, NULL, NULL, &descriptor) != ERROR_SUCCESS
	) {
		return false;
	}

	owned = owner != NULL && (
		EqualSid(owner, sids.user.info.User.Sid) ||
		EqualSid(owner, sids.owner.info.Owner)
	);

	LocalFree(descriptor);

	if (!owned) {
		SetLastError(ERROR_ACCESS_DENIED);
	}

	return owned;
}

bool ucache_win32_load_salt(const char **err_in)
{
	SECURITY_ATTRIBUTES attributes;
	PSECURITY_DESCRIPTOR descriptor;
	wchar_t dir[MAXPATHLEN], path[MAXPATHLEN];
	uint32_t attempt;
	int written;
	bool missing;

	if (ucache_win32_salt_loaded) {
		return true;
	}

	descriptor = ucache_win32_owner_only_descriptor();
	if (descriptor == NULL) {
		*err_in = "owner-only descriptor";

		return false;
	}

	attributes.nLength = sizeof(attributes);
	attributes.lpSecurityDescriptor = descriptor;
	attributes.bInheritHandle = FALSE;

	if (!ucache_win32_open_private_dir(dir, sizeof(dir) / sizeof(dir[0]), &attributes)) {
		LocalFree(descriptor);

		*err_in = "private directory";

		return false;
	}

	written = swprintf(path, sizeof(path) / sizeof(path[0]), L"%ls\\%ls", dir, UCACHE_WIN32_SALT_NAME);

	for (attempt = 0; written >= 0 && attempt < 2 && !ucache_win32_salt_loaded; attempt++) {
		ucache_win32_salt_loaded = ucache_win32_read_salt(path, &missing);
		if (ucache_win32_salt_loaded || !missing || !ucache_win32_create_salt(dir, path, &attributes)) {
			break;
		}
	}

	LocalFree(descriptor);

	if (!ucache_win32_salt_loaded) {
		*err_in = "salt";
	}

	return ucache_win32_salt_loaded;
}

void ucache_win32_format_name(char *buf, size_t buf_size, const char *name, size_t unique_id)
{
	const char *sapi_name = sapi_module.name != NULL ? sapi_module.name : "", *identity;
	ucache_ctx *ctx = ucache_active_ctx();
	PHP_SHA256_CTX sha_ctx;
	DWORD user_name_len = UNLEN + 1;
	uint8_t digest[32];
	size_t identity_len;
	wchar_t user_name[UNLEN + 1];
	int layout_len;
	char layout[256], name_id[40];
	bool clamped;

	identity = ctx->boundary_identity != NULL
		? ctx->boundary_identity
		: (ctx->lock_name != NULL ? ctx->lock_name : "")
	;
	identity_len = ctx->boundary_identity != NULL
		? ctx->boundary_identity_len
		: strlen(identity)
	;

	layout_len = snprintf(
		layout,
		sizeof(layout),
		"PhpUserCache.win32|%zx|" ZEND_LONG_FMT "|%u|%.20s|%lu|%zu|",
		unique_id,
		UC_G(entries_hint),
		ucache_calc_entry_lock_capacity(ucache_calc_capacity(unique_id, &clamped)),
		sapi_name,
		ucache_win32_ctx_is_proc_private(ctx) ? (unsigned long) GetCurrentProcessId() : 0UL,
		identity_len
	);

	PHP_SHA256Init(&sha_ctx);

	/* Several accounts can share one session namespace (e.g. IIS app pools). */
	if (GetUserNameW(user_name, &user_name_len) && user_name_len != 0) {
		PHP_SHA256Update(&sha_ctx, (const uint8_t *) user_name, (user_name_len - 1) * sizeof(wchar_t));
	}

	if (layout_len > 0) {
		PHP_SHA256Update(&sha_ctx, (const uint8_t *) layout, MIN((size_t) layout_len, sizeof(layout) - 1));
	}

	ucache_sha256_update_build_identity(&sha_ctx);

	if (!ucache_win32_ctx_is_proc_private(ctx) && ucache_win32_salt_loaded) {
		PHP_SHA256Update(&sha_ctx, ucache_win32_salt, sizeof(ucache_win32_salt));
	}

	PHP_SHA256Update(&sha_ctx, (const uint8_t *) identity, identity_len);
	PHP_SHA256Final(digest, &sha_ctx);

	zend_bin2hex(name_id, digest, sizeof(name_id) / 2);

	snprintf(buf, buf_size, "%s@%.40s", name, name_id);
}

bool ucache_win32_commit_to(ucache_win32_seg *seg, size_t end_bytes)
{
	size_t end;

	if (end_bytes <= seg->committed_bytes) {
		return true;
	}

	if (seg->committed_bytes != 0 && UCACHE_DEBUG_FAULT("FAIL_WIN32_COMMIT")) {
		SetLastError(ERROR_COMMITMENT_LIMIT);

		return false;
	}

	end = ZEND_MM_ALIGNED_SIZE_EX(end_bytes, UCACHE_WIN32_COMMIT_CHUNK);
	if (end > seg->seg.size) {
		end = seg->seg.size;
	}

	if (VirtualAlloc(
			(char *) seg->seg.p + seg->committed_bytes,
			end - seg->committed_bytes,
			MEM_COMMIT,
			PAGE_READWRITE
		) == NULL
	) {
		return false;
	}

	seg->committed_bytes = end;

	return true;
}

void ucache_win32_reset_range(ucache_win32_seg *seg, size_t start_bytes, size_t end_bytes)
{
	ZEND_ASSERT(start_bytes % UCACHE_WIN32_COMMIT_CHUNK == 0 && start_bytes < end_bytes);

	if (ucache_win32_commit_to(seg, end_bytes)) {
		VirtualAlloc((char *) seg->seg.p + start_bytes, end_bytes - start_bytes, MEM_RESET, PAGE_NOACCESS);
	}
}
#endif /* ZEND_WIN32 */

const ucache_shm_handler_entry *ucache_handler_table(void)
{
#ifdef UCACHE_USE_MMAP
	static const ucache_shm_handlers mmap_handlers = {
		ucache_mmap_create_seg,
		ucache_munmap_detach_seg
	};
#endif /* UCACHE_USE_MMAP */
#ifdef ZEND_WIN32
	static const ucache_shm_handlers win32_handlers = {
		ucache_win32_create_seg,
		ucache_win32_detach_seg
	};
#endif /* ZEND_WIN32 */
	static const ucache_shm_handler_entry handlers[] = {
#ifdef UCACHE_USE_MMAP
		{ "mmap", &mmap_handlers },
#endif /* UCACHE_USE_MMAP */
#ifdef UCACHE_USE_SHM
		{ "shm", &ucache_alloc_shm_handlers },
#endif /* UCACHE_USE_SHM */
#ifdef UCACHE_USE_SHM_OPEN
		{ "posix", &ucache_alloc_posix_handlers },
#endif /* UCACHE_USE_SHM_OPEN */
#ifdef ZEND_WIN32
		{ "win32", &win32_handlers },
#endif /* ZEND_WIN32 */
		{ NULL, NULL }
	};

	return handlers;
}

void ucache_cleanup_seg(const ucache_shm_handlers *handler, ucache_shm_seg *seg)
{
	if (handler == NULL || seg == NULL) {
		return;
	}

	handler->detach_seg(seg);

	free(seg);
}
