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

typedef struct {
	char *key;
	uint32_t key_len;
	zend_ulong hash;
	uint64_t owner_pid;
	uint64_t owner_start_time;
	uint64_t owner_token;
	uint32_t expires_at;
} ucache_recovered_entry_lock;

static bool ucache_owner_is_dead_probe(uint64_t owner_pid, uint64_t owner_start_time)
{
	uint64_t cur_start_time;

	if (ucache_platform.proc_has_exited(owner_pid)) {
		return true;
	}

	if (owner_start_time != 0) {
		cur_start_time = ucache_proc_owner_token(owner_pid);
		if (cur_start_time != 0 && cur_start_time != owner_start_time) {
			return true;
		}
	}

	return false;
}

static bool ucache_recovery_lock_key_is_readable(
		const ucache_hdr *hdr,
		const ucache_entry_lock_record *record)
{
	if (record->key_offset == 0 || record->key_len == 0) {
		return false;
	}

	return ucache_payload_in_bounds(hdr, record->key_offset, record->key_len);
}

static bool ucache_recovery_entry_lock_is_duplicate(
		const ucache_entry_lock_record *record,
		const ucache_recovered_entry_lock *locks,
		uint32_t count)
{
	uint32_t i;

	for (i = 0; i < count; i++) {
		if (locks[i].hash == record->hash &&
			locks[i].owner_token == record->owner_token &&
			locks[i].owner_pid == record->owner_pid &&
			locks[i].key_len == record->key_len &&
			memcmp(locks[i].key, ucache_ptr(record->key_offset), record->key_len) == 0
		) {
			return true;
		}
	}

	return false;
}

static bool ucache_entry_lock_layout_sane(
		const ucache_storage *storage,
		const ucache_hdr *hdr)
{
	uint32_t lock_capacity = hdr->entry_lock_capacity;

	return lock_capacity >= UCACHE_ENTRY_LOCK_MIN_CAPACITY &&
		lock_capacity <= UCACHE_ENTRY_LOCK_MAX_CAPACITY &&
		(lock_capacity & (lock_capacity - 1)) == 0 &&
		hdr->entry_lock_offset >= sizeof(ucache_hdr) &&
		(size_t) hdr->entry_lock_offset <= storage->size &&
		(size_t) lock_capacity * sizeof(ucache_entry_lock_record)
			<= storage->size - hdr->entry_lock_offset
	;
}

static bool ucache_data_region_sane(
		const ucache_storage *storage,
		const ucache_hdr *hdr)
{
	uint64_t lock_table_end = (uint64_t) hdr->entry_lock_offset
		+ (uint64_t) hdr->entry_lock_capacity * sizeof(ucache_entry_lock_record)
	;

	return (uint64_t) hdr->data_offset >= lock_table_end &&
		(size_t) hdr->data_offset <= storage->size &&
		hdr->data_size <= (uint64_t) storage->size - hdr->data_offset
	;
}

static bool ucache_free_bins_layout_sane(
		const ucache_storage *storage,
		const ucache_hdr *hdr)
{
	return hdr->free_bin_count != 0 &&
		hdr->free_bin_count <= UCACHE_FREE_BINS &&
		hdr->free_bins_offset >= sizeof(ucache_hdr) &&
		(uint64_t) hdr->free_bins_offset + ucache_free_bins_bytes(hdr->free_bin_count) <= hdr->data_offset &&
		(size_t) hdr->data_offset <= storage->size
	;
}

