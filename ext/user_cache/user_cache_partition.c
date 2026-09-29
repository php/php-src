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

#include "user_cache_internal.h"

#include "php_syslog.h"

#define UCACHE_GRAPH_PIN_SLOT_GRANULARITY	64U
#define UCACHE_GRAPH_PIN_SLOTS_PER_PROC	2U

typedef struct _ucache_boundary_partition {
	char *boundary;
	size_t boundary_len;
	zend_ulong boundary_hash;
	php_ucache_partition *partition;
	struct _ucache_boundary_partition *next;
} ucache_boundary_partition;

static ucache_boundary_partition *ucache_boundary_partitions = NULL;
static uint64_t ucache_scope_seq;
static uint32_t ucache_boundary_partition_count = 0;
static bool ucache_boundary_creation_disabled = false;
static atomic_bool ucache_boundary_startup_failed_logged = false;

#ifdef ZTS
MUTEX_T ucache_boundary_partitions_mutex = NULL;
#endif
php_ucache_partition *ucache_partitions = NULL;

static bool ucache_init_partition_ctx(php_ucache_partition *partition, const char *name)
{
	ucache_ctx *ctx = &partition->ctx;

	ctx->storage.lock_file = -1;
	ctx->lock_name = ucache_ctx_state.lock_name;

	if (name != NULL) {
		partition->name = strdup(name);
		if (partition->name == NULL) {
			return false;
		}

		ctx->lock_name = partition->name;
	}

	return true;
}

static php_ucache_partition *ucache_partition_alloc(const char *name)
{
	php_ucache_partition *partition = calloc(1, sizeof(php_ucache_partition));

	if (partition == NULL) {
		return NULL;
	}

	if (!ucache_init_partition_ctx(partition, name)) {
		free(partition);

		return NULL;
	}

	return partition;
}

static void ucache_partition_link(php_ucache_partition *partition)
{
	partition->next = ucache_partitions;
	ucache_partitions = partition;
}

static bool ucache_startup_storage_for_ctx(ucache_ctx *ctx)
{
	ucache_ctx *prev_ctx;

	prev_ctx = ucache_activate_ctx(ctx);

	ucache_reset_runtime();
	if (ucache_active_runtime()->enabled && UC_G(enable)) {
		if (!ucache_startup_storage_before_req()) {
			ucache_restore_ctx(prev_ctx);

			return false;
		}
	}

	ucache_restore_ctx(prev_ctx);

	return true;
}

static bool ucache_format_boundary_key_prefix(char *prefix, size_t prefix_size, size_t *prefix_len)
{
#if !defined(ZEND_WIN32) && defined(HAVE_UNISTD_H)
	int n;

	n = snprintf(
		prefix,
		prefix_size,
		"uid:%ld:gid:%ld:",
		(long) geteuid(),
		(long) getegid()
	);

	if (n < 0 || (size_t) n >= prefix_size) {
		return false;
	}

	*prefix_len = (size_t) n;
#else
	prefix[0] = '\0';
	*prefix_len = 0;
	(void) prefix_size;
#endif

	return true;
}

static char *ucache_build_boundary_key(
		const char *boundary,
		size_t boundary_len,
		size_t *key_len)
{
	size_t prefix_len;
	char *boundary_key, prefix[64];

	if (!ucache_format_boundary_key_prefix(prefix, sizeof(prefix), &prefix_len)) {
		return NULL;
	}

	if (boundary_len > SIZE_MAX - prefix_len - 1) {
		return NULL;
	}

	boundary_key = malloc(prefix_len + boundary_len + 1);
	if (boundary_key == NULL) {
		return NULL;
	}

	memcpy(boundary_key, prefix, prefix_len);
	memcpy(boundary_key + prefix_len, boundary, boundary_len);

	boundary_key[prefix_len + boundary_len] = '\0';

	*key_len = prefix_len + boundary_len;

	return boundary_key;
}

static ucache_boundary_partition *ucache_find_boundary_partition(
		const char *boundary,
		size_t boundary_len,
		zend_ulong boundary_hash)
{
	ucache_boundary_partition *entry;

	for (entry = ucache_boundary_partitions; entry != NULL; entry = entry->next) {
		if (entry->boundary_hash == boundary_hash &&
			entry->boundary_len == boundary_len &&
			memcmp(entry->boundary, boundary, boundary_len) == 0
		) {
			return entry;
		}
	}

	return NULL;
}