static bool ucache_collect_recovery_entry_locks_locked(
		ucache_hdr *hdr,
		ucache_recovered_entry_lock **locks_ptr,
		uint32_t *count_ptr)
{
	ucache_recovered_entry_lock *locks;
	ucache_entry_lock_record *record;
	uint64_t now = ucache_time_rel(hdr, ucache_clock_now());
	uint32_t i, count = 0;
	char *key;

	for (i = 0; i < hdr->entry_lock_capacity; i++) {
		record = &ucache_entry_lock_records_ptr(hdr)[i];
		if (ucache_entry_lock_record_is_active_locked(record, now) &&
			ucache_recovery_lock_key_is_readable(hdr, record)
		) {
			count++;
		}
	}

	*locks_ptr = NULL;
	*count_ptr = 0;

	if (count == 0) {
		return true;
	}

	locks = (ucache_recovered_entry_lock *) calloc(count, sizeof(*locks));
	if (locks == NULL) {
		return false;
	}

	for (i = 0; i < hdr->entry_lock_capacity; i++) {
		record = &ucache_entry_lock_records_ptr(hdr)[i];
		if (!ucache_entry_lock_record_is_active_locked(record, now) ||
			!ucache_recovery_lock_key_is_readable(hdr, record) ||
			ucache_recovery_entry_lock_is_duplicate(record, locks, *count_ptr)
		) {
			continue;
		}

		ZEND_ASSERT(*count_ptr < count);

		key = (char *) malloc(record->key_len);
		if (key == NULL) {
			while (*count_ptr != 0) {
				free(locks[--*count_ptr].key);
			}

			free(locks);

			return false;
		}

		memcpy(key, ucache_ptr(record->key_offset), record->key_len);

		locks[*count_ptr].key = key;
		locks[*count_ptr].key_len = record->key_len;
		locks[*count_ptr].hash = record->hash;
		locks[*count_ptr].owner_pid = record->owner_pid;
		locks[*count_ptr].owner_start_time = record->owner_start_time;
		locks[*count_ptr].owner_token = record->owner_token;
		locks[*count_ptr].expires_at = record->expires_at;

		(*count_ptr)++;
	}

	*locks_ptr = locks;

	return true;
}

static void ucache_free_recovery_entry_locks(
		ucache_recovered_entry_lock *locks,
		uint32_t count)
{
	uint32_t i;

	if (locks == NULL) {
		return;
	}

	for (i = 0; i < count; i++) {
		free(locks[i].key);
	}

	free(locks);
}

static void ucache_restore_recovery_entry_locks_locked(
		ucache_hdr *hdr,
		ucache_recovered_entry_lock *locks,
		uint32_t count)
{
	ucache_entry_lock_record *record;
	uint32_t i, probe, slot_idx, key_offset;

	for (i = 0; i < count; i++) {
		if (locks[i].key == NULL) {
			continue;
		}

		for (probe = 0; probe < hdr->entry_lock_capacity; probe++) {
			slot_idx = (ucache_entry_lock_table_idx(hdr, locks[i].hash) + probe) &
				(hdr->entry_lock_capacity - 1)
			;

			record = &ucache_entry_lock_records_ptr(hdr)[slot_idx];
			if (record->state == UCACHE_ENTRY_LOCK_USED) {
				continue;
			}

			key_offset = ucache_alloc_locked(locks[i].key_len, locks[i].key, UCACHE_BLOCK_OWNER_NONE);
			if (key_offset == 0) {
				break;
			}

			memset(record, 0, sizeof(*record));

			record->hash = locks[i].hash;
			record->owner_pid = locks[i].owner_pid;
			record->owner_start_time = locks[i].owner_start_time;
			record->owner_token = locks[i].owner_token;
			record->expires_at = locks[i].expires_at;
			record->key_offset = key_offset;
			record->key_len = locks[i].key_len;
			record->state = UCACHE_ENTRY_LOCK_USED;

			hdr->entry_lock_count++;

			break;
		}
	}
}

static bool ucache_recovery_blocked_by_live_ref(const ucache_hdr *hdr)
{
	const ucache_reader_slot *reader_slot;
	ucache_graph_pin_slot *pin_slot;
	uint64_t owner_start_time, owner_pid;
	uint32_t i;

	ucache_atomic_fence_seq_cst();

	for (i = 0; i < UCACHE_READER_SLOTS; i++) {
		reader_slot = &hdr->reader_slots[i];
		if (ucache_atomic_load_32(&reader_slot->active) == 0) {
			continue;
		}

		owner_pid = ucache_atomic_load_64(&reader_slot->owner_pid);
		owner_start_time = ucache_atomic_load_64(&reader_slot->owner_start_time);
		if (owner_pid != 0 &&
			!ucache_owner_is_dead(owner_pid, owner_start_time)
		) {
			return true;
		}
	}

	for (i = 0; i < ucache_graph_pin_slot_count(hdr); i++) {
		pin_slot = (ucache_graph_pin_slot *) &hdr->graph_pin_slots[i];
		if (atomic_load(&pin_slot->pin_count) == 0) {
			continue;
		}

		owner_pid = (uint64_t) (uint32_t) atomic_load(&pin_slot->owner_pid);
		if (owner_pid != 0 &&
			owner_pid != (uint64_t) (uint32_t) UCACHE_GRAPH_PIN_OWNER_ABANDONED &&
			!ucache_owner_is_dead(owner_pid, ucache_atomic_load_64(&pin_slot->owner_start_time))
		) {
			return true;
		}
	}

	return false;
}

static void ucache_recovery_release_dead_graph_pin_slots(ucache_hdr *hdr)
{
	ucache_graph_pin_slot *slot;
	uint64_t owner_start_time;
	uint32_t i;
	int owner, pin_count;

	for (i = 0; i < ucache_graph_pin_slot_count(hdr); i++) {
		slot = &hdr->graph_pin_slots[i];

		pin_count = atomic_load(&slot->pin_count);
		if (pin_count == 0) {
			continue;
		}

		owner_start_time = ucache_atomic_load_64(&slot->owner_start_time);
		owner = atomic_load(&slot->owner_pid);
		if (owner != UCACHE_GRAPH_PIN_OWNER_ABANDONED &&
			(owner == 0 || !ucache_owner_is_dead((uint64_t) (uint32_t) owner, owner_start_time))
		) {
			continue;
		}

		if (!atomic_compare_exchange_strong(&slot->pin_count, &pin_count, 0)) {
			continue;
		}

		ucache_atomic_cas_64(&slot->owner_start_time, owner_start_time, 0);

		atomic_compare_exchange_strong(&slot->owner_pid, &owner, 0);

		hdr->graph_dead_pin_owners_reclaimed++;
	}
}

#ifdef ZEND_WIN32
uint32_t ucache_win32_sleep_us(uint32_t interval_us)
{
	zend_hrtime_t start;
	DWORD interval_ms = interval_us / 1000U;
	uint64_t elapsed_us;

	if (interval_ms == 0) {
		interval_ms = 1;
	}

	start = zend_hrtime();

	Sleep(interval_ms);

	elapsed_us = (zend_hrtime() - start) / 1000U;

	return (uint32_t) MIN(MAX(elapsed_us, (uint64_t) interval_ms * 1000U), UINT32_MAX);
}

uint64_t ucache_win32_proc_start_time_token(uint64_t pid)
{
	FILETIME creation, exit_time, kernel_time, user_time;
	HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, (DWORD) pid);
	uint64_t start_time = 0;

	if (process == NULL) {
		return 0;
	}

	if (GetProcessTimes(process, &creation, &exit_time, &kernel_time, &user_time)) {
		start_time = ((uint64_t) creation.dwHighDateTime << 32) | creation.dwLowDateTime;
	}

	CloseHandle(process);

	return start_time;
}

bool ucache_win32_proc_has_exited(uint64_t pid)
{
	HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, (DWORD) pid);
	DWORD exit_code = 0;

	if (process == NULL) {
		return GetLastError() == ERROR_INVALID_PARAMETER;
	}

	if (GetExitCodeProcess(process, &exit_code) && exit_code != STILL_ACTIVE) {
		CloseHandle(process);

		return true;
	}

	CloseHandle(process);

	return false;
}

int ucache_win32_alloc_err_code(void)
{
	return (int) GetLastError();
}

void ucache_win32_log_alloc_failure(const char *failure, const char *err_in, int err_code)
{
	char *msg = php_win32_error_to_msg(err_code);

	ucache_warn_docref(
		"UserCache: %s: %s: %s (%d)",
		failure,
		err_in != NULL ? err_in : "unknown",
		msg,
		err_code
	);

	php_win32_error_msg_free(msg);
}
#else
uint32_t ucache_posix_sleep_us(uint32_t interval_us)
{
#ifdef HAVE_UNISTD_H
	usleep(interval_us);
#endif

	return interval_us;
}

uint64_t ucache_posix_proc_start_time_token(uint64_t pid)
{
#if defined(__linux__)
	const char *p;
	ssize_t stat_len;
	int fd;
	long long thread_count;
	unsigned long long start_time;
	char state;
	char path[64], stat_buf[1024];

	/* QEMU user mode fakes the start time in a process's own /proc/<pid>/stat but not in its task entry. */
	snprintf(path, sizeof(path), "/proc/%llu/task/%llu/stat", (unsigned long long) pid, (unsigned long long) pid);
	fd = open(path, O_RDONLY);
	if (fd < 0) {
		return 0;
	}

	stat_len = read(fd, stat_buf, sizeof(stat_buf) - 1);

	close(fd);

	if (stat_len <= 0) {
		return 0;
	}

	stat_buf[stat_len] = '\0';

	p = strrchr(stat_buf, ')');
	if (p == NULL) {
		return 0;
	}

	if (sscanf(
			p + 1,
			" %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*u %*d %*d %*d %*d %lld %*d %llu",
			&state,
			&thread_count,
			&start_time
		) != 3
	) {
		return 0;
	}

	if (state == 'X' || (state == 'Z' && thread_count <= 1)) {
		return UCACHE_START_TIME_EXITED;
	}

	return (uint64_t) start_time;
#elif defined(__APPLE__) || defined(__FreeBSD__)
	struct kinfo_proc info;
	size_t size = sizeof(info);
	int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, (int) pid };

	if (sysctl(mib, 4, &info, &size, NULL, 0) != 0 || size < sizeof(info)) {
		return 0;
	}