static void ucache_log_boundary_partition_limit(void)
{
	char limit_msg[256];

	if (!php_ucache_is_enabled_by_ini()) {
		return;
	}

	snprintf(
		limit_msg,
		sizeof(limit_msg),
		"UserCache boundary partition limit (%u) reached; creation of new partitions has been disabled "
		"for this process; existing partitions remain available",
		UCACHE_MAX_BOUNDARY_PARTITIONS
	);

	ucache_log_err(limit_msg);
}

static ucache_boundary_partition *ucache_create_boundary_partition(
		const char *sapi_prefix,
		const char *boundary,
		size_t boundary_len,
		zend_ulong boundary_hash,
		bool *limit_reached)
{
	ucache_boundary_partition *entry;
	char partition_name[128];

	if (ucache_boundary_creation_disabled) {
		return NULL;
	}

	if (ucache_boundary_partition_count >= UCACHE_MAX_BOUNDARY_PARTITIONS) {
		ucache_boundary_creation_disabled = true;

		*limit_reached = true;

		return NULL;
	}

	entry = calloc(1, sizeof(*entry));
	if (entry == NULL) {
		return NULL;
	}

	entry->boundary = malloc(boundary_len + 1);
	if (entry->boundary == NULL) {
		free(entry);

		return NULL;
	}

	memcpy(entry->boundary, boundary, boundary_len);

	entry->boundary[boundary_len] = '\0';
	entry->boundary_len = boundary_len;
	entry->boundary_hash = boundary_hash;

	snprintf(
		partition_name,
		sizeof(partition_name),
		"%s:boundary:" ZEND_ULONG_FMT ":" ZEND_ULONG_FMT ":%zx",
		sapi_prefix,
		boundary_hash,
		boundary_len > 1 ? zend_inline_hash_func(boundary + 1, boundary_len - 1) : boundary_hash,
		boundary_len
	);

	entry->partition = ucache_partition_alloc(partition_name);
	if (entry->partition == NULL) {
		free(entry->boundary);
		free(entry);

		return NULL;
	}

	ucache_partition_link(entry->partition);

	entry->partition->ctx.boundary_identity = entry->boundary;
	entry->partition->ctx.boundary_identity_len = entry->boundary_len;

	entry->next = ucache_boundary_partitions;

	ucache_boundary_partitions = entry;
	ucache_boundary_partition_count++;

	return entry;
}

static void ucache_activate_req_unavailable(php_ucache_reason reason)
{
	UC_G(active_partition) = NULL;
	UC_G(active_ctx_ptr) = NULL;
	UC_G(req_unavailable_reason) = reason;
	UC_G(runtime_resolved) = false;
}

static php_ucache_partition *ucache_boundary_partition_get(
		const char *sapi_prefix,
		const char *boundary,
		size_t supplied_boundary_len)
{
	ucache_boundary_partition *entry;
	zend_ulong boundary_hash;
	size_t boundary_len;
	char *boundary_key;
	bool limit_reached = false;

	boundary_key = ucache_build_boundary_key(boundary, supplied_boundary_len, &boundary_len);
	if (boundary_key == NULL) {
		return NULL;
	}

	boundary_hash = zend_inline_hash_func(boundary_key, boundary_len);

	ucache_boundary_partitions_lock();

	entry = ucache_find_boundary_partition(boundary_key, boundary_len, boundary_hash);
	if (entry == NULL) {
		entry = ucache_create_boundary_partition(
			sapi_prefix,
			boundary_key,
			boundary_len,
			boundary_hash,
			&limit_reached
		);
		if (entry == NULL) {
			ucache_boundary_partitions_unlock();

			free(boundary_key);

			if (limit_reached) {
				ucache_log_boundary_partition_limit();
			}

			return NULL;
		}
	}

	ucache_boundary_partitions_unlock();

	free(boundary_key);

	return entry->partition;
}

static bool ucache_boundary_partition_startup_storage(php_ucache_partition *partition)
{
	if (php_ucache_partition_startup_storage(partition)) {
		return true;
	}

	if (php_ucache_is_enabled_by_ini() &&
		!atomic_exchange(&ucache_boundary_startup_failed_logged, true)
	) {
		ucache_log_err("UserCache partition startup failed; UserCache will be unavailable");
	}

	return false;
}

static bool ucache_exec_is_live(void)
{
	return UC_G(exec_active) || EG(active) || UC_G(op_depth) != 0;
}