# ifdef __APPLE__
#  ifdef SZOMB
	if (info.kp_proc.p_stat == SZOMB) {
		return UCACHE_START_TIME_EXITED;
	}
#  endif
	return (((uint64_t) info.kp_proc.p_starttime.tv_sec << 20)
		| (uint64_t) info.kp_proc.p_starttime.tv_usec) + 1
	;
# else
#  ifdef SZOMB
	if (info.ki_stat == SZOMB) {
		return UCACHE_START_TIME_EXITED;
	}
#  endif
	return (((uint64_t) info.ki_start.tv_sec << 20)
		| (uint64_t) info.ki_start.tv_usec) + 1
	;
# endif
#elif defined(__NetBSD__) || defined(__OpenBSD__)
# ifdef __NetBSD__
	struct kinfo_proc2 info;
	int mib[6] = { CTL_KERN, KERN_PROC2, KERN_PROC_PID, (int) pid, (int) sizeof(info), 1 };
# else
	struct kinfo_proc info;
	int mib[6] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, (int) pid, (int) sizeof(info), 1 };
# endif
	size_t size = sizeof(info);

	if (sysctl(mib, 6, &info, &size, NULL, 0) != 0 || size < sizeof(info)) {
		return 0;
	}

# ifdef SZOMB
	if (info.p_stat == SZOMB) {
		return UCACHE_START_TIME_EXITED;
	}
# endif
	return (((uint64_t) info.p_ustart_sec << 20)
		| (uint64_t) info.p_ustart_usec) + 1
	;
#elif defined(__DragonFly__)
	struct kinfo_proc info;
	size_t size = sizeof(info);
	int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, (int) pid };

	if (sysctl(mib, 4, &info, &size, NULL, 0) != 0 || size < sizeof(info)) {
		return 0;
	}

# ifdef SZOMB
	if (info.kp_stat == SZOMB) {
		return UCACHE_START_TIME_EXITED;
	}
# endif
	return (((uint64_t) info.kp_start.tv_sec << 20)
		| (uint64_t) info.kp_start.tv_usec) + 1
	;
#elif defined(__sun)
	psinfo_t info;
	ssize_t info_len;
	int fd;
	char path[64];

	snprintf(path, sizeof(path), "/proc/%llu/psinfo", (unsigned long long) pid);
	fd = open(path, O_RDONLY);
	if (fd < 0) {
		return 0;
	}

	info_len = read(fd, &info, sizeof(info));

	close(fd);

	if (info_len != (ssize_t) sizeof(info)) {
		return 0;
	}

	if (info.pr_nlwp == 0) {
		return UCACHE_START_TIME_EXITED;
	}

	return (((uint64_t) info.pr_start.tv_sec << 20)
		| (uint64_t) (info.pr_start.tv_nsec / 1000)) + 1
	;
#elif defined(_AIX)
	struct procentry64 info;
	pid_t idx = (pid_t) pid;

	if (getprocs64(&info, (int) sizeof(info), NULL, 0, &idx, 1) != 1 || (uint64_t) info.pi_pid != pid) {
		return 0;
	}

# ifdef SZOMB
	if (info.pi_state == SZOMB) {
		return UCACHE_START_TIME_EXITED;
	}
# endif
	return ((uint64_t) info.pi_start << 20) + 1;
#else
	(void) pid;

	return 0;
#endif
}

bool ucache_posix_proc_has_exited(uint64_t pid)
{
	return kill((pid_t) pid, 0) == -1 && errno == ESRCH;
}

int ucache_posix_alloc_err_code(void)
{
	return errno;
}

void ucache_posix_log_alloc_failure(const char *failure, const char *err_in, int err_code)
{
	ucache_warn_docref(
		"UserCache: %s: %s: %s (%d)",
		failure,
		err_in != NULL ? err_in : "unknown",
		strerror(err_code),
		err_code
	);
}
#endif /* ZEND_WIN32 */