#ifndef ZEND_WIN32
static bool ucache_normalize_absolute_path(const char *path, char *normalized)
{
	const char *component, *end;
	size_t len = 0, size;

	if (path[0] != '/') {
		return false;
	}

	for (component = path; *component != '\0'; component = end) {
		while (*component == '/') {
			component++;
		}

		for (end = component; *end != '\0' && *end != '/'; end++);

		size = (size_t) (end - component);
		if (size == 0 || (size == 1 && component[0] == '.')) {
			continue;
		}

		if (size == 2 && component[0] == '.' && component[1] == '.') {
			while (len > 0 && normalized[len - 1] != '/') {
				len--;
			}

			if (len > 0) {
				len--;
			}

			continue;
		}

		if (size >= MAXPATHLEN - 1 - len) {
			return false;
		}

		normalized[len++] = '/';
		memcpy(normalized + len, component, size);
		len += size;
	}

	if (len == 0) {
		normalized[len++] = '/';
	}

	normalized[len] = '\0';

	return true;
}
#endif

static bool ucache_resolve_boundary_doc_root(const char *doc_root, char *resolved)
{
#ifdef ZEND_WIN32
	DWORD attributes;
	size_t len;
#else
	struct stat st;
#endif

	if (doc_root == NULL || doc_root[0] == '\0') {
		return false;
	}

#ifdef ZEND_WIN32
	if (_fullpath(resolved, doc_root, MAXPATHLEN) == NULL) {
		return false;
	}

	for (len = strlen(resolved); len > 3 && (resolved[len - 1] == '\\' || resolved[len - 1] == '/'); len--) {
		resolved[len - 1] = '\0';
	}

	attributes = GetFileAttributesA(resolved);

	return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
	return ucache_normalize_absolute_path(doc_root, resolved) &&
		stat(resolved, &st) == 0 &&
		S_ISDIR(st.st_mode)
	;
#endif
}

static bool ucache_partition_switch_is_forbidden(void)
{
	return UC_G(persistent_exec) &&
		(!UC_G(in_req_shutdown) || UC_G(exec_poisoned)) &&
		(UC_G(exec_active) || EG(active))
	;
}

static void ucache_poison_exec(void)
{
	UC_G(exec_poisoned) = true;
	UC_G(in_req_shutdown) = true;
	UC_G(runtime_resolved) = false;
}

static void ucache_forget_unwound_ops(void)
{
	if (EG(current_execute_data) == NULL) {
		UC_G(op_depth) = 0;
	}
}

static bool ucache_logical_boundary_is_idle(void)
{
	return UC_G(op_depth) == 0 &&
		!UC_G(lock_held) &&
		UC_G(owned_decode_frame) == NULL &&
		UC_G(decode_restore_queue) == NULL
	;
}

static zend_result ucache_finish_logical_lock_scope(void)
{
	UC_G(logical_req_ending) = true;
	UC_G(runtime_resolved) = false;

	zend_try {
		ucache_release_req_entry_locks();
		ucache_retry_deferred_entry_lock_releases();
	} zend_catch {
		ucache_poison_exec();
		zend_bailout();
	} zend_end_try();

	if (UC_G(entry_lock_table) != NULL || ucache_active_ctx_has_deferred_entry_lock_releases()) {
		ucache_poison_exec();

		return FAILURE;
	}

	UC_G(logical_req_ending) = false;
	UC_G(runtime_resolved) = false;
	UC_G(access_now) = 0;
	UC_G(access_now_touches) = 0;
	UC_G(graph_pin_claim_failed_hdr) = NULL;
	UC_G(reader_claim_failed_hdr) = NULL;

	return SUCCESS;
}

static bool ucache_is_before_first_req(void)
{
	return !php_during_module_startup() && !atomic_load(&ucache_registration_closed);
}

static ZEND_COLD void ucache_sapi_log_warning(const char *format, va_list args)
{
	zend_string *msg, *line;

	if (!PG(log_errors) || !(EG(error_reporting) & E_WARNING) || sapi_module.log_message == NULL) {
		return;
	}

	msg = zend_vstrpprintf(0, format, args);
	line = zend_strpprintf(0, "PHP Warning:  %s", ZSTR_VAL(msg));

	sapi_module.log_message(ZSTR_VAL(line), LOG_WARNING);

	zend_string_release_ex(line, false);
	zend_string_release_ex(msg, false);
}

void ucache_partitions_shutdown(void)
{
	php_ucache_partition *partition, *next;
	ucache_ctx *prev_ctx;

	prev_ctx = UC_G(active_ctx_ptr);

	partition = ucache_partitions;
	while (partition != NULL) {
		next = partition->next;

		ucache_activate_ctx(&partition->ctx);
		ucache_shutdown_storage();
		ucache_reset_runtime();

		free(partition->name);
		free(partition);

		partition = next;
	}

	ucache_partitions = NULL;

	ucache_restore_ctx(prev_ctx);
}

void ucache_boundary_partitions_shutdown(void)
{
	ucache_boundary_partition *entry, *next;

	ucache_boundary_partitions_lock();

	entry = ucache_boundary_partitions;
	while (entry != NULL) {
		next = entry->next;

		free(entry->boundary);
		free(entry);

		entry = next;
	}

	ucache_boundary_partitions = NULL;
	ucache_boundary_partition_count = 0;
	ucache_boundary_creation_disabled = false;

	atomic_store(&ucache_boundary_startup_failed_logged, false);

	ucache_boundary_partitions_unlock();
}

void ucache_restore_exec_preparation(void)
{
	if (UC_G(exec_prepared)) {
		UC_G(active_partition) = UC_G(exec_prev_partition);
		UC_G(active_ctx_ptr) = UC_G(exec_prev_ctx);
		UC_G(req_unavailable_reason)
			= UC_G(exec_prev_unavailable_reason)
		;
	}

	UC_G(exec_prepared) = false;
	UC_G(exec_prev_partition) = NULL;
	UC_G(exec_prev_ctx) = NULL;
	UC_G(exec_prev_unavailable_reason) = PHP_UCACHE_REASON_NONE;
	UC_G(exec_partition) = UC_G(active_partition);
	UC_G(persistent_exec) =
		ucache_runtime_opted_in &&
		ucache_registered_mode == PHP_UCACHE_MODE_PERSISTENT
	;
	UC_G(runtime_resolved) = false;
}

void ucache_boundary_partitions_lock(void)
{
#ifdef ZTS
	if (ucache_boundary_partitions_mutex != NULL) {
		tsrm_mutex_lock(ucache_boundary_partitions_mutex);
	}
#endif
}

void ucache_boundary_partitions_unlock(void)
{
#ifdef ZTS
	if (ucache_boundary_partitions_mutex != NULL) {
		tsrm_mutex_unlock(ucache_boundary_partitions_mutex);
	}
#endif
}

ZEND_COLD void ucache_warn(const char *format, ...)
{
	va_list args;
	zend_string *msg;

	va_start(args, format);

	if (ucache_is_before_first_req()) {
		ucache_sapi_log_warning(format, args);
	} else {
		msg = zend_vstrpprintf(0, format, args);

		zend_error_zstr(E_WARNING, msg);

		zend_string_release_ex(msg, false);
	}

	va_end(args);
}

ZEND_COLD void ucache_warn_docref(const char *format, ...)
{
	va_list args;

	va_start(args, format);

	if (ucache_is_before_first_req()) {
		ucache_sapi_log_warning(format, args);
	} else {
		php_verror(NULL, E_WARNING, format, args);
	}

	va_end(args);
}

ZEND_COLD void ucache_log_err(const char *msg)
{
	if (!ucache_is_before_first_req()) {
		php_log_err(msg);
	} else if (sapi_module.log_message != NULL) {
		sapi_module.log_message(msg, LOG_NOTICE);
	}
}

ZEND_API php_ucache_partition *php_ucache_partition_create(const char *name)
{
	php_ucache_partition *partition = ucache_partition_alloc(name);

	if (partition == NULL) {
		return NULL;
	}

	ucache_boundary_partitions_lock();

	ucache_partition_link(partition);

	ucache_boundary_partitions_unlock();

	return partition;
}

ZEND_API void php_ucache_partition_set_max_procs(php_ucache_partition *partition, uint32_t max_procs)
{
	uint32_t slot_count = (uint32_t) MIN(
		(uint64_t) max_procs * UCACHE_GRAPH_PIN_SLOTS_PER_PROC,
		UCACHE_GRAPH_PIN_SLOTS_MAX
	);

	ZEND_ASSERT(!partition->ctx.storage.initialized);

	partition->ctx.graph_pin_slot_count = MAX(
		ZEND_MM_ALIGNED_SIZE_EX(slot_count, UCACHE_GRAPH_PIN_SLOT_GRANULARITY),
		UCACHE_GRAPH_PIN_SLOT_GRANULARITY
	);
}