bool ucache_recover_after_owner_death_locked(void)
{
	ucache_storage *storage = &ucache_active_ctx()->storage;
	ucache_hdr *hdr = ucache_hdr_ptr();
	ucache_recovered_entry_lock *locks = NULL;
	ucache_reader_slot *slot;
	uint64_t owner_pid, owner_start_time;
	uint32_t lock_count = 0, i;
	size_t table_span;

	if (hdr == NULL || !ucache_hdr_is_initialized_locked()) {
		return true;
	}

	if ((hdr->write_seq & 1) == 0) {
		if (hdr->entry_lock_table_section_open == 0) {
			return true;
		}

		ucache_seq_announce(&hdr->write_seq, hdr->write_seq + 1);
	}

	ZEND_ASSERT((hdr->write_seq & 1) != 0);

	hdr->entry_lock_table_section_open = 0;

	if (ucache_recovery_blocked_by_live_ref(hdr)) {
		return false;
	}

	if (ucache_entry_lock_layout_sane(storage, hdr) &&
		ucache_data_region_sane(storage, hdr) &&
		!ucache_collect_recovery_entry_locks_locked(hdr, &locks, &lock_count)
	) {
		return false;
	}

	table_span = (size_t) hdr->capacity * UCACHE_TABLE_SLOT_SIZE;
	if (storage->size > sizeof(ucache_hdr) &&
		table_span <= storage->size - sizeof(ucache_hdr)
	) {
		memset(
			ucache_entries_ptr(hdr),
			0,
			(size_t) hdr->capacity * sizeof(ucache_entry)
		);

		ucache_access_stamps_reset(hdr);

		ucache_pool_idx_reset_locked(hdr);
	}

	if (ucache_entry_lock_layout_sane(storage, hdr)) {
		memset(
			ucache_entry_lock_records_ptr(hdr),
			0,
			(size_t) hdr->entry_lock_capacity * sizeof(ucache_entry_lock_record)
		);
	}

	memset(hdr->orphaned_graphs, 0, sizeof(hdr->orphaned_graphs));

	for (i = 0; i < UCACHE_READER_SLOTS; i++) {
		slot = &hdr->reader_slots[i];

		if (ucache_atomic_load_32(&slot->active) == 0) {
			continue;
		}

		owner_start_time = ucache_atomic_load_64(&slot->owner_start_time);
		owner_pid = ucache_atomic_load_64(&slot->owner_pid);

		if (owner_pid != 0 &&
			ucache_owner_is_dead(owner_pid, owner_start_time) &&
			ucache_atomic_load_64(&slot->owner_pid) == owner_pid &&
			ucache_atomic_load_64(&slot->owner_start_time) == owner_start_time
		) {
			ucache_atomic_cas_32(&slot->active, 1, 0);
		}
	}

	ucache_recovery_release_dead_graph_pin_slots(hdr);

	hdr->count = 0;
	hdr->expiring_count = 0;
	hdr->expiry_floor = UCACHE_EXPIRY_FLOOR_NONE;
	hdr->tombstone_count = 0;
	hdr->entry_lock_count = 0;
	hdr->entry_lock_tombstone_count = 0;
	hdr->next_free = 0;
	hdr->free_list_bytes = 0;

	if (ucache_free_bins_layout_sane(storage, hdr)) {
		memset(ucache_free_bins_ptr(hdr), 0, ucache_free_bins_bytes(hdr->free_bin_count));
	}

	hdr->orphaned_graphs_saturated = 0;
#if ZEND_DEBUG
	ucache_debug_reserve_data_below_4g_locked(hdr);
#endif

	ucache_restore_recovery_entry_locks_locked(hdr, locks, lock_count);

	ucache_free_recovery_entry_locks(locks, lock_count);

	ucache_bump_mutation_epoch_locked(hdr);

	ucache_seq_publish(&hdr->write_seq, hdr->write_seq + 1);

	return true;
}

bool ucache_owner_is_dead(uint64_t owner_pid, uint64_t owner_start_time)
{
	ucache_owner_probe *probe = &UC_G(owner_probes)[owner_pid % UCACHE_OWNER_PROBES];
	uint64_t now = ucache_clock_now() / UCACHE_CLOCK_TICKS_PER_SEC;

	if (probe->pid == owner_pid && probe->start_time == owner_start_time && probe->probed_at == now) {
		return probe->dead;
	}

	probe->dead = ucache_owner_is_dead_probe(owner_pid, owner_start_time);
	probe->pid = owner_pid;
	probe->start_time = owner_start_time;
	probe->probed_at = now;

	return probe->dead;
}

uint64_t ucache_self_start_time_token(void)
{
	return ucache_cached_self_start_time_token(ucache_cached_pid());
}