ZEND_API bool php_ucache_partition_startup_storage(php_ucache_partition *partition)
{
	php_ucache_partition *prev_partition;
	bool result;

	if (partition == NULL) {
		return true;
	}

	if (ucache_partition_switch_is_forbidden() &&
		partition != (
			UC_G(exec_active)
				? UC_G(exec_partition)
				: UC_G(active_partition)
		)
	) {
		return false;
	}

	prev_partition = UC_G(active_partition);

	UC_G(active_partition) = partition;

	result = ucache_startup_storage_for_ctx(&partition->ctx);

	UC_G(active_partition) = prev_partition;

	return result;
}

ZEND_API bool php_ucache_startup_default_ctx_storage(void)
{
	return ucache_startup_storage_for_ctx(&ucache_ctx_state);
}

ZEND_API void php_ucache_partition_activate(php_ucache_partition *partition)
{
	if (ucache_partition_switch_is_forbidden() &&
		partition != (
			UC_G(exec_active)
				? UC_G(exec_partition)
				: UC_G(active_partition)
		)
	) {
		zend_throw_error(NULL, "Cannot change the UserCache partition during a persistent PHP execution");

		return;
	}

	UC_G(active_partition) = partition;
	UC_G(active_ctx_ptr) = NULL;
	UC_G(req_unavailable_reason) = PHP_UCACHE_REASON_NONE;
	UC_G(runtime_resolved) = false;
}

ZEND_API void php_ucache_activate_boundary_partition_by_id(
		const char *sapi_prefix,
		const char *boundary,
		size_t boundary_len,
		php_ucache_reason failure_reason)
{
	php_ucache_partition *partition = NULL;
	bool storage_started;

	if (ucache_partition_switch_is_forbidden()) {
		zend_throw_error(NULL, "Cannot select a UserCache boundary during a persistent PHP execution");

		return;
	}

	if (boundary != NULL && boundary_len != 0) {
		partition = ucache_boundary_partition_get(sapi_prefix, boundary, boundary_len);
	}

	if (partition == NULL) {
		ucache_activate_req_unavailable(failure_reason);

		return;
	}

	storage_started = ucache_boundary_partition_startup_storage(partition);

	php_ucache_partition_activate(partition);

	if (!storage_started) {
		UC_G(req_unavailable_reason) = PHP_UCACHE_REASON_SHM_INIT_FAILED;
	}
}

ZEND_API void php_ucache_activate_boundary_partition(
		const char *sapi_prefix,
		const char *doc_root,
		php_ucache_reason failure_reason)
{
	char resolved_doc_root[MAXPATHLEN];

	if (!ucache_resolve_boundary_doc_root(doc_root, resolved_doc_root)) {
		php_ucache_activate_boundary_partition_by_id(sapi_prefix, NULL, 0, failure_reason);

		return;
	}

	php_ucache_activate_boundary_partition_by_id(
		sapi_prefix,
		resolved_doc_root,
		strlen(resolved_doc_root),
		failure_reason
	);
}

bool ucache_exec_available(void)
{
	return !UC_G(exec_poisoned) &&
		!UC_G(logical_req_ending) &&
		(
			ucache_registered_mode != PHP_UCACHE_MODE_HOST_MANAGED ||
			UC_G(exec_prepared)
		)
	;
}

ZEND_API zend_result php_ucache_opt_in(php_ucache_mode mode)
{
	if (mode < PHP_UCACHE_MODE_REQ ||
		mode > PHP_UCACHE_MODE_HOST_MANAGED ||
		php_during_module_startup() ||
		!php_get_module_initialized() ||
		atomic_load(&ucache_registration_closed) ||
		ucache_exec_is_live()
	) {
		return FAILURE;
	}

	if (ucache_runtime_opted_in) {
		return ucache_registered_mode == mode ? SUCCESS : FAILURE;
	}

	ucache_registered_mode = mode;

	if (mode == PHP_UCACHE_MODE_REQ) {
		ucache_use_req_method_handlers();
	}

	ucache_runtime_opted_in = true;

	UC_G(persistent_exec) = mode == PHP_UCACHE_MODE_PERSISTENT;
	UC_G(runtime_resolved) = false;

	return SUCCESS;
}

ZEND_API zend_result php_ucache_exec_prepare(
		php_ucache_exec_mode mode,
		php_ucache_partition *partition)
{
	php_ucache_partition *entry;
	bool partition_valid = partition == NULL;

	if (ucache_exec_is_live() || UC_G(in_req_shutdown)) {
		return FAILURE;
	}

	ucache_restore_exec_preparation();

	if (!ucache_runtime_opted_in ||
		(
			mode != PHP_UCACHE_EXEC_REQ &&
			mode != PHP_UCACHE_EXEC_PERSISTENT
		) ||
		(
			ucache_registered_mode == PHP_UCACHE_MODE_REQ &&
			mode != PHP_UCACHE_EXEC_REQ
		) ||
		(
			ucache_registered_mode == PHP_UCACHE_MODE_PERSISTENT &&
			mode != PHP_UCACHE_EXEC_PERSISTENT
		)
	) {
		return FAILURE;
	}

	ucache_boundary_partitions_lock();

	for (entry = ucache_partitions; entry != NULL && !partition_valid; entry = entry->next) {
		partition_valid = entry == partition;
	}

	ucache_boundary_partitions_unlock();

	if (!partition_valid) {
		return FAILURE;
	}

	UC_G(exec_prev_partition) = UC_G(active_partition);
	UC_G(exec_prev_ctx) = UC_G(active_ctx_ptr);
	UC_G(exec_prev_unavailable_reason) = UC_G(req_unavailable_reason);
	UC_G(active_partition) = partition;
	UC_G(active_ctx_ptr) = NULL;
	UC_G(req_unavailable_reason) = PHP_UCACHE_REASON_NONE;
	UC_G(exec_partition) = partition;
	UC_G(exec_prepared) = true;
	UC_G(persistent_exec) = mode == PHP_UCACHE_EXEC_PERSISTENT;
	UC_G(runtime_resolved) = false;

	atomic_store(&ucache_registration_closed, true);

	return SUCCESS;
}

ZEND_API zend_result php_ucache_exec_cancel_prepare(void)
{
	if (ucache_exec_is_live() ||
		UC_G(in_req_shutdown)
	) {
		return FAILURE;
	}

	ucache_restore_exec_preparation();

	return SUCCESS;
}

ZEND_API zend_result php_ucache_logical_req_begin(php_ucache_scope_token *token)
{
	uint64_t prev;

	if (token == NULL ||
		!UC_G(exec_active) ||
		!UC_G(persistent_exec) ||
		!ucache_exec_available() ||
		UC_G(in_req_shutdown) ||
		UC_G(logical_scope_token) != 0 ||
		!ucache_logical_boundary_is_idle()
	) {
		return FAILURE;
	}

	prev = ucache_atomic_load_64(&ucache_scope_seq);
	do {
		if (prev == UINT64_MAX) {
			return FAILURE;
		}

		if (ucache_atomic_cas_64(&ucache_scope_seq, prev, prev + 1)) {
			break;
		}

		prev = ucache_atomic_load_64(&ucache_scope_seq);
	} while (true);

	if (ucache_finish_logical_lock_scope() == FAILURE) {
		return FAILURE;
	}

	UC_G(logical_scope_token) = prev + 1;

	*token = UC_G(logical_scope_token);

	return SUCCESS;
}

ZEND_API zend_result php_ucache_logical_req_end(
		php_ucache_scope_token token,
		php_ucache_scope_outcome outcome)
{
	if (token == 0 ||
		token != UC_G(logical_scope_token) ||
		!UC_G(exec_active) ||
		!UC_G(persistent_exec)
	) {
		return FAILURE;
	}

	if (outcome == PHP_UCACHE_SCOPE_ABORTED) {
		UC_G(logical_scope_token) = 0;

		ucache_poison_exec();
		ucache_forget_unwound_ops();

		return SUCCESS;
	}

	if (outcome != PHP_UCACHE_SCOPE_COMPLETE ||
		UC_G(logical_req_ending) ||
		UC_G(exec_poisoned) ||
		UC_G(in_req_shutdown) ||
		!ucache_logical_boundary_is_idle()
	) {
		return FAILURE;
	}

	if (ucache_finish_logical_lock_scope() == FAILURE) {
		return FAILURE;
	}

	ucache_release_pool_status_snapshots();

	ucache_retry_op_lease_releases();

	ucache_expunge_expired_at_req_end();

	UC_G(logical_scope_token) = 0;

	return SUCCESS;
}
