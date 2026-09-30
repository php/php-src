/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Pratik Bhujel <prateekbhujelpb@gmail.com>                    |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "zend_enum.h"
#include "zend_exceptions.h"
#include "zend_smart_str.h"
#include "php_network.h"
#include "php_poll.h"
#include "php_streams.h"
#include "ext/standard/basic_functions.h"
#include "ext/date/php_time.h"
#include "io_terminal.h"
#include "io_terminal_decl.h"
#include "io_terminal_arginfo.h"

#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#ifdef PHP_WIN32
# include <io.h>
# include <windows.h>
#else
# include <limits.h>
# include <signal.h>
# include <sys/stat.h>
# include <termios.h>
# include <time.h>
# include <unistd.h>
# include <sys/ioctl.h>
#endif

#define PHP_IO_TERMINAL_SEQUENCE_TIMEOUT_MS 25

static inline time_t php_io_terminal_clamp_duration_seconds(uint64_t seconds)
{
	if (sizeof(time_t) < 8) {
		return seconds > (uint64_t) INT32_MAX ? (time_t) INT32_MAX : (time_t) seconds;
	}
	return seconds > (uint64_t) INT64_MAX ? (time_t) INT64_MAX : (time_t) seconds;
}

static inline zend_hrtime_t php_io_terminal_saturating_add_deadline(zend_hrtime_t base_ns, zend_hrtime_t duration_ns)
{
	if (UINT64_MAX - base_ns <= duration_ns) {
		return UINT64_MAX;
	}
	return base_ns + duration_ns;
}

#ifdef PHP_WIN32
typedef HANDLE php_io_terminal_native_stream;
# define PHP_IO_TERMINAL_INVALID_NATIVE_STREAM INVALID_HANDLE_VALUE
# define PHP_IO_TERMINAL_MAX_WAIT_MS (INFINITE - 1)
#else
typedef int php_io_terminal_native_stream;
# define PHP_IO_TERMINAL_INVALID_NATIVE_STREAM (-1)

static inline zend_hrtime_t php_io_terminal_timespec_to_ns(const struct timespec *ts)
{
	if (ts == NULL) {
		return 0;
	}
	if (ts->tv_sec <= 0 && ts->tv_nsec <= 0) {
		return 0;
	}
	if ((uint64_t) ts->tv_sec >= (UINT64_MAX / 1000000000ULL)) {
		return UINT64_MAX;
	}
	zend_hrtime_t sec_ns = (zend_hrtime_t) ts->tv_sec * 1000000000ULL;
	if (UINT64_MAX - sec_ns < (zend_hrtime_t) ts->tv_nsec) {
		return UINT64_MAX;
	}
	return sec_ns + (zend_hrtime_t) ts->tv_nsec;
}

static inline void php_io_terminal_ns_to_timespec(zend_hrtime_t ns, struct timespec *ts)
{
	ts->tv_sec = (time_t) (ns / 1000000000ULL);
	ts->tv_nsec = (long) (ns % 1000000000ULL);
}

typedef struct php_io_terminal_utf8_pending {
	unsigned char bytes[4];
	size_t length;
	size_t expected;
} php_io_terminal_utf8_pending;
#endif

#ifdef PHP_WIN32
typedef struct php_io_terminal_identity {
	php_io_terminal_native_stream stream;
	bool is_console;
} php_io_terminal_identity;
#else
typedef struct php_io_terminal_identity {
	php_io_terminal_native_stream stream;
	dev_t dev;
	bool has_dev;
	pid_t sid;
	bool has_sid;
} php_io_terminal_identity;
#endif

typedef struct php_io_terminal_shared_mode {
	php_io_terminal_identity identity;
	php_io_terminal_native_stream restore_stream;
#ifdef PHP_WIN32
	DWORD saved_mode;
#else
	struct termios saved_mode;
#endif
	uint32_t lease_count;
	struct php_io_terminal_shared_mode *prev;
	struct php_io_terminal_shared_mode *next;
} php_io_terminal_shared_mode;

typedef struct php_io_terminal_mode_token_object {
	php_io_terminal_shared_mode *shared;
	bool valid;
	zend_object std;
} php_io_terminal_mode_token_object;

typedef struct php_io_terminal_object {
	zval input_stream_val;
	zval output_stream_val;
	php_io_terminal_identity identity;
	bool has_identity;
	zend_object *active_mode_token;
	zend_long last_cols;
	zend_long last_rows;
	bool has_last_size;
#ifdef PHP_WIN32
	INPUT_RECORD pending_key;
	WCHAR pending_key_high_surrogate;
	WCHAR pending_high_surrogate;
#else
	php_io_terminal_utf8_pending pending_utf8;
#endif
	zend_object std;
} php_io_terminal_object;

typedef struct php_io_terminal_stream_target {
	php_io_terminal_native_stream native_stream;
	php_stream *php_stream;
	zval *stream_resource;
	bool is_default;
} php_io_terminal_stream_target;

static zend_class_entry *php_io_terminal_exception_ce;
static zend_class_entry *php_io_terminal_key_ce;
static zend_class_entry *php_io_terminal_terminal_size_ce;
static zend_class_entry *php_io_terminal_mode_token_ce;
static zend_class_entry *php_io_terminal_terminal_ce;
static zend_class_entry *php_io_terminal_system_mode_token_ce;
static zend_class_entry *php_io_terminal_system_terminal_ce;
static zend_object_handlers php_io_terminal_mode_token_handlers;
static zend_object_handlers php_io_terminal_object_handlers;
ZEND_TLS php_io_terminal_shared_mode *php_io_terminal_active_shared_modes;

#if !defined(PHP_WIN32)
# if defined(HAVE_PTSNAME_R) || defined(_GNU_SOURCE) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
#  define PHP_IO_TERMINAL_HAVE_PTSNAME_R 1
# endif
# if !defined(PHP_IO_TERMINAL_HAVE_PTSNAME_R) && defined(ZTS)
static MUTEX_T php_io_terminal_ptsname_mutex = NULL;
#  define PHP_IO_TERMINAL_PTSNAME_LOCK() do { if (php_io_terminal_ptsname_mutex) tsrm_mutex_lock(php_io_terminal_ptsname_mutex); } while (0)
#  define PHP_IO_TERMINAL_PTSNAME_UNLOCK() do { if (php_io_terminal_ptsname_mutex) tsrm_mutex_unlock(php_io_terminal_ptsname_mutex); } while (0)
# else
#  define PHP_IO_TERMINAL_PTSNAME_LOCK()
#  define PHP_IO_TERMINAL_PTSNAME_UNLOCK()
# endif

#define PHP_IO_TERMINAL_READ_EOF      (-2)
#define PHP_IO_TERMINAL_READ_ERROR    (-1)
#define PHP_IO_TERMINAL_READ_TIMEOUT    0
#define PHP_IO_TERMINAL_READ_SUCCESS    1
#define PHP_IO_TERMINAL_READ_RESIZE     2

#ifdef SIGWINCH
static volatile sig_atomic_t php_io_terminal_resize_generation = 0;
static unsigned int php_io_terminal_resize_readers = 0;
static struct sigaction php_io_terminal_previous_resize_action;
# ifdef ZTS
static MUTEX_T php_io_terminal_resize_mutex = NULL;
#  define PHP_IO_TERMINAL_RESIZE_LOCK() do { if (php_io_terminal_resize_mutex) tsrm_mutex_lock(php_io_terminal_resize_mutex); } while (0)
#  define PHP_IO_TERMINAL_RESIZE_UNLOCK() do { if (php_io_terminal_resize_mutex) tsrm_mutex_unlock(php_io_terminal_resize_mutex); } while (0)
# else
#  define PHP_IO_TERMINAL_RESIZE_LOCK()
#  define PHP_IO_TERMINAL_RESIZE_UNLOCK()
# endif

static zend_always_inline bool php_io_terminal_is_callable_handler(void (*handler)(int))
{
	if (handler == NULL) {
		return false;
	}
	if (handler == SIG_DFL) {
		return false;
	}
	if (handler == SIG_IGN) {
		return false;
	}
	return true;
}

static void php_io_terminal_sigwinch_handler(int signo, siginfo_t *info, void *context)
{
	(void) signo;

	php_io_terminal_resize_generation = php_io_terminal_resize_generation == SIG_ATOMIC_MAX
		? 0
		: php_io_terminal_resize_generation + 1;

	/* Forward to existing handler if present and not default/ignored */
	if (php_io_terminal_is_callable_handler(php_io_terminal_previous_resize_action.sa_handler)) {
		if (php_io_terminal_previous_resize_action.sa_flags & SA_SIGINFO) {
			php_io_terminal_previous_resize_action.sa_sigaction(signo, info, context);
		} else {
			php_io_terminal_previous_resize_action.sa_handler(signo);
		}
	}
}

static bool php_io_terminal_install_resize_handler(sig_atomic_t *generation)
{
	struct sigaction action;
	bool installed = true;
	sig_atomic_t current_generation;

	PHP_IO_TERMINAL_RESIZE_LOCK();
	current_generation = php_io_terminal_resize_generation;

	if (php_io_terminal_resize_readers == 0) {
		memset(&action, 0, sizeof(action));
		action.sa_sigaction = php_io_terminal_sigwinch_handler;
		action.sa_flags = SA_SIGINFO;
		sigemptyset(&action.sa_mask);

		installed = sigaction(SIGWINCH, &action, &php_io_terminal_previous_resize_action) == 0;
	}

	if (installed) {
		php_io_terminal_resize_readers++;
		*generation = current_generation;
	}

	PHP_IO_TERMINAL_RESIZE_UNLOCK();

	return installed;
}

static void php_io_terminal_restore_resize_handler(void)
{
	PHP_IO_TERMINAL_RESIZE_LOCK();

	if (php_io_terminal_resize_readers > 0 && --php_io_terminal_resize_readers == 0) {
		sigaction(SIGWINCH, &php_io_terminal_previous_resize_action, NULL);
	}

	PHP_IO_TERMINAL_RESIZE_UNLOCK();
}
#endif /* SIGWINCH */
#endif /* !PHP_WIN32 */

#define PHP_IO_TERMINAL_MODE_TOKEN_OBJ_FROM_ZOBJ(_obj) \
	ZEND_CONTAINER_OF(_obj, php_io_terminal_mode_token_object, std)
#define PHP_IO_TERMINAL_MODE_TOKEN_OBJ_FROM_ZV(_zv) \
	PHP_IO_TERMINAL_MODE_TOKEN_OBJ_FROM_ZOBJ(Z_OBJ_P(_zv))

#define PHP_IO_TERMINAL_OBJ_FROM_ZOBJ(_obj) \
	ZEND_CONTAINER_OF(_obj, php_io_terminal_object, std)
#define PHP_IO_TERMINAL_OBJ_FROM_ZV(_zv) \
	PHP_IO_TERMINAL_OBJ_FROM_ZOBJ(Z_OBJ_P(_zv))

static php_io_terminal_native_stream php_io_terminal_native_stream_from_php_stream(php_stream *stream);

static bool php_io_terminal_native_stream_is_valid(php_io_terminal_native_stream stream)
{
#ifdef PHP_WIN32
	return stream != INVALID_HANDLE_VALUE && stream != NULL;
#else
	return stream >= 0;
#endif
}

static bool php_io_terminal_native_stream_is_tty(php_io_terminal_native_stream stream)
{
	if (!php_io_terminal_native_stream_is_valid(stream)) {
		return false;
	}
#ifdef PHP_WIN32
	DWORD mode;
	return GetConsoleMode(stream, &mode) != 0;
#else
	return isatty(stream) == 1;
#endif
}

#ifdef PHP_WIN32
static bool php_io_terminal_get_identity(php_io_terminal_native_stream handle, php_io_terminal_identity *identity)
{
	identity->stream = handle;
	identity->is_console = false;
	if (!php_io_terminal_native_stream_is_valid(handle)) {
		return false;
	}
	DWORD mode;
	if (!GetConsoleMode(handle, &mode)) {
		return false;
	}
	identity->is_console = true;
	return true;
}

static bool php_io_terminal_identities_match(const php_io_terminal_identity *first, const php_io_terminal_identity *second)
{
	return first->is_console && second->is_console;
}
#else
#ifndef PATH_MAX
# define PATH_MAX 1024
#endif

static bool php_io_terminal_get_pty_peer_dev(int fd, dev_t *peer_dev)
{
	char peer_path[PATH_MAX];
	bool got_peer_name = false;

#if defined(PHP_IO_TERMINAL_HAVE_PTSNAME_R)
	if (ptsname_r(fd, peer_path, sizeof(peer_path)) == 0) {
		got_peer_name = true;
	}
#else
	PHP_IO_TERMINAL_PTSNAME_LOCK();
	const char *pts = ptsname(fd);
	if (pts != NULL) {
		size_t len = strlen(pts);
		if (len < sizeof(peer_path)) {
			memcpy(peer_path, pts, len + 1);
			got_peer_name = true;
		}
	}
	PHP_IO_TERMINAL_PTSNAME_UNLOCK();
#endif

	if (got_peer_name && peer_path[0] != '\0') {
		struct stat st;
		if (stat(peer_path, &st) == 0 && S_ISCHR(st.st_mode) && st.st_rdev != 0) {
			*peer_dev = st.st_rdev;
			return true;
		}
	}

	return false;
}

static bool php_io_terminal_get_identity(php_io_terminal_native_stream fd, php_io_terminal_identity *identity)
{
	memset(identity, 0, sizeof(*identity));
	identity->stream = fd;
	identity->sid = -1;

	if (!php_io_terminal_native_stream_is_valid(fd) || isatty(fd) != 1) {
		return false;
	}

	pid_t sid = tcgetsid(fd);
	if (sid > 0) {
		identity->sid = sid;
		identity->has_sid = true;
	}

	dev_t peer_dev = 0;
	if (php_io_terminal_get_pty_peer_dev(fd, &peer_dev)) {
		identity->dev = peer_dev;
		identity->has_dev = true;
		return true;
	}

#if defined(TIOCGDEV)
	unsigned int kdev = 0;
	if (ioctl(fd, TIOCGDEV, &kdev) == 0 && kdev != 0) {
		identity->dev = (dev_t) kdev;
		identity->has_dev = true;
		return true;
	}
#endif

	struct stat st;
	if (fstat(fd, &st) == 0 && S_ISCHR(st.st_mode) && st.st_rdev != 0) {
		struct stat st_tty;
		if (stat("/dev/tty", &st_tty) == 0 && S_ISCHR(st_tty.st_mode) && st.st_rdev == st_tty.st_rdev) {
			/* /dev/tty is an indirect alias identified by its controlling terminal session */
		} else {
			identity->dev = st.st_rdev;
			identity->has_dev = true;
			return true;
		}
	}

	return identity->has_dev || identity->has_sid;
}

static bool php_io_terminal_identities_match(const php_io_terminal_identity *first, const php_io_terminal_identity *second)
{
	if (first->has_dev && second->has_dev) {
		return first->dev == second->dev;
	}

	if (first->has_sid && second->has_sid && first->sid == second->sid) {
		return true;
	}

	return false;
}
#endif

#ifdef PHP_WIN32
static php_io_terminal_native_stream php_io_terminal_dup_stream(php_io_terminal_native_stream handle)
{
	HANDLE current_process = GetCurrentProcess();
	HANDLE new_handle = INVALID_HANDLE_VALUE;

	if (DuplicateHandle(current_process, handle, current_process, &new_handle, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
		return new_handle;
	}

	new_handle = CreateFileW(
		L"CONIN$",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);

	if (new_handle != INVALID_HANDLE_VALUE) {
		return new_handle;
	}

	return INVALID_HANDLE_VALUE;
}

static void php_io_terminal_close_native_stream(php_io_terminal_native_stream stream)
{
	if (stream != INVALID_HANDLE_VALUE && stream != NULL) {
		CloseHandle(stream);
	}
}
#else
static php_io_terminal_native_stream php_io_terminal_dup_stream(php_io_terminal_native_stream fd)
{
	int new_fd = -1;

#if defined(F_DUPFD_CLOEXEC)
	new_fd = fcntl(fd, F_DUPFD_CLOEXEC, 0);
	if (new_fd >= 0) {
		return new_fd;
	}
#endif

	new_fd = dup(fd);
	if (new_fd >= 0) {
#if defined(FD_CLOEXEC)
		int flags = fcntl(new_fd, F_GETFD);
		if (flags >= 0) {
			fcntl(new_fd, F_SETFD, flags | FD_CLOEXEC);
		}
#endif
		return new_fd;
	}

	return -1;
}

static void php_io_terminal_close_native_stream(php_io_terminal_native_stream stream)
{
	if (stream >= 0) {
		close(stream);
	}
}
#endif

static php_io_terminal_shared_mode *php_io_terminal_find_shared_mode(const php_io_terminal_identity *identity)
{
	php_io_terminal_shared_mode *curr = php_io_terminal_active_shared_modes;
	while (curr != NULL) {
		if (php_io_terminal_identities_match(&curr->identity, identity)) {
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}

static bool php_io_terminal_restore_shared_mode(const php_io_terminal_shared_mode *shared)
{
	if (!php_io_terminal_native_stream_is_valid(shared->restore_stream)) {
		return false;
	}

#ifdef PHP_WIN32
	if (!SetConsoleMode(shared->restore_stream, shared->saved_mode)) {
		return false;
	}
	DWORD actual_mode;
	if (GetConsoleMode(shared->restore_stream, &actual_mode) && actual_mode != shared->saved_mode) {
		return false;
	}
#else
	if (tcsetattr(shared->restore_stream, TCSANOW, &shared->saved_mode) != 0) {
		return false;
	}
	struct termios actual_mode;
	if (tcgetattr(shared->restore_stream, &actual_mode) == 0) {
		if ((actual_mode.c_lflag & (ECHO | ICANON)) != (shared->saved_mode.c_lflag & (ECHO | ICANON))) {
			return false;
		}
	}
#endif
	return true;
}

static void php_io_terminal_destroy_shared_mode(php_io_terminal_shared_mode *shared)
{
	if (shared->prev != NULL) {
		shared->prev->next = shared->next;
	} else {
		php_io_terminal_active_shared_modes = shared->next;
	}

	if (shared->next != NULL) {
		shared->next->prev = shared->prev;
	}

	shared->prev = NULL;
	shared->next = NULL;

	if (php_io_terminal_native_stream_is_valid(shared->restore_stream)) {
		php_io_terminal_close_native_stream(shared->restore_stream);
		shared->restore_stream = PHP_IO_TERMINAL_INVALID_NATIVE_STREAM;
	}

	efree(shared);
}

static bool php_io_terminal_release_token_lease(php_io_terminal_mode_token_object *token_obj, const char **error_msg)
{
	php_io_terminal_shared_mode *shared = token_obj->shared;

	if (!token_obj->valid || shared == NULL) {
		if (error_msg != NULL) {
			*error_msg = "Terminal mode token is not active";
		}
		return false;
	}

	if (shared->lease_count > 1) {
		shared->lease_count--;
		token_obj->valid = false;
		token_obj->shared = NULL;
		return true;
	}

	if (!php_io_terminal_restore_shared_mode(shared)) {
		if (error_msg != NULL) {
			*error_msg = "Failed to restore terminal mode";
		}
		return false;
	}

	token_obj->valid = false;
	token_obj->shared = NULL;

	php_io_terminal_destroy_shared_mode(shared);
	return true;
}

static void php_io_terminal_mode_token_free_obj(zend_object *object)
{
	php_io_terminal_mode_token_object *intern = PHP_IO_TERMINAL_MODE_TOKEN_OBJ_FROM_ZOBJ(object);

	if (intern->valid && intern->shared != NULL) {
		if (!php_io_terminal_release_token_lease(intern, NULL)) {
			intern->valid = false;
			intern->shared = NULL;
		}
	}

	zend_object_std_dtor(&intern->std);
}

static zend_object *php_io_terminal_mode_token_create_object(zend_class_entry *ce)
{
	php_io_terminal_mode_token_object *intern = zend_object_alloc(sizeof(*intern), ce);

	intern->shared = NULL;
	intern->valid = false;

	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);
	intern->std.handlers = &php_io_terminal_mode_token_handlers;

	return &intern->std;
}

static void php_io_terminal_create_mode_token(zval *return_value, php_io_terminal_shared_mode *shared)
{
	php_io_terminal_mode_token_object *intern;

	object_init_ex(return_value, php_io_terminal_system_mode_token_ce);
	intern = PHP_IO_TERMINAL_MODE_TOKEN_OBJ_FROM_ZV(return_value);
	intern->shared = shared;
	intern->valid = true;
}

static void php_io_terminal_free_obj(zend_object *object)
{
	php_io_terminal_object *intern = PHP_IO_TERMINAL_OBJ_FROM_ZOBJ(object);

	if (intern->active_mode_token != NULL) {
		OBJ_RELEASE(intern->active_mode_token);
		intern->active_mode_token = NULL;
	}

	zval_ptr_dtor(&intern->input_stream_val);
	zval_ptr_dtor(&intern->output_stream_val);

	zend_object_std_dtor(&intern->std);
}

static zend_object *php_io_terminal_create_object(zend_class_entry *ce)
{
	php_io_terminal_object *intern = zend_object_alloc(sizeof(*intern), ce);

	ZVAL_UNDEF(&intern->input_stream_val);
	ZVAL_UNDEF(&intern->output_stream_val);
	memset(&intern->identity, 0, sizeof(intern->identity));
	intern->has_identity = false;
	intern->active_mode_token = NULL;
	intern->last_cols = 0;
	intern->last_rows = 0;
	intern->has_last_size = false;

#ifdef PHP_WIN32
	memset(&intern->pending_key, 0, sizeof(intern->pending_key));
	intern->pending_key_high_surrogate = 0;
	intern->pending_high_surrogate = 0;
#else
	memset(&intern->pending_utf8, 0, sizeof(intern->pending_utf8));
#endif

	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);
	intern->std.handlers = &php_io_terminal_object_handlers;

	return &intern->std;
}


static php_io_terminal_native_stream php_io_terminal_native_stream_from_php_stream(php_stream *stream)
{
#ifdef PHP_WIN32
	php_socket_t descriptor = (php_socket_t) -1;
	intptr_t os_handle;

	if (php_stream_cast(stream, PHP_STREAM_AS_FD | PHP_STREAM_CAST_INTERNAL, (void **) &descriptor, 0) != SUCCESS) {
		return INVALID_HANDLE_VALUE;
	}

	if (descriptor == INVALID_SOCKET || (uintptr_t) descriptor > INT_MAX) {
		return INVALID_HANDLE_VALUE;
	}

	os_handle = _get_osfhandle((int) descriptor);
	return os_handle == -1 ? INVALID_HANDLE_VALUE : (HANDLE) os_handle;
#else
	php_socket_t descriptor = (php_socket_t) -1;

	if (php_stream_cast(stream, PHP_STREAM_AS_FD | PHP_STREAM_CAST_INTERNAL, (void **) &descriptor, 0) != SUCCESS) {
		return -1;
	}

	if (descriptor < 0 || descriptor > INT_MAX) {
		return -1;
	}

	return (int) descriptor;
#endif
}

static bool php_io_terminal_stream_target_init_ex(
	zval *stream_arg,
	bool is_input,
	bool quiet,
	php_io_terminal_stream_target *target
)
{
	memset(target, 0, sizeof(*target));

	if (stream_arg == NULL || Z_ISUNDEF_P(stream_arg) || Z_TYPE_P(stream_arg) == IS_NULL) {
		target->is_default = true;
#ifdef PHP_WIN32
		target->native_stream = is_input ? GetStdHandle(STD_INPUT_HANDLE) : GetStdHandle(STD_OUTPUT_HANDLE);
#else
		target->native_stream = is_input ? STDIN_FILENO : STDOUT_FILENO;
#endif
		if (is_input) {
			zval *stdin_zv = zend_get_constant_str("STDIN", sizeof("STDIN") - 1);
			if (stdin_zv != NULL && Z_TYPE_P(stdin_zv) == IS_RESOURCE) {
				target->php_stream = (php_stream *) zend_fetch_resource2(
					Z_RES_P(stdin_zv),
					NULL,
					php_file_le_stream(),
					php_file_le_pstream()
				);
				if (target->php_stream != NULL) {
					target->stream_resource = stdin_zv;
				}
			}
		} else {
			zval *stdout_zv = zend_get_constant_str("STDOUT", sizeof("STDOUT") - 1);
			if (stdout_zv != NULL && Z_TYPE_P(stdout_zv) == IS_RESOURCE) {
				target->php_stream = (php_stream *) zend_fetch_resource2(
					Z_RES_P(stdout_zv),
					NULL,
					php_file_le_stream(),
					php_file_le_pstream()
				);
				if (target->php_stream != NULL) {
					target->stream_resource = stdout_zv;
				}
			}
		}
		return true;
	}

	if (Z_TYPE_P(stream_arg) == IS_RESOURCE) {
		target->php_stream = (php_stream *) zend_fetch_resource2(
			Z_RES_P(stream_arg),
			quiet ? NULL : "stream",
			php_file_le_stream(),
			php_file_le_pstream()
		);
		if (target->php_stream == NULL) {
			return false;
		}

		target->is_default = false;
		target->stream_resource = stream_arg;
		target->native_stream = php_io_terminal_native_stream_from_php_stream(target->php_stream);
		return true;
	}

	return false;
}

static zend_always_inline bool php_io_terminal_stream_target_init(
	zval *stream_arg,
	bool is_input,
	php_io_terminal_stream_target *target
)
{
	return php_io_terminal_stream_target_init_ex(stream_arg, is_input, false, target);
}

#ifdef PHP_WIN32
static DWORD php_io_terminal_make_raw_mode(DWORD mode)
{
	return (mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT)) | ENABLE_WINDOW_INPUT;
}

static php_io_terminal_shared_mode *php_io_terminal_acquire_raw_mode(
	php_io_terminal_native_stream handle,
	const php_io_terminal_identity *identity
)
{
	php_io_terminal_shared_mode *shared = php_io_terminal_find_shared_mode(identity);
	if (shared != NULL) {
		DWORD current_mode;
		if (GetConsoleMode(handle, &current_mode) && (current_mode & (ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT)) != 0) {
			DWORD raw = php_io_terminal_make_raw_mode(current_mode);
			SetConsoleMode(handle, raw);
		}
		shared->lease_count++;
		return shared;
	}

	DWORD mode;
	if (!php_io_terminal_native_stream_is_valid(handle) || !GetConsoleMode(handle, &mode)) {
		return NULL;
	}

	php_io_terminal_native_stream restore_stream = php_io_terminal_dup_stream(handle);
	if (!php_io_terminal_native_stream_is_valid(restore_stream)) {
		return NULL;
	}

	DWORD raw_mode = php_io_terminal_make_raw_mode(mode);
	if (!SetConsoleMode(handle, raw_mode)) {
		php_io_terminal_close_native_stream(restore_stream);
		return NULL;
	}

	DWORD actual_mode;
	if (GetConsoleMode(handle, &actual_mode) && (actual_mode & (ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT)) != 0) {
		php_io_terminal_close_native_stream(restore_stream);
		return NULL;
	}

	shared = emalloc(sizeof(*shared));
	shared->identity = *identity;
	shared->restore_stream = restore_stream;
	shared->saved_mode = mode;
	shared->lease_count = 1;
	shared->prev = NULL;
	shared->next = php_io_terminal_active_shared_modes;
	if (php_io_terminal_active_shared_modes != NULL) {
		php_io_terminal_active_shared_modes->prev = shared;
	}
	php_io_terminal_active_shared_modes = shared;

	return shared;
}

static bool php_io_terminal_stream_size(php_io_terminal_native_stream handle, zend_long *columns, zend_long *rows)
{
	CONSOLE_SCREEN_BUFFER_INFO info;

	if (!php_io_terminal_native_stream_is_valid(handle) || !GetConsoleScreenBufferInfo(handle, &info)) {
		return false;
	}

	*columns = (zend_long) (info.srWindow.Right - info.srWindow.Left + 1);
	*rows = (zend_long) (info.srWindow.Bottom - info.srWindow.Top + 1);

	return true;
}

static zend_string *php_io_terminal_key_from_virtual_key(WORD vk)
{
	switch (vk) {
		case VK_UP:
			return ZSTR_INIT_LITERAL("up", false);
		case VK_DOWN:
			return ZSTR_INIT_LITERAL("down", false);
		case VK_RIGHT:
			return ZSTR_INIT_LITERAL("right", false);
		case VK_LEFT:
			return ZSTR_INIT_LITERAL("left", false);
		case VK_RETURN:
			return ZSTR_INIT_LITERAL("enter", false);
		case VK_BACK:
			return ZSTR_INIT_LITERAL("backspace", false);
		case VK_ESCAPE:
			return ZSTR_INIT_LITERAL("escape", false);
		case VK_TAB:
			return ZSTR_INIT_LITERAL("tab", false);
		case VK_HOME:
			return ZSTR_INIT_LITERAL("home", false);
		case VK_END:
			return ZSTR_INIT_LITERAL("end", false);
		case VK_DELETE:
			return ZSTR_INIT_LITERAL("delete", false);
		case VK_PRIOR:
			return ZSTR_INIT_LITERAL("pageup", false);
		case VK_NEXT:
			return ZSTR_INIT_LITERAL("pagedown", false);
		case VK_F1:
			return ZSTR_INIT_LITERAL("f1", false);
		case VK_F2:
			return ZSTR_INIT_LITERAL("f2", false);
		case VK_F3:
			return ZSTR_INIT_LITERAL("f3", false);
		case VK_F4:
			return ZSTR_INIT_LITERAL("f4", false);
		case VK_F5:
			return ZSTR_INIT_LITERAL("f5", false);
		case VK_F6:
			return ZSTR_INIT_LITERAL("f6", false);
		case VK_F7:
			return ZSTR_INIT_LITERAL("f7", false);
		case VK_F8:
			return ZSTR_INIT_LITERAL("f8", false);
		case VK_F9:
			return ZSTR_INIT_LITERAL("f9", false);
		case VK_F10:
			return ZSTR_INIT_LITERAL("f10", false);
		case VK_F11:
			return ZSTR_INIT_LITERAL("f11", false);
		case VK_F12:
			return ZSTR_INIT_LITERAL("f12", false);
		default:
			return NULL;
	}
}

static zend_string *php_io_terminal_key_from_wchar(WCHAR ch, WCHAR *high_surrogate)
{
	char buffer[8];
	WCHAR units[2];
	int units_len = 0;
	int buffer_len;

	if (ch == 0) {
		*high_surrogate = 0;
		return NULL;
	}

	if (ch >= 0xd800 && ch <= 0xdbff) {
		*high_surrogate = ch;
		return NULL;
	}

	if (ch >= 0xdc00 && ch <= 0xdfff) {
		if (*high_surrogate >= 0xd800 && *high_surrogate <= 0xdbff) {
			units[0] = *high_surrogate;
			units[1] = ch;
			units_len = 2;
		} else {
			*high_surrogate = 0;
			return NULL;
		}
	} else {
		units[0] = ch;
		units_len = 1;
	}

	*high_surrogate = 0;
	buffer_len = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, units, units_len, buffer, sizeof(buffer), NULL, NULL);
	return buffer_len > 0 ? zend_string_init(buffer, (size_t) buffer_len, false) : NULL;
}

static zend_string *php_io_terminal_key_from_input_record(const KEY_EVENT_RECORD *key, WCHAR *high_surrogate)
{
	zend_string *named_key = php_io_terminal_key_from_virtual_key(key->wVirtualKeyCode);
	if (named_key != NULL) {
		*high_surrogate = 0;
		return named_key;
	}
	return php_io_terminal_key_from_wchar(key->uChar.UnicodeChar, high_surrogate);
}

static bool php_io_terminal_read_console_record(
	HANDLE handle,
	DWORD wait_ms,
	INPUT_RECORD *record,
	WCHAR *high_surrogate,
	INPUT_RECORD *pending_key,
	WCHAR *pending_key_high_surrogate
)
{
	if (pending_key->Event.KeyEvent.wRepeatCount > 0) {
		*record = *pending_key;
		*high_surrogate = *pending_key_high_surrogate;
		pending_key->Event.KeyEvent.wRepeatCount = 0;
		return true;
	}

	DWORD wait_res = WaitForSingleObject(handle, wait_ms);
	if (wait_res == WAIT_TIMEOUT) {
		return false;
	}
	if (wait_res != WAIT_OBJECT_0) {
		zend_throw_exception(php_io_terminal_exception_ce, "Failed to wait on console input", 0);
		return false;
	}

	DWORD records_read;
	if (!ReadConsoleInputW(handle, record, 1, &records_read) || records_read != 1) {
		zend_throw_exception(php_io_terminal_exception_ce, "Failed to read console input", 0);
		return false;
	}

	return true;
}

static zend_string *php_io_terminal_read_stream_key(
	php_io_terminal_native_stream input,
	php_stream *stream,
	DWORD wait_ms,
	WCHAR *pending_high_surrogate,
	INPUT_RECORD *pending_key,
	WCHAR *pending_key_high_surrogate
)
{
	HANDLE handle = input;
	DWORD mode = 0;
	WCHAR high_surrogate = *pending_high_surrogate;
	ULONGLONG deadline_ms = wait_ms == INFINITE ? 0 : GetTickCount64() + wait_ms;
	zend_string *result = NULL;
	bool mode_changed = false;

	if (handle == INVALID_HANDLE_VALUE || handle == NULL
		|| (stream != NULL && stream->writepos > stream->readpos)) {
		zend_throw_exception(php_io_terminal_exception_ce, "Failed to read key: input stream is not a terminal", 0);
		return NULL;
	}

	if (!GetConsoleMode(handle, &mode)) {
		zend_throw_exception(php_io_terminal_exception_ce, "Failed to read key: input stream is not a terminal", 0);
		return NULL;
	}

	DWORD raw_mode = php_io_terminal_make_raw_mode(mode) | ENABLE_WINDOW_INPUT;
	if (raw_mode != mode && !SetConsoleMode(handle, raw_mode)) {
		zend_throw_exception(php_io_terminal_exception_ce, "Failed to set console mode", 0);
		return NULL;
	}
	mode_changed = raw_mode != mode;

	while (true) {
		INPUT_RECORD record;
		DWORD remaining_ms = INFINITE;

		if (wait_ms != INFINITE) {
			ULONGLONG now = GetTickCount64();
			remaining_ms = now >= deadline_ms ? 0 : (DWORD) (deadline_ms - now);
		}

		if (!php_io_terminal_read_console_record(
				handle,
				remaining_ms,
				&record,
				&high_surrogate,
				pending_key,
				pending_key_high_surrogate)) {
			break;
		}

		if (record.EventType == WINDOW_BUFFER_SIZE_EVENT) {
			result = ZSTR_INIT_LITERAL("resize", false);
			break;
		}

		if (record.EventType == KEY_EVENT) {
			KEY_EVENT_RECORD *key = &record.Event.KeyEvent;
			if (!key->bKeyDown) {
				continue;
			}

			WCHAR prev_high = high_surrogate;
			result = php_io_terminal_key_from_input_record(key, &high_surrogate);
			if (result != NULL) {
				if (key->wRepeatCount > 1) {
					key->wRepeatCount--;
					*pending_key = record;
					*pending_key_high_surrogate = prev_high;
				}
				break;
			}
		}
	}

	*pending_high_surrogate = high_surrogate;

	if (mode_changed && !SetConsoleMode(handle, mode)) {
		if (result != NULL) {
			zend_string_release(result);
		}
		if (!EG(exception)) {
			zend_throw_exception(php_io_terminal_exception_ce, "Failed to restore console mode", 0);
		}
		return NULL;
	}

	return result;
}

static zend_string *php_io_terminal_read_stream_secret(
	php_io_terminal_native_stream input,
	php_stream *stream,
	INPUT_RECORD *pending_key,
	WCHAR *pending_key_high_surrogate,
	DWORD wait_ms,
	bool has_timeout,
	bool *timed_out
)
{
	*timed_out = false;
	HANDLE handle = input;
	DWORD mode;

	if (handle == INVALID_HANDLE_VALUE || handle == NULL
		|| (stream != NULL && stream->writepos > stream->readpos)
		|| !GetConsoleMode(handle, &mode)) {
		return NULL;
	}

	DWORD raw_mode = php_io_terminal_make_raw_mode(mode);
	if (raw_mode != mode && !SetConsoleMode(handle, raw_mode)) {
		return NULL;
	}

	bool mode_changed = raw_mode != mode;
	WCHAR high_surrogate = 0;
	smart_str secret = {0};
	bool success = false;
	bool failed = false;

	zend_hrtime_t overall_deadline_ns = 0;
	if (has_timeout) {
		overall_deadline_ns = php_io_terminal_saturating_add_deadline(
			zend_hrtime(),
			((zend_hrtime_t) wait_ms * 1000000ULL)
		);
	}

	while (true) {
		INPUT_RECORD record;
		DWORD current_wait = INFINITE;

		if (has_timeout) {
			zend_hrtime_t now = zend_hrtime();
			if (now >= overall_deadline_ns) {
				current_wait = 0;
			} else {
				zend_hrtime_t remaining_ns = overall_deadline_ns - now;
				uint64_t ms = (remaining_ns + 999999ULL) / 1000000ULL;
				current_wait = ms >= (DWORD) PHP_IO_TERMINAL_MAX_WAIT_MS ? PHP_IO_TERMINAL_MAX_WAIT_MS : (DWORD) ms;
			}
		}

		if (!php_io_terminal_read_console_record(
				handle,
				current_wait,
				&record,
				&high_surrogate,
				pending_key,
				pending_key_high_surrogate)) {
			if (has_timeout && (current_wait == 0 || zend_hrtime() >= overall_deadline_ns)) {
				*timed_out = true;
			}
			failed = true;
			break;
		}

		if (record.EventType != KEY_EVENT) {
			continue;
		}

		KEY_EVENT_RECORD *key = &record.Event.KeyEvent;
		if (!key->bKeyDown) {
			continue;
		}

		if (key->wVirtualKeyCode == VK_RETURN) {
			success = true;
			break;
		}

		if (key->wVirtualKeyCode == VK_ESCAPE
			|| ((key->wVirtualKeyCode == 'C' || key->wVirtualKeyCode == 'D')
				&& (key->dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) != 0)) {
			failed = true;
			break;
		}

		WORD repeats = key->wRepeatCount > 0 ? key->wRepeatCount : 1;
		while (repeats-- > 0) {
			if (key->wVirtualKeyCode == VK_BACK) {
				if (secret.s != NULL && ZSTR_LEN(secret.s) > 0) {
					size_t len = ZSTR_LEN(secret.s);
					while (len > 0) {
						unsigned char byte = (unsigned char) ZSTR_VAL(secret.s)[len - 1];
						len--;
						if ((byte & 0xc0) != 0x80) {
							break;
						}
					}
					ZSTR_LEN(secret.s) = len;
				}
				continue;
			}

			if (key->uChar.UnicodeChar != 0) {
				zend_string *utf8 = php_io_terminal_key_from_wchar(key->uChar.UnicodeChar, &high_surrogate);
				if (utf8 != NULL) {
					smart_str_append(&secret, utf8);
					zend_string_release(utf8);
				}
			}
		}
	}

	if (mode_changed && !SetConsoleMode(handle, mode)) {
		failed = true;
		*timed_out = false;
	}

	if (success && !failed) {
		return smart_str_extract(&secret);
	}

	smart_str_free(&secret);
	return NULL;
}
#else
/* POSIX implementation */

static void php_io_terminal_make_raw_mode(struct termios *mode)
{
	mode->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	mode->c_oflag &= ~OPOST;
	mode->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	mode->c_cflag &= ~(CSIZE | PARENB);
	mode->c_cflag |= CS8;
	mode->c_cc[VMIN] = 1;
	mode->c_cc[VTIME] = 0;
}

static php_io_terminal_shared_mode *php_io_terminal_acquire_raw_mode(
	php_io_terminal_native_stream fd,
	const php_io_terminal_identity *identity
)
{
	php_io_terminal_shared_mode *shared = php_io_terminal_find_shared_mode(identity);
	if (shared != NULL) {
		struct termios current_mode;
		if (tcgetattr(fd, &current_mode) == 0 && (current_mode.c_lflag & (ECHO | ICANON)) != 0) {
			struct termios raw = current_mode;
			php_io_terminal_make_raw_mode(&raw);
			tcsetattr(fd, TCSANOW, &raw);
		}
		shared->lease_count++;
		return shared;
	}

	struct termios mode;
	if (!php_io_terminal_native_stream_is_valid(fd) || isatty(fd) != 1 || tcgetattr(fd, &mode) != 0) {
		return NULL;
	}

	php_io_terminal_native_stream restore_stream = php_io_terminal_dup_stream(fd);
	if (!php_io_terminal_native_stream_is_valid(restore_stream)) {
		return NULL;
	}

	struct termios raw_mode = mode;
	php_io_terminal_make_raw_mode(&raw_mode);

	if (tcsetattr(fd, TCSANOW, &raw_mode) != 0) {
		php_io_terminal_close_native_stream(restore_stream);
		return NULL;
	}

	struct termios actual_mode;
	if (tcgetattr(fd, &actual_mode) == 0 && (actual_mode.c_lflag & (ECHO | ICANON)) != 0) {
		php_io_terminal_close_native_stream(restore_stream);
		return NULL;
	}

	shared = emalloc(sizeof(*shared));
	shared->identity = *identity;
	shared->restore_stream = restore_stream;
	shared->saved_mode = mode;
	shared->lease_count = 1;
	shared->prev = NULL;
	shared->next = php_io_terminal_active_shared_modes;
	if (php_io_terminal_active_shared_modes != NULL) {
		php_io_terminal_active_shared_modes->prev = shared;
	}
	php_io_terminal_active_shared_modes = shared;

	return shared;
}

static bool php_io_terminal_stream_size(php_io_terminal_native_stream fd, zend_long *columns, zend_long *rows)
{
	struct winsize ws;

	if (!php_io_terminal_native_stream_is_valid(fd)) {
		return false;
	}

	if (ioctl(fd, TIOCGWINSZ, &ws) != 0 || ws.ws_col == 0 || ws.ws_row == 0) {
		return false;
	}

	*columns = (zend_long) ws.ws_col;
	*rows = (zend_long) ws.ws_row;

	return true;
}

static size_t php_io_terminal_utf8_sequence_len(unsigned char byte)
{
	if ((byte & 0x80) == 0) {
		return 1;
	}
	if ((byte & 0xe0) == 0xc0) {
		return 2;
	}
	if ((byte & 0xf0) == 0xe0) {
		return 3;
	}
	if ((byte & 0xf8) == 0xf0) {
		return 4;
	}
	return 1;
}

static void php_io_terminal_buffer_remove_last_utf8_char(smart_str *secret)
{
	size_t len;

	if (secret->s == NULL || ZSTR_LEN(secret->s) == 0) {
		return;
	}

	len = ZSTR_LEN(secret->s);
	while (len > 0) {
		unsigned char byte = (unsigned char) ZSTR_VAL(secret->s)[len - 1];
		len--;
		if ((byte & 0xc0) != 0x80) {
			break;
		}
	}

	ZSTR_LEN(secret->s) = len;
}

static bool php_io_terminal_timespec_subtract_elapsed(struct timespec *remaining, zend_hrtime_t elapsed_ns)
{
	uint64_t elapsed_seconds = elapsed_ns / ZEND_NANO_IN_SEC;
	uint64_t elapsed_nanoseconds = elapsed_ns % ZEND_NANO_IN_SEC;

	if (elapsed_seconds > (uint64_t) remaining->tv_sec
		|| (elapsed_seconds == (uint64_t) remaining->tv_sec
			&& elapsed_nanoseconds >= (uint64_t) remaining->tv_nsec)) {
		remaining->tv_sec = 0;
		remaining->tv_nsec = 0;
		return false;
	}

	remaining->tv_sec -= (time_t) elapsed_seconds;
	if ((uint64_t) remaining->tv_nsec < elapsed_nanoseconds) {
		remaining->tv_sec--;
		remaining->tv_nsec += ZEND_NANO_IN_SEC;
	}
	remaining->tv_nsec -= (long) elapsed_nanoseconds;

	return true;
}

static php_poll_ctx *php_io_terminal_create_poll_context(int fd)
{
	php_poll_ctx *poll_ctx = php_poll_create(PHP_POLL_BACKEND_AUTO, 0);

	if (poll_ctx == NULL) {
		return NULL;
	}

	if (php_poll_set_max_events_hint(poll_ctx, 1) == FAILURE
		|| php_poll_init(poll_ctx) == FAILURE
		|| php_poll_add(poll_ctx, fd, PHP_POLL_READ, NULL) == FAILURE) {
		php_poll_destroy(poll_ctx);
		return NULL;
	}

	return poll_ctx;
}

static int php_io_terminal_read_byte(
	int fd,
	php_stream *stream,
	php_poll_ctx *poll_ctx,
	unsigned char *byte,
	const struct timespec *timeout,
	bool allow_resize,
	const sig_atomic_t *generation
)
{
	if (stream != NULL && stream->writepos > stream->readpos) {
		size_t read_bytes = php_stream_read(stream, (char *) byte, 1);
		if (read_bytes == 1) {
			return PHP_IO_TERMINAL_READ_SUCCESS;
		}
		return php_stream_eof(stream) ? PHP_IO_TERMINAL_READ_EOF : PHP_IO_TERMINAL_READ_ERROR;
	}

	if (poll_ctx == NULL) {
		return PHP_IO_TERMINAL_READ_ERROR;
	}

	struct timespec remaining;
	if (timeout != NULL) {
		remaining = *timeout;
	}

	while (true) {
		php_poll_event event;
		const struct timespec *wait_timeout = NULL;
		zend_hrtime_t start_ns = 0;

#if defined(SIGWINCH)
		if (allow_resize && generation != NULL && *generation != php_io_terminal_resize_generation) {
			return PHP_IO_TERMINAL_READ_RESIZE;
		}
#endif

		if (timeout != NULL) {
			wait_timeout = &remaining;
			start_ns = zend_hrtime();
		}

		int poll_result = php_poll_wait(poll_ctx, &event, 1, wait_timeout);
		if (poll_result > 0) {
			break;
		}
		if (poll_result == 0) {
			return PHP_IO_TERMINAL_READ_TIMEOUT;
		}

		if (php_poll_get_error(poll_ctx) == PHP_POLL_ERR_INTERRUPTED) {
#if defined(SIGWINCH)
			if (allow_resize && generation != NULL && *generation != php_io_terminal_resize_generation) {
				return PHP_IO_TERMINAL_READ_RESIZE;
			}
#endif
			if (timeout != NULL) {
				zend_hrtime_t elapsed_ns = zend_hrtime() - start_ns;
				if (UNEXPECTED(elapsed_ns == 0)
					|| !php_io_terminal_timespec_subtract_elapsed(&remaining, elapsed_ns)) {
					return PHP_IO_TERMINAL_READ_TIMEOUT;
				}
			}
			continue;
		}

		return PHP_IO_TERMINAL_READ_ERROR;
	}

	ssize_t bytes_read = read(fd, byte, 1);
	if (bytes_read == 0) {
		return PHP_IO_TERMINAL_READ_EOF;
	}
	if (bytes_read < 0) {
		return PHP_IO_TERMINAL_READ_ERROR;
	}

	return PHP_IO_TERMINAL_READ_SUCCESS;
}

static zend_string *php_io_terminal_finish_utf8_sequence(
	int fd,
	php_stream *stream,
	php_poll_ctx *poll_ctx,
	php_io_terminal_utf8_pending *pending,
	bool has_overall_deadline,
	zend_hrtime_t overall_deadline_ns
)
{
	while (pending->length < pending->expected) {
		unsigned char key;
		struct timespec current_timeout;
		const struct timespec *timeout = NULL;

		if (has_overall_deadline) {
			zend_hrtime_t now = zend_hrtime();
			if (now >= overall_deadline_ns) {
				current_timeout.tv_sec = 0;
				current_timeout.tv_nsec = 0;
			} else {
				php_io_terminal_ns_to_timespec(overall_deadline_ns - now, &current_timeout);
			}
			timeout = &current_timeout;
		}

		int result = php_io_terminal_read_byte(fd, stream, poll_ctx, &key, timeout, false, NULL);
		if (result == PHP_IO_TERMINAL_READ_TIMEOUT) {
			return NULL;
		}
		if (result == PHP_IO_TERMINAL_READ_EOF) {
			zend_throw_exception(php_io_terminal_exception_ce, "End of file reached on terminal input stream", 0);
			return NULL;
		}
		if (result != PHP_IO_TERMINAL_READ_SUCCESS) {
			zend_throw_exception(php_io_terminal_exception_ce, "Failed to read from terminal input stream", 0);
			return NULL;
		}

		pending->bytes[pending->length++] = key;
		if ((key & 0xc0) != 0x80) {
			break;
		}
	}

	zend_string *result = zend_string_init((const char *) pending->bytes, pending->length, false);
	memset(pending, 0, sizeof(*pending));
	return result;
}

static zend_string *php_io_terminal_key_from_escape_sequence(
	int fd,
	php_stream *stream,
	php_poll_ctx *poll_ctx,
	const struct timespec *sequence_timeout,
	bool has_overall_deadline,
	zend_hrtime_t overall_deadline_ns
)
{
	unsigned char seq[32];
	size_t seq_len = 0;
	seq[seq_len++] = 0x1b;

	zend_hrtime_t now = zend_hrtime();
	zend_hrtime_t seq_timeout_ns = php_io_terminal_timespec_to_ns(sequence_timeout);
	zend_hrtime_t deadline_ns = php_io_terminal_saturating_add_deadline(now, seq_timeout_ns);
	if (has_overall_deadline && deadline_ns > overall_deadline_ns) {
		deadline_ns = overall_deadline_ns;
	}

	if (now >= deadline_ns) {
		return ZSTR_INIT_LITERAL("escape", false);
	}

	struct timespec remaining_ts;
	php_io_terminal_ns_to_timespec(deadline_ns - now, &remaining_ts);

	int read_result = php_io_terminal_read_byte(fd, stream, poll_ctx, &seq[seq_len], &remaining_ts, false, NULL);
	if (read_result == PHP_IO_TERMINAL_READ_TIMEOUT) {
		return ZSTR_INIT_LITERAL("escape", false);
	}
	if (read_result == PHP_IO_TERMINAL_READ_EOF) {
		zend_throw_exception(php_io_terminal_exception_ce, "End of file reached on terminal input stream", 0);
		return NULL;
	}
	if (read_result != PHP_IO_TERMINAL_READ_SUCCESS) {
		zend_throw_exception(php_io_terminal_exception_ce, "Failed to read from terminal input stream", 0);
		return NULL;
	}
	seq_len++;

	if (seq[1] == '[') {
		while (seq_len < sizeof(seq)) {
			now = zend_hrtime();
			if (now >= deadline_ns) {
				break;
			}
			php_io_terminal_ns_to_timespec(deadline_ns - now, &remaining_ts);

			unsigned char next;
			read_result = php_io_terminal_read_byte(fd, stream, poll_ctx, &next, &remaining_ts, false, NULL);
			if (read_result != PHP_IO_TERMINAL_READ_SUCCESS) {
				if (read_result == PHP_IO_TERMINAL_READ_EOF) {
					zend_throw_exception(php_io_terminal_exception_ce, "End of file reached on terminal input stream", 0);
					return NULL;
				}
				if (read_result == PHP_IO_TERMINAL_READ_ERROR) {
					zend_throw_exception(php_io_terminal_exception_ce, "Failed to read from terminal input stream", 0);
					return NULL;
				}
				break;
			}

			seq[seq_len++] = next;
			if (next >= 0x40 && next <= 0x7e) {
				break;
			}
		}

#define PHP_IO_TERMINAL_CSI_IS(literal) \
	(memcmp(seq, literal, sizeof(literal) - 1) == 0)

		if (seq_len == 3) {
			if (PHP_IO_TERMINAL_CSI_IS("\x1b[A")) return ZSTR_INIT_LITERAL("up", false);
			if (PHP_IO_TERMINAL_CSI_IS("\x1b[B")) return ZSTR_INIT_LITERAL("down", false);
			if (PHP_IO_TERMINAL_CSI_IS("\x1b[C")) return ZSTR_INIT_LITERAL("right", false);
			if (PHP_IO_TERMINAL_CSI_IS("\x1b[D")) return ZSTR_INIT_LITERAL("left", false);
			if (PHP_IO_TERMINAL_CSI_IS("\x1b[H")) return ZSTR_INIT_LITERAL("home", false);
			if (PHP_IO_TERMINAL_CSI_IS("\x1b[F")) return ZSTR_INIT_LITERAL("end", false);
		} else if (seq_len == 4) {
			if (PHP_IO_TERMINAL_CSI_IS("\x1b[1~")) return ZSTR_INIT_LITERAL("home", false);
			if (PHP_IO_TERMINAL_CSI_IS("\x1b[4~")) return ZSTR_INIT_LITERAL("end", false);
			if (PHP_IO_TERMINAL_CSI_IS("\x1b[3~")) return ZSTR_INIT_LITERAL("delete", false);
			if (PHP_IO_TERMINAL_CSI_IS("\x1b[5~")) return ZSTR_INIT_LITERAL("pageup", false);
			if (PHP_IO_TERMINAL_CSI_IS("\x1b[6~")) return ZSTR_INIT_LITERAL("pagedown", false);
		} else if (seq_len == 5) {
			if (PHP_IO_TERMINAL_CSI_IS("\x1b[11~")) return ZSTR_INIT_LITERAL("f1", false);
			if (PHP_IO_TERMINAL_CSI_IS("\x1b[12~")) return ZSTR_INIT_LITERAL("f2", false);
			if (PHP_IO_TERMINAL_CSI_IS("\x1b[13~")) return ZSTR_INIT_LITERAL("f3", false);
			if (PHP_IO_TERMINAL_CSI_IS("\x1b[14~")) return ZSTR_INIT_LITERAL("f4", false);
			if (PHP_IO_TERMINAL_CSI_IS("\x1b[15~")) return ZSTR_INIT_LITERAL("f5", false);
			if (PHP_IO_TERMINAL_CSI_IS("\x1b[17~")) return ZSTR_INIT_LITERAL("f6", false);
			if (PHP_IO_TERMINAL_CSI_IS("\x1b[18~")) return ZSTR_INIT_LITERAL("f7", false);
			if (PHP_IO_TERMINAL_CSI_IS("\x1b[19~")) return ZSTR_INIT_LITERAL("f8", false);
			if (PHP_IO_TERMINAL_CSI_IS("\x1b[20~")) return ZSTR_INIT_LITERAL("f9", false);
			if (PHP_IO_TERMINAL_CSI_IS("\x1b[21~")) return ZSTR_INIT_LITERAL("f10", false);
			if (PHP_IO_TERMINAL_CSI_IS("\x1b[23~")) return ZSTR_INIT_LITERAL("f11", false);
			if (PHP_IO_TERMINAL_CSI_IS("\x1b[24~")) return ZSTR_INIT_LITERAL("f12", false);
		}

#undef PHP_IO_TERMINAL_CSI_IS
	} else if (seq[1] == 'O' && seq_len < sizeof(seq)) {
		now = zend_hrtime();
		if (now < deadline_ns) {
			php_io_terminal_ns_to_timespec(deadline_ns - now, &remaining_ts);
			read_result = php_io_terminal_read_byte(fd, stream, poll_ctx, &seq[seq_len], &remaining_ts, false, NULL);
			if (read_result == PHP_IO_TERMINAL_READ_SUCCESS) {
				seq_len++;
				switch (seq[2]) {
					case 'P': return ZSTR_INIT_LITERAL("f1", false);
					case 'Q': return ZSTR_INIT_LITERAL("f2", false);
					case 'R': return ZSTR_INIT_LITERAL("f3", false);
					case 'S': return ZSTR_INIT_LITERAL("f4", false);
					case 'H': return ZSTR_INIT_LITERAL("home", false);
					case 'F': return ZSTR_INIT_LITERAL("end", false);
				}
			} else if (read_result == PHP_IO_TERMINAL_READ_EOF) {
				zend_throw_exception(php_io_terminal_exception_ce, "End of file reached on terminal input stream", 0);
				return NULL;
			} else if (read_result == PHP_IO_TERMINAL_READ_ERROR) {
				zend_throw_exception(php_io_terminal_exception_ce, "Failed to read from terminal input stream", 0);
				return NULL;
			}
		}
	}

	return zend_string_init((const char *) seq, seq_len, false);
}

static zend_string *php_io_terminal_key_from_utf8_sequence(
	int fd,
	php_stream *stream,
	php_poll_ctx *poll_ctx,
	unsigned char key,
	php_io_terminal_utf8_pending *pending,
	bool has_overall_deadline,
	zend_hrtime_t overall_deadline_ns
)
{
	size_t sequence_len = php_io_terminal_utf8_sequence_len(key);

	if (sequence_len == 1) {
		return ZSTR_CHAR(key);
	}

	memset(pending, 0, sizeof(*pending));
	pending->bytes[0] = key;
	pending->length = 1;
	pending->expected = sequence_len;

	return php_io_terminal_finish_utf8_sequence(
		fd, stream, poll_ctx, pending,
		has_overall_deadline, overall_deadline_ns
	);
}

static zend_string *php_io_terminal_key_from_byte(
	int fd,
	php_stream *stream,
	php_poll_ctx *poll_ctx,
	unsigned char key,
	const struct timespec *sequence_timeout,
	php_io_terminal_utf8_pending *pending,
	bool has_overall_deadline,
	zend_hrtime_t overall_deadline_ns
)
{
	switch (key) {
		case '\r':
		case '\n':
			return ZSTR_INIT_LITERAL("enter", false);
		case '\t':
			return ZSTR_INIT_LITERAL("tab", false);
		case 0x7f:
		case '\b':
			return ZSTR_INIT_LITERAL("backspace", false);
		case 0x1b:
			return php_io_terminal_key_from_escape_sequence(
				fd, stream, poll_ctx, sequence_timeout, has_overall_deadline, overall_deadline_ns
			);
		default:
			return php_io_terminal_key_from_utf8_sequence(
				fd, stream, poll_ctx, key, pending, has_overall_deadline, overall_deadline_ns
			);
	}
}

static zend_string *php_io_terminal_read_stream_key(
	php_io_terminal_native_stream input,
	php_stream *stream,
	const struct timespec *timeout,
	const struct timespec *sequence_timeout,
	php_io_terminal_utf8_pending *pending
)
{
	int fd = input;
	if (fd < 0 || isatty(fd) != 1) {
		zend_throw_exception(php_io_terminal_exception_ce, "Failed to read key: input stream is not a terminal", 0);
		return NULL;
	}

	struct termios mode;
	if (tcgetattr(fd, &mode) != 0) {
		zend_throw_exception(php_io_terminal_exception_ce, "Failed to read key: input stream is not a terminal", 0);
		return NULL;
	}

	php_poll_ctx *poll_ctx = php_io_terminal_create_poll_context(fd);
	if (poll_ctx == NULL) {
		zend_throw_exception(php_io_terminal_exception_ce, "Failed to initialize terminal poll context", 0);
		return NULL;
	}

	sig_atomic_t resize_generation = 0;
#if defined(SIGWINCH)
	bool resize_handler_installed = php_io_terminal_install_resize_handler(&resize_generation);
#else
	bool resize_handler_installed = false;
#endif

	struct termios raw_mode = mode;
	php_io_terminal_make_raw_mode(&raw_mode);
	bool mode_changed = memcmp(&raw_mode, &mode, sizeof(mode)) != 0;

	if (mode_changed && tcsetattr(fd, TCSANOW, &raw_mode) != 0) {
#if defined(SIGWINCH)
		if (resize_handler_installed) {
			php_io_terminal_restore_resize_handler();
		}
#endif
		php_poll_destroy(poll_ctx);
		zend_throw_exception(php_io_terminal_exception_ce, "Failed to set terminal raw mode", 0);
		return NULL;
	}

	bool has_overall_deadline = (timeout != NULL);
	zend_hrtime_t overall_deadline_ns = 0;
	if (has_overall_deadline) {
		overall_deadline_ns = php_io_terminal_saturating_add_deadline(
			zend_hrtime(),
			php_io_terminal_timespec_to_ns(timeout)
		);
	}

	zend_string *result = NULL;

	if (pending->length > 0) {
		result = php_io_terminal_finish_utf8_sequence(
			fd, stream, poll_ctx, pending,
			has_overall_deadline, overall_deadline_ns
		);
	} else {
		unsigned char key;
		int read_result = php_io_terminal_read_byte(fd, stream, poll_ctx, &key, timeout, true,
			resize_handler_installed ? &resize_generation : NULL);
		if (read_result == PHP_IO_TERMINAL_READ_RESIZE) {
			result = ZSTR_INIT_LITERAL("resize", false);
		} else if (read_result == PHP_IO_TERMINAL_READ_SUCCESS) {
			result = php_io_terminal_key_from_byte(
				fd, stream, poll_ctx, key, sequence_timeout, pending,
				has_overall_deadline, overall_deadline_ns
			);
		} else if (read_result == PHP_IO_TERMINAL_READ_EOF) {
			zend_throw_exception(php_io_terminal_exception_ce, "End of file reached on terminal input stream", 0);
		} else if (read_result == PHP_IO_TERMINAL_READ_ERROR) {
			zend_throw_exception(php_io_terminal_exception_ce, "Failed to read from terminal input stream", 0);
		}
	}

	if (mode_changed && tcsetattr(fd, TCSANOW, &mode) != 0) {
#if defined(SIGWINCH)
		if (resize_handler_installed) {
			php_io_terminal_restore_resize_handler();
		}
#endif
		if (result != NULL) {
			zend_string_release(result);
		}
		php_poll_destroy(poll_ctx);
		if (!EG(exception)) {
			zend_throw_exception(php_io_terminal_exception_ce, "Failed to restore terminal mode", 0);
		}
		return NULL;
	}

#if defined(SIGWINCH)
	if (resize_handler_installed) {
		php_io_terminal_restore_resize_handler();
	}
#endif

	php_poll_destroy(poll_ctx);
	return result;
}

static zend_string *php_io_terminal_read_stream_secret(
	php_io_terminal_native_stream input,
	php_stream *stream,
	const struct timespec *timeout,
	bool *timed_out
)
{
	*timed_out = false;
	int fd = input;
	if (fd < 0 || isatty(fd) != 1) {
		return NULL;
	}

	struct termios mode;
	if (tcgetattr(fd, &mode) != 0) {
		return NULL;
	}

	php_poll_ctx *poll_ctx = php_io_terminal_create_poll_context(fd);
	if (poll_ctx == NULL) {
		return NULL;
	}

	struct termios raw_mode = mode;
	php_io_terminal_make_raw_mode(&raw_mode);
	bool mode_changed = memcmp(&raw_mode, &mode, sizeof(mode)) != 0;

	if (mode_changed && tcsetattr(fd, TCSANOW, &raw_mode) != 0) {
		php_poll_destroy(poll_ctx);
		return NULL;
	}

	bool has_overall_deadline = (timeout != NULL);
	zend_hrtime_t overall_deadline_ns = 0;
	if (has_overall_deadline) {
		overall_deadline_ns = php_io_terminal_saturating_add_deadline(
			zend_hrtime(),
			php_io_terminal_timespec_to_ns(timeout)
		);
	}

	smart_str secret = {0};
	bool success = false;

	while (true) {
		unsigned char key;
		struct timespec current_timeout;
		const struct timespec *current_timeout_ptr = NULL;

		if (has_overall_deadline) {
			zend_hrtime_t now = zend_hrtime();
			if (now >= overall_deadline_ns) {
				current_timeout.tv_sec = 0;
				current_timeout.tv_nsec = 0;
			} else {
				php_io_terminal_ns_to_timespec(overall_deadline_ns - now, &current_timeout);
			}
			current_timeout_ptr = &current_timeout;
		}

		int read_result = php_io_terminal_read_byte(fd, stream, poll_ctx, &key, current_timeout_ptr, false, NULL);
		if (read_result == PHP_IO_TERMINAL_READ_TIMEOUT) {
			*timed_out = true;
			goto restore;
		}
		if (read_result != PHP_IO_TERMINAL_READ_SUCCESS) {
			*timed_out = false;
			goto restore;
		}

process_key:
		switch (key) {
			case '\r':
			case '\n':
				success = true;
				goto restore;
			case 0x7f:
			case 0x08:
				php_io_terminal_buffer_remove_last_utf8_char(&secret);
				break;
			case 0x03:
			case 0x04:
				*timed_out = false;
				goto restore;
			case 0x1b:
			{
				struct timespec step_timeout;
				const struct timespec *step_timeout_ptr = NULL;
				if (has_overall_deadline) {
					zend_hrtime_t now = zend_hrtime();
					if (now >= overall_deadline_ns) {
						step_timeout.tv_sec = 0;
						step_timeout.tv_nsec = 0;
					} else {
						zend_hrtime_t remaining_ns = overall_deadline_ns - now;
						zend_hrtime_t step_ns = remaining_ns < 25000000ULL ? remaining_ns : 25000000ULL;
						php_io_terminal_ns_to_timespec(step_ns, &step_timeout);
					}
					step_timeout_ptr = &step_timeout;
				} else {
					step_timeout.tv_sec = 0;
					step_timeout.tv_nsec = 25000000L;
					step_timeout_ptr = &step_timeout;
				}

				unsigned char next;
				int read_res = php_io_terminal_read_byte(fd, stream, poll_ctx, &next, step_timeout_ptr, false, NULL);
				if (read_res == PHP_IO_TERMINAL_READ_TIMEOUT) {
					/* Standalone Escape cancels */
					*timed_out = false;
					goto restore;
				}
				if (read_res != PHP_IO_TERMINAL_READ_SUCCESS) {
					/* Disconnect (EOF) or read error */
					*timed_out = false;
					goto restore;
				}

				if (next == 0x03 || next == 0x04 || next == 0x1b || next == '\r' || next == '\n') {
					/* Escape followed immediately by control byte, Enter, or repeated Escape cancels */
					*timed_out = false;
					goto restore;
				}

				if (next == '[') {
					while (true) {
						if (has_overall_deadline) {
							zend_hrtime_t now = zend_hrtime();
							if (now >= overall_deadline_ns) {
								step_timeout.tv_sec = 0;
								step_timeout.tv_nsec = 0;
							} else {
								zend_hrtime_t remaining_ns = overall_deadline_ns - now;
								zend_hrtime_t step_ns = remaining_ns < 25000000ULL ? remaining_ns : 25000000ULL;
								php_io_terminal_ns_to_timespec(step_ns, &step_timeout);
							}
							step_timeout_ptr = &step_timeout;
						} else {
							step_timeout.tv_sec = 0;
							step_timeout.tv_nsec = 25000000L;
							step_timeout_ptr = &step_timeout;
						}

						unsigned char csi_byte;
						read_res = php_io_terminal_read_byte(fd, stream, poll_ctx, &csi_byte, step_timeout_ptr, false, NULL);
						if (read_res != PHP_IO_TERMINAL_READ_SUCCESS) {
							if (has_overall_deadline && zend_hrtime() >= overall_deadline_ns) {
								*timed_out = true;
								goto restore;
							}
							if (read_res == PHP_IO_TERMINAL_READ_TIMEOUT) {
								/* Internal sequence timeout: end sequence skipping and continue secret input! */
								break;
							}
							/* Disconnect (EOF) or read error */
							*timed_out = false;
							goto restore;
						}
						if (csi_byte == 0x03 || csi_byte == 0x04 || csi_byte == 0x1b || csi_byte == '\r' || csi_byte == '\n') {
							/* Cancellation byte inside CSI! */
							*timed_out = false;
							goto restore;
						}
						if (csi_byte >= 0x40 && csi_byte <= 0x7e) {
							break;
						}
					}
					break;
				}

				if (next == 'O') {
					if (has_overall_deadline) {
						zend_hrtime_t now = zend_hrtime();
						if (now >= overall_deadline_ns) {
							step_timeout.tv_sec = 0;
							step_timeout.tv_nsec = 0;
						} else {
							zend_hrtime_t remaining_ns = overall_deadline_ns - now;
							zend_hrtime_t step_ns = remaining_ns < 25000000ULL ? remaining_ns : 25000000ULL;
							php_io_terminal_ns_to_timespec(step_ns, &step_timeout);
						}
						step_timeout_ptr = &step_timeout;
					} else {
						step_timeout.tv_sec = 0;
						step_timeout.tv_nsec = 25000000L;
						step_timeout_ptr = &step_timeout;
					}

					unsigned char ss3_byte;
					read_res = php_io_terminal_read_byte(fd, stream, poll_ctx, &ss3_byte, step_timeout_ptr, false, NULL);
					if (read_res != PHP_IO_TERMINAL_READ_SUCCESS) {
						if (has_overall_deadline && zend_hrtime() >= overall_deadline_ns) {
							*timed_out = true;
							goto restore;
						}
						if (read_res == PHP_IO_TERMINAL_READ_TIMEOUT) {
							/* Internal sequence timeout: continue secret input */
							break;
						}
						*timed_out = false;
						goto restore;
					}
					if (ss3_byte == 0x03 || ss3_byte == 0x04 || ss3_byte == 0x1b || ss3_byte == '\r' || ss3_byte == '\n') {
						/* Cancellation byte inside SS3! */
						*timed_out = false;
						goto restore;
					}
					/* Valid SS3 key (like F1-F4): safely ignored */
					break;
				}

				/* Any other trailing byte after escape: abort cleanly */
				*timed_out = false;
				goto restore;
			}
			default:
			{
				size_t seq_len = php_io_terminal_utf8_sequence_len(key);
				if (seq_len == 1) {
					smart_str_appendc(&secret, (char) key);
				} else {
					unsigned char seq[4];
					size_t i;
					bool invalid_continuation = false;
					unsigned char invalid_byte = 0;

					seq[0] = key;
					for (i = 1; i < seq_len; i++) {
						struct timespec cont_timeout;
						const struct timespec *cont_timeout_ptr = NULL;
						if (has_overall_deadline) {
							zend_hrtime_t now = zend_hrtime();
							if (now >= overall_deadline_ns) {
								cont_timeout.tv_sec = 0;
								cont_timeout.tv_nsec = 0;
							} else {
								php_io_terminal_ns_to_timespec(overall_deadline_ns - now, &cont_timeout);
							}
							cont_timeout_ptr = &cont_timeout;
						}

						int cont_res = php_io_terminal_read_byte(fd, stream, poll_ctx, &seq[i], cont_timeout_ptr, false, NULL);
						if (cont_res != PHP_IO_TERMINAL_READ_SUCCESS) {
							if (has_overall_deadline && zend_hrtime() >= overall_deadline_ns) {
								*timed_out = true;
								goto restore;
							}
							*timed_out = false;
							goto restore;
						}
						if ((seq[i] & 0xc0) != 0x80) {
							invalid_continuation = true;
							invalid_byte = seq[i];
							break;
						}
					}
					if (invalid_continuation) {
						smart_str_appendl(&secret, (const char *) seq, i);
						key = invalid_byte;
						goto process_key;
					}
					if (i == seq_len) {
						smart_str_appendl(&secret, (const char *) seq, i);
					}
				}
				break;
			}
		}
	}

restore:
	if (mode_changed && tcsetattr(fd, TCSANOW, &mode) != 0) {
		success = false;
		*timed_out = false;
	}

	php_poll_destroy(poll_ctx);

	if (success) {
		return smart_str_extract(&secret);
	}

	smart_str_free(&secret);
	return NULL;
}
#endif /* !PHP_WIN32 */

static zend_object *php_io_terminal_key_enum_from_string(zend_string *key)
{
	if (zend_string_equals_literal(key, "up")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "Up");
	}
	if (zend_string_equals_literal(key, "down")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "Down");
	}
	if (zend_string_equals_literal(key, "right")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "Right");
	}
	if (zend_string_equals_literal(key, "left")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "Left");
	}
	if (zend_string_equals_literal(key, "enter")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "Enter");
	}
	if (zend_string_equals_literal(key, "backspace")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "Backspace");
	}
	if (zend_string_equals_literal(key, "escape")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "Escape");
	}
	if (zend_string_equals_literal(key, "tab")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "Tab");
	}
	if (zend_string_equals_literal(key, "home")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "Home");
	}
	if (zend_string_equals_literal(key, "end")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "End");
	}
	if (zend_string_equals_literal(key, "delete")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "Delete");
	}
	if (zend_string_equals_literal(key, "pageup")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "PageUp");
	}
	if (zend_string_equals_literal(key, "pagedown")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "PageDown");
	}
	if (zend_string_equals_literal(key, "resize")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "Resize");
	}
	if (zend_string_equals_literal(key, "f1")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "F1");
	}
	if (zend_string_equals_literal(key, "f2")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "F2");
	}
	if (zend_string_equals_literal(key, "f3")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "F3");
	}
	if (zend_string_equals_literal(key, "f4")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "F4");
	}
	if (zend_string_equals_literal(key, "f5")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "F5");
	}
	if (zend_string_equals_literal(key, "f6")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "F6");
	}
	if (zend_string_equals_literal(key, "f7")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "F7");
	}
	if (zend_string_equals_literal(key, "f8")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "F8");
	}
	if (zend_string_equals_literal(key, "f9")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "F9");
	}
	if (zend_string_equals_literal(key, "f10")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "F10");
	}
	if (zend_string_equals_literal(key, "f11")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "F11");
	}
	if (zend_string_equals_literal(key, "f12")) {
		return zend_enum_get_case_cstr(php_io_terminal_key_ce, "F12");
	}
	return NULL;
}

/* Io\Terminal\TerminalSize methods */
PHP_METHOD(Io_Terminal_TerminalSize, __construct)
{
	zend_long cols, rows;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(cols)
		Z_PARAM_LONG(rows)
	ZEND_PARSE_PARAMETERS_END();

	if (cols <= 0) {
		zend_argument_value_error(1, "must be greater than 0");
		RETURN_THROWS();
	}

	if (rows <= 0) {
		zend_argument_value_error(2, "must be greater than 0");
		RETURN_THROWS();
	}

	zend_update_property_long(php_io_terminal_terminal_size_ce, Z_OBJ_P(ZEND_THIS), "cols", sizeof("cols") - 1, cols);
	if (UNEXPECTED(EG(exception))) {
		RETURN_THROWS();
	}

	zend_update_property_long(php_io_terminal_terminal_size_ce, Z_OBJ_P(ZEND_THIS), "rows", sizeof("rows") - 1, rows);
	if (UNEXPECTED(EG(exception))) {
		RETURN_THROWS();
	}
}

/* Io\Terminal\SystemModeToken methods */
PHP_METHOD(Io_Terminal_SystemModeToken, __construct)
{
	zend_throw_error(NULL, "Cannot directly construct Io\\Terminal\\SystemModeToken");
}

/* Io\Terminal\SystemTerminal methods */
PHP_METHOD(Io_Terminal_SystemTerminal, __construct)
{
	zend_throw_error(NULL, "Cannot directly construct Io\\Terminal\\SystemTerminal");
}

PHP_METHOD(Io_Terminal_SystemTerminal, fromStdio)
{
	ZEND_PARSE_PARAMETERS_NONE();

	object_init_ex(return_value, php_io_terminal_system_terminal_ce);
	php_io_terminal_object *intern = PHP_IO_TERMINAL_OBJ_FROM_ZV(return_value);

	php_io_terminal_stream_target target;
	if (php_io_terminal_stream_target_init(NULL, true, &target)
		&& php_io_terminal_native_stream_is_valid(target.native_stream)) {
		intern->has_identity = php_io_terminal_get_identity(target.native_stream, &intern->identity);
	}
}

PHP_METHOD(Io_Terminal_SystemTerminal, fromStreams)
{
	zval *input_arg;
	zval *output_arg = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_RESOURCE(input_arg)
		Z_PARAM_OPTIONAL
		Z_PARAM_RESOURCE_OR_NULL(output_arg)
	ZEND_PARSE_PARAMETERS_END();

	php_io_terminal_stream_target target;
	if (!php_io_terminal_stream_target_init(input_arg, true, &target) || target.php_stream == NULL) {
		if (!EG(exception)) {
			zend_argument_value_error(1, "must be a valid stream resource");
		}
		RETURN_THROWS();
	}

	if (output_arg != NULL && !Z_ISNULL_P(output_arg)) {
		php_io_terminal_stream_target out_target;
		if (!php_io_terminal_stream_target_init(output_arg, false, &out_target) || out_target.php_stream == NULL) {
			if (!EG(exception)) {
				zend_argument_value_error(2, "must be a valid stream resource or null");
			}
			RETURN_THROWS();
		}
	}

	object_init_ex(return_value, php_io_terminal_system_terminal_ce);
	php_io_terminal_object *intern = PHP_IO_TERMINAL_OBJ_FROM_ZV(return_value);
	ZVAL_COPY(&intern->input_stream_val, input_arg);

	if (output_arg != NULL && !Z_ISNULL_P(output_arg)) {
		ZVAL_COPY(&intern->output_stream_val, output_arg);
	} else {
		ZVAL_COPY(&intern->output_stream_val, input_arg);
	}

	if (php_io_terminal_native_stream_is_valid(target.native_stream)) {
		intern->has_identity = php_io_terminal_get_identity(target.native_stream, &intern->identity);
	}
}

PHP_METHOD(Io_Terminal_SystemTerminal, getSize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_terminal_object *intern = PHP_IO_TERMINAL_OBJ_FROM_ZV(ZEND_THIS);
	php_io_terminal_stream_target stream;

	if (!php_io_terminal_stream_target_init(&intern->output_stream_val, false, &stream)) {
		RETURN_THROWS();
	}

	zend_long columns = 0;
	zend_long rows = 0;

	if (!php_io_terminal_stream_size(stream.native_stream, &columns, &rows)) {
		RETURN_NULL();
	}

	intern->last_cols = columns;
	intern->last_rows = rows;
	intern->has_last_size = true;

	zval params[2];
	ZVAL_LONG(&params[0], columns);
	ZVAL_LONG(&params[1], rows);
	object_init_with_constructor(return_value, php_io_terminal_terminal_size_ce, 2, params, NULL);
}

PHP_METHOD(Io_Terminal_SystemTerminal, enableRawMode)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_terminal_object *intern = PHP_IO_TERMINAL_OBJ_FROM_ZV(ZEND_THIS);
	php_io_terminal_stream_target stream;

	if (!php_io_terminal_stream_target_init(&intern->input_stream_val, true, &stream)) {
		RETURN_THROWS();
	}

	if (!php_io_terminal_native_stream_is_valid(stream.native_stream) || !php_io_terminal_native_stream_is_tty(stream.native_stream)) {
		zend_throw_exception(php_io_terminal_exception_ce, "Failed to enable terminal raw mode", 0);
		RETURN_THROWS();
	}

	php_io_terminal_identity identity;
	if (!php_io_terminal_get_identity(stream.native_stream, &identity)) {
		zend_throw_exception(php_io_terminal_exception_ce, "Failed to enable terminal raw mode", 0);
		RETURN_THROWS();
	}

	intern->identity = identity;
	intern->has_identity = true;

	php_io_terminal_shared_mode *shared = php_io_terminal_acquire_raw_mode(stream.native_stream, &identity);
	if (shared == NULL) {
		zend_throw_exception(php_io_terminal_exception_ce, "Failed to enable terminal raw mode", 0);
		RETURN_THROWS();
	}

	php_io_terminal_create_mode_token(return_value, shared);

	if (intern->active_mode_token != NULL) {
		OBJ_RELEASE(intern->active_mode_token);
	}
	intern->active_mode_token = Z_OBJ_P(return_value);
	GC_ADDREF(intern->active_mode_token);
}

/*
 * Invariant: A native Terminal implementation may only restore a native
 * ModeToken associated with the same underlying terminal/shared mode record.
 *
 * The native implementation intentionally allows a token to be restored by
 * another Terminal object that refers to the same underlying terminal identity
 * or shared mode record.
 *
 * TerminalInterface::restoreMode(?ModeTokenInterface $mode = null) accepts the
 * interface to allow polymorphic userland terminal fakes (e.g. for unit testing).
 * However, the native Terminal implementation requires native C state attached to
 * an internal ModeToken instance associated with the same underlying terminal or
 * shared mode record; foreign userland token implementations must still be rejected
 * with ValueError.
 */
PHP_METHOD(Io_Terminal_SystemTerminal, restoreMode)
{
	zval *mode_token = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS_OR_NULL(mode_token, php_io_terminal_mode_token_ce)
	ZEND_PARSE_PARAMETERS_END();

	php_io_terminal_object *intern = PHP_IO_TERMINAL_OBJ_FROM_ZV(ZEND_THIS);

	if (mode_token != NULL && !Z_ISNULL_P(mode_token)) {
		if (!instanceof_function(Z_OBJCE_P(mode_token), php_io_terminal_system_mode_token_ce)) {
			zend_argument_value_error(1, "must be an active terminal mode token belonging to this terminal");
			RETURN_THROWS();
		}
		php_io_terminal_mode_token_object *mode = PHP_IO_TERMINAL_MODE_TOKEN_OBJ_FROM_ZV(mode_token);

		if (!mode->valid || mode->shared == NULL) {
			zend_argument_value_error(1, "must be an active terminal mode token belonging to this terminal");
			RETURN_THROWS();
		}

		bool matches = false;
		if (intern->active_mode_token != NULL && intern->active_mode_token == Z_OBJ_P(mode_token)) {
			matches = true;
		} else {
			php_io_terminal_stream_target stream;
			php_io_terminal_identity current_identity;

			if (php_io_terminal_stream_target_init_ex(&intern->input_stream_val, true, true, &stream)
				&& php_io_terminal_native_stream_is_valid(stream.native_stream)
				&& php_io_terminal_get_identity(stream.native_stream, &current_identity)) {
				matches = php_io_terminal_identities_match(&current_identity, &mode->shared->identity);
			} else if (intern->has_identity) {
				matches = php_io_terminal_identities_match(&intern->identity, &mode->shared->identity);
			}
		}

		if (!matches) {
			zend_argument_value_error(1, "must be an active terminal mode token belonging to this terminal");
			RETURN_THROWS();
		}

		const char *err = NULL;
		if (!php_io_terminal_release_token_lease(mode, &err)) {
			zend_throw_exception(php_io_terminal_exception_ce, err != NULL ? err : "Failed to restore terminal mode", 0);
			RETURN_THROWS();
		}

		if (intern->active_mode_token != NULL && intern->active_mode_token == Z_OBJ_P(mode_token)) {
			OBJ_RELEASE(intern->active_mode_token);
			intern->active_mode_token = NULL;
		}

		RETURN_TRUE;
	}

	if (intern->active_mode_token != NULL) {
		php_io_terminal_mode_token_object *mode = PHP_IO_TERMINAL_MODE_TOKEN_OBJ_FROM_ZOBJ(intern->active_mode_token);

		if (!mode->valid || mode->shared == NULL) {
			OBJ_RELEASE(intern->active_mode_token);
			intern->active_mode_token = NULL;
			RETURN_FALSE;
		}

		const char *err = NULL;
		if (!php_io_terminal_release_token_lease(mode, &err)) {
			zend_throw_exception(php_io_terminal_exception_ce, err != NULL ? err : "Failed to restore terminal mode", 0);
			RETURN_THROWS();
		}

		OBJ_RELEASE(intern->active_mode_token);
		intern->active_mode_token = NULL;

		RETURN_TRUE;
	}

	RETURN_FALSE;
}

PHP_METHOD(Io_Terminal_SystemTerminal, readKey)
{
	php_date_time_duration *timeout_duration = NULL;
	php_date_time_duration *seq_timeout_duration = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_DATE_TIME_DURATION_OR_NULL(timeout_duration)
		Z_PARAM_DATE_TIME_DURATION_OR_NULL(seq_timeout_duration)
	ZEND_PARSE_PARAMETERS_END();

	if (timeout_duration != NULL && timeout_duration->duration.negative) {
		zend_argument_value_error(1, "must not be negative");
		RETURN_THROWS();
	}

	if (seq_timeout_duration != NULL && seq_timeout_duration->duration.negative) {
		zend_argument_value_error(2, "must not be negative");
		RETURN_THROWS();
	}

	php_io_terminal_object *intern = PHP_IO_TERMINAL_OBJ_FROM_ZV(ZEND_THIS);
	php_io_terminal_stream_target out_stream;

	if (php_io_terminal_stream_target_init(&intern->output_stream_val, false, &out_stream)) {
		zend_long cur_cols = 0, cur_rows = 0;
		if (php_io_terminal_stream_size(out_stream.native_stream, &cur_cols, &cur_rows)) {
			if (intern->has_last_size && (cur_cols != intern->last_cols || cur_rows != intern->last_rows)) {
				intern->last_cols = cur_cols;
				intern->last_rows = cur_rows;
				RETURN_OBJ_COPY(zend_enum_get_case_cstr(php_io_terminal_key_ce, "Resize"));
			}
			intern->last_cols = cur_cols;
			intern->last_rows = cur_rows;
			intern->has_last_size = true;
		}
	}

	php_io_terminal_stream_target stream;
	if (!php_io_terminal_stream_target_init(&intern->input_stream_val, true, &stream)) {
		RETURN_THROWS();
	}

#ifdef PHP_WIN32
	DWORD wait_ms = INFINITE;
	if (timeout_duration != NULL) {
		uint64_t sec = timeout_duration->duration.seconds;
		uint32_t nsec = timeout_duration->duration.nanoseconds;
		if (sec == 0 && nsec == 0) {
			wait_ms = 0;
		} else if (sec >= (DWORD) PHP_IO_TERMINAL_MAX_WAIT_MS / 1000) {
			wait_ms = PHP_IO_TERMINAL_MAX_WAIT_MS;
		} else {
			uint64_t ms = sec * 1000 + (nsec + 999999) / 1000000;
			wait_ms = ms >= (DWORD) PHP_IO_TERMINAL_MAX_WAIT_MS ? PHP_IO_TERMINAL_MAX_WAIT_MS : (DWORD) ms;
		}
	}

	zend_string *key = php_io_terminal_read_stream_key(
		stream.native_stream,
		stream.php_stream,
		wait_ms,
		&intern->pending_high_surrogate,
		&intern->pending_key,
		&intern->pending_key_high_surrogate
	);
#else
	struct timespec timeout;
	struct timespec sequence_timeout = {0, PHP_IO_TERMINAL_SEQUENCE_TIMEOUT_MS * 1000000L};
	const struct timespec *timeout_ptr = NULL;
	bool is_non_blocking = false;

	if (timeout_duration != NULL) {
		is_non_blocking = (timeout_duration->duration.seconds == 0 && timeout_duration->duration.nanoseconds == 0);
		timeout.tv_sec = php_io_terminal_clamp_duration_seconds(timeout_duration->duration.seconds);
		timeout.tv_nsec = timeout_duration->duration.nanoseconds;
		timeout_ptr = &timeout;
	}

	if (is_non_blocking) {
		sequence_timeout.tv_sec = 0;
		sequence_timeout.tv_nsec = 0;
	} else if (seq_timeout_duration != NULL) {
		sequence_timeout.tv_sec = php_io_terminal_clamp_duration_seconds(seq_timeout_duration->duration.seconds);
		sequence_timeout.tv_nsec = seq_timeout_duration->duration.nanoseconds;
	}

	zend_string *key = php_io_terminal_read_stream_key(
		stream.native_stream,
		stream.php_stream,
		timeout_ptr,
		&sequence_timeout,
		&intern->pending_utf8
	);
#endif

	if (EG(exception)) {
		if (key != NULL) {
			zend_string_release(key);
		}
		RETURN_THROWS();
	}

	if (key == NULL) {
		RETURN_NULL();
	}

	zend_object *key_case = php_io_terminal_key_enum_from_string(key);
	if (key_case != NULL) {
		if (key_case == zend_enum_get_case_cstr(php_io_terminal_key_ce, "Resize")) {
			php_io_terminal_stream_target out_s;
			if (php_io_terminal_stream_target_init(&intern->output_stream_val, false, &out_s)) {
				zend_long c = 0, r = 0;
				if (php_io_terminal_stream_size(out_s.native_stream, &c, &r)) {
					intern->last_cols = c;
					intern->last_rows = r;
					intern->has_last_size = true;
				}
			}
		}
		zend_string_release(key);
		RETURN_OBJ_COPY(key_case);
	}

	RETURN_STR(key);
}

#ifdef PHP_WIN32
static zend_string *php_io_terminal_read_line_windows(
	php_io_terminal_native_stream handle,
	php_stream *stream,
	php_io_terminal_object *intern
)
{
	/* If there is already buffered data in php_stream, use php_stream_get_line */
	if (stream != NULL && stream->writepos > stream->readpos) {
		size_t line_len = 0;
		char *buf = php_stream_get_line(stream, NULL, 0, &line_len);
		if (buf == NULL) {
			if (php_stream_eof(stream)) {
				return NULL;
			}
			if (!EG(exception)) {
				zend_throw_exception(php_io_terminal_exception_ce, "Failed to read line from stream", 0);
			}
			return NULL;
		}
		if (line_len >= 2 && buf[line_len - 2] == '\r' && buf[line_len - 1] == '\n') {
			line_len -= 2;
		} else if (line_len >= 1 && buf[line_len - 1] == '\n') {
			line_len -= 1;
		}
		buf[line_len] = '\0';
		zend_string *res = zend_string_init(buf, line_len, 0);
		efree(buf);
		return res;
	}

	DWORD original_mode = 0;
	bool is_console = (handle != INVALID_HANDLE_VALUE && handle != NULL && GetConsoleMode(handle, &original_mode) != 0);

	if (!is_console) {
		/* Redirected/non-console stream or handle */
		if (stream != NULL) {
			size_t line_len = 0;
			char *buf = php_stream_get_line(stream, NULL, 0, &line_len);
			if (buf == NULL) {
				if (php_stream_eof(stream)) {
					return NULL;
				}
				if (!EG(exception)) {
					zend_throw_exception(php_io_terminal_exception_ce, "Failed to read line from stream", 0);
				}
				return NULL;
			}
			if (line_len >= 2 && buf[line_len - 2] == '\r' && buf[line_len - 1] == '\n') {
				line_len -= 2;
			} else if (line_len >= 1 && buf[line_len - 1] == '\n') {
				line_len -= 1;
			}
			buf[line_len] = '\0';
			zend_string *res = zend_string_init(buf, line_len, 0);
			efree(buf);
			return res;
		}

		/* Bare non-console handle */
		smart_str line = {0};
		char ch;
		DWORD bytes_read;
		while (1) {
			if (!ReadFile(handle, &ch, 1, &bytes_read, NULL)) {
				DWORD err = GetLastError();
				if (err == ERROR_BROKEN_PIPE || err == ERROR_HANDLE_EOF) {
					break; /* EOF */
				}
				smart_str_free(&line);
				zend_throw_exception(php_io_terminal_exception_ce, "Failed to read line from handle", 0);
				return NULL;
			}
			if (bytes_read == 0) {
				break; /* EOF */
			}
			smart_str_appendc(&line, ch);
			if (ch == '\n') {
				break;
			}
		}

		if (line.s == NULL) {
			return NULL;
		}

		size_t len = ZSTR_LEN(line.s);
		char *val = ZSTR_VAL(line.s);
		if (len >= 2 && val[len - 2] == '\r' && val[len - 1] == '\n') {
			len -= 2;
		} else if (len >= 1 && val[len - 1] == '\n') {
			len -= 1;
		}
		val[len] = '\0';
		zend_string *res = zend_string_init(val, len, 0);
		smart_str_free(&line);
		return res;
	}

	/* Native Windows console input */
	DWORD cooked_mode = original_mode | (ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);
	bool mode_changed = false;
	if (cooked_mode != original_mode) {
		if (!SetConsoleMode(handle, cooked_mode)) {
			zend_throw_exception(php_io_terminal_exception_ce, "Failed to set console mode for line reading", 0);
			return NULL;
		}
		mode_changed = true;
	}

	size_t wcapacity = 1024;
	size_t wlen = 0;
	WCHAR *wline = emalloc(wcapacity * sizeof(WCHAR));
	bool read_failed = false;

	while (true) {
		DWORD chars_read = 0;
		BOOL ok = ReadConsoleW(handle, wline + wlen, (DWORD)(wcapacity - wlen), &chars_read, NULL);
		if (!ok) {
			read_failed = true;
			break;
		}
		if (chars_read == 0) {
			break; /* EOF */
		}
		wlen += chars_read;
		if (wline[wlen - 1] == L'\n' || wline[wlen - 1] == 0x1a) {
			break;
		}
		if (wlen == wcapacity) {
			wcapacity *= 2;
			wline = erealloc(wline, wcapacity * sizeof(WCHAR));
		}
	}

	if (mode_changed) {
		SetConsoleMode(handle, original_mode);
	}

	if (read_failed) {
		efree(wline);
		zend_throw_exception(php_io_terminal_exception_ce, "Failed to read line from console", 0);
		return NULL;
	}

	if (wlen == 0 || (wlen == 1 && wline[0] == 0x1a)) {
		efree(wline);
		return NULL; /* EOF */
	}

	/* Normalize CRLF / LF */
	if (wlen >= 2 && wline[wlen - 2] == L'\r' && wline[wlen - 1] == L'\n') {
		wlen -= 2;
	} else if (wlen >= 1 && (wline[wlen - 1] == L'\n' || wline[wlen - 1] == 0x1a)) {
		wlen -= 1;
	} else if (wlen >= 1 && wline[wlen - 1] == L'\r') {
		wlen -= 1;
	}

	if (wlen == 0) {
		efree(wline);
		return zend_string_init("", 0, 0);
	}

	int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wline, (int) wlen, NULL, 0, NULL, NULL);
	if (utf8_len < 0) {
		efree(wline);
		zend_throw_exception(php_io_terminal_exception_ce, "Failed to convert console line to UTF-8", 0);
		return NULL;
	}

	zend_string *res = zend_string_alloc(utf8_len, 0);
	WideCharToMultiByte(CP_UTF8, 0, wline, (int) wlen, ZSTR_VAL(res), utf8_len, NULL, NULL);
	ZSTR_VAL(res)[utf8_len] = '\0';
	efree(wline);
	return res;
}
#else
static char *php_io_terminal_read_line_from_fd(int fd, size_t *out_len)
{
	smart_str line = {0};
	char ch;
	errno = 0;
	while (1) {
		ssize_t n = read(fd, &ch, 1);
		if (n > 0) {
			smart_str_appendc(&line, ch);
			if (ch == '\n') {
				break;
			}
		} else if (n == 0) {
			break; /* EOF */
		} else {
			if (errno == EINTR) {
				continue;
			}
			smart_str_free(&line);
			return NULL;
		}
	}

	if (line.s == NULL) {
		*out_len = 0;
		return NULL;
	}

	*out_len = ZSTR_LEN(line.s);
	char *res = estrndup(ZSTR_VAL(line.s), *out_len);
	smart_str_free(&line);
	return res;
}

static char *php_io_terminal_read_line_posix(
	php_io_terminal_native_stream fd,
	php_stream *stream,
	size_t *out_len,
	bool *is_eof
)
{
	*is_eof = false;
	*out_len = 0;

	if (stream != NULL) {
		char *buf = php_stream_get_line(stream, NULL, 0, out_len);
		if (buf == NULL) {
			if (php_stream_eof(stream)) {
				*is_eof = true;
				return NULL;
			}
			/* Read error */
			return NULL;
		}
		return buf;
	}

	if (fd >= 0) {
		char *buf = php_io_terminal_read_line_from_fd(fd, out_len);
		if (buf == NULL) {
			if (errno == 0 || errno == EAGAIN) {
				*is_eof = true;
			}
			return NULL;
		}
		return buf;
	}

	return NULL;
}
#endif

PHP_METHOD(Io_Terminal_SystemTerminal, readLine)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_terminal_object *intern = PHP_IO_TERMINAL_OBJ_FROM_ZV(ZEND_THIS);

	php_io_terminal_stream_target stream;
	if (!php_io_terminal_stream_target_init(&intern->input_stream_val, true, &stream)) {
		RETURN_THROWS();
	}

	if (!intern->has_identity && php_io_terminal_native_stream_is_valid(stream.native_stream)) {
		intern->has_identity = php_io_terminal_get_identity(stream.native_stream, &intern->identity);
	}

	if (intern->active_mode_token != NULL || (intern->has_identity && php_io_terminal_find_shared_mode(&intern->identity) != NULL)) {
		zend_throw_exception(php_io_terminal_exception_ce, "Cannot read a line while raw mode is active for this terminal", 0);
		RETURN_THROWS();
	}

#ifdef PHP_WIN32
	zend_string *line = php_io_terminal_read_line_windows(stream.native_stream, stream.php_stream, intern);
	if (EG(exception)) {
		if (line != NULL) {
			zend_string_release(line);
		}
		RETURN_THROWS();
	}
	if (line == NULL) {
		RETURN_NULL();
	}
	RETURN_STR(line);
#else
	size_t out_len = 0;
	bool is_eof = false;
	char *buf = php_io_terminal_read_line_posix(stream.native_stream, stream.php_stream, &out_len, &is_eof);
	if (EG(exception)) {
		if (buf != NULL) {
			efree(buf);
		}
		RETURN_THROWS();
	}

	if (buf == NULL) {
		if (is_eof) {
			if (intern->pending_utf8.length > 0) {
				zend_string *res = zend_string_init((char *) intern->pending_utf8.bytes, intern->pending_utf8.length, 0);
				intern->pending_utf8.length = 0;
				intern->pending_utf8.expected = 0;
				RETURN_STR(res);
			}
			RETURN_NULL();
		}
		if (!EG(exception)) {
			zend_throw_exception(php_io_terminal_exception_ce, "Failed to read line from terminal", 0);
		}
		RETURN_THROWS();
	}

	/* Strip line ending */
	if (out_len >= 2 && buf[out_len - 2] == '\r' && buf[out_len - 1] == '\n') {
		out_len -= 2;
	} else if (out_len >= 1 && buf[out_len - 1] == '\n') {
		out_len -= 1;
	}
	buf[out_len] = '\0';

	zend_string *res;
	if (intern->pending_utf8.length > 0) {
		size_t total_len = intern->pending_utf8.length + out_len;
		res = zend_string_alloc(total_len, 0);
		memcpy(ZSTR_VAL(res), intern->pending_utf8.bytes, intern->pending_utf8.length);
		if (out_len > 0) {
			memcpy(ZSTR_VAL(res) + intern->pending_utf8.length, buf, out_len);
		}
		ZSTR_VAL(res)[total_len] = '\0';
		intern->pending_utf8.length = 0;
		intern->pending_utf8.expected = 0;
	} else {
		res = zend_string_init(buf, out_len, 0);
	}
	efree(buf);
	RETURN_STR(res);
#endif
}

PHP_METHOD(Io_Terminal_SystemTerminal, readSecret)
{
	php_date_time_duration *timeout_duration = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_DATE_TIME_DURATION_OR_NULL(timeout_duration)
	ZEND_PARSE_PARAMETERS_END();

	if (timeout_duration != NULL && timeout_duration->duration.negative) {
		zend_argument_value_error(1, "must not be negative");
		RETURN_THROWS();
	}

	php_io_terminal_object *intern = PHP_IO_TERMINAL_OBJ_FROM_ZV(ZEND_THIS);

#ifdef PHP_WIN32
	memset(&intern->pending_key, 0, sizeof(intern->pending_key));
	intern->pending_key_high_surrogate = 0;
	intern->pending_high_surrogate = 0;
#else
	memset(&intern->pending_utf8, 0, sizeof(intern->pending_utf8));
#endif

	php_io_terminal_stream_target stream;
	if (!php_io_terminal_stream_target_init(&intern->input_stream_val, true, &stream)) {
		RETURN_THROWS();
	}

	bool timed_out = false;
#ifdef PHP_WIN32
	DWORD wait_ms = INFINITE;
	bool has_timeout = false;
	if (timeout_duration != NULL) {
		has_timeout = true;
		uint64_t sec = timeout_duration->duration.seconds;
		uint32_t nsec = timeout_duration->duration.nanoseconds;
		if (sec == 0 && nsec == 0) {
			wait_ms = 0;
		} else if (sec >= (DWORD) PHP_IO_TERMINAL_MAX_WAIT_MS / 1000) {
			wait_ms = PHP_IO_TERMINAL_MAX_WAIT_MS;
		} else {
			uint64_t ms = sec * 1000 + (nsec + 999999) / 1000000;
			wait_ms = ms >= (DWORD) PHP_IO_TERMINAL_MAX_WAIT_MS ? PHP_IO_TERMINAL_MAX_WAIT_MS : (DWORD) ms;
		}
	}
	zend_string *secret = php_io_terminal_read_stream_secret(
		stream.native_stream,
		stream.php_stream,
		&intern->pending_key,
		&intern->pending_key_high_surrogate,
		wait_ms,
		has_timeout,
		&timed_out
	);
#else
	struct timespec timeout;
	const struct timespec *timeout_ptr = NULL;
	if (timeout_duration != NULL) {
		timeout.tv_sec = php_io_terminal_clamp_duration_seconds(timeout_duration->duration.seconds);
		timeout.tv_nsec = timeout_duration->duration.nanoseconds;
		timeout_ptr = &timeout;
	}
	zend_string *secret = php_io_terminal_read_stream_secret(stream.native_stream, stream.php_stream, timeout_ptr, &timed_out);
#endif
	if (secret != NULL) {
		RETURN_STR(secret);
	}

	if (timed_out) {
		RETURN_NULL();
	}

	if (!EG(exception)) {
		zend_throw_exception(php_io_terminal_exception_ce, "Unable to read secret from terminal", 0);
	}
	RETURN_THROWS();
}

PHP_MINIT_FUNCTION(terminal)
{
	zend_class_entry *io_exception_ce = zend_hash_str_find_ptr(
		CG(class_table),
		"io\\ioexception",
		sizeof("io\\ioexception") - 1
	);
	ZEND_ASSERT(io_exception_ce != NULL);

	php_io_terminal_exception_ce
		= register_class_Io_Terminal_TerminalException(io_exception_ce);

	php_io_terminal_key_ce = register_class_Io_Terminal_Key();

	php_io_terminal_terminal_size_ce = register_class_Io_Terminal_TerminalSize();

	php_io_terminal_mode_token_ce = register_class_Io_Terminal_ModeToken();
	php_io_terminal_terminal_ce = register_class_Io_Terminal_Terminal();

	php_io_terminal_system_mode_token_ce = register_class_Io_Terminal_SystemModeToken(php_io_terminal_mode_token_ce);
	php_io_terminal_system_mode_token_ce->create_object = php_io_terminal_mode_token_create_object;
	memcpy(&php_io_terminal_mode_token_handlers, zend_get_std_object_handlers(), sizeof(php_io_terminal_mode_token_handlers));
	php_io_terminal_mode_token_handlers.offset = offsetof(php_io_terminal_mode_token_object, std);
	php_io_terminal_mode_token_handlers.free_obj = php_io_terminal_mode_token_free_obj;
	php_io_terminal_mode_token_handlers.clone_obj = NULL;

	php_io_terminal_system_terminal_ce = register_class_Io_Terminal_SystemTerminal(php_io_terminal_terminal_ce);
	php_io_terminal_system_terminal_ce->create_object = php_io_terminal_create_object;
	memcpy(&php_io_terminal_object_handlers, zend_get_std_object_handlers(), sizeof(php_io_terminal_object_handlers));
	php_io_terminal_object_handlers.offset = offsetof(php_io_terminal_object, std);
	php_io_terminal_object_handlers.free_obj = php_io_terminal_free_obj;
	php_io_terminal_object_handlers.clone_obj = NULL;

#ifdef ZTS
# ifndef PHP_WIN32
#  ifdef SIGWINCH
	php_io_terminal_resize_mutex = tsrm_mutex_alloc();
#  endif
#  ifndef PHP_IO_TERMINAL_HAVE_PTSNAME_R
	php_io_terminal_ptsname_mutex = tsrm_mutex_alloc();
#  endif
# endif
#endif

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(terminal)
{
#ifdef ZTS
# ifndef PHP_WIN32
#  ifdef SIGWINCH
	if (php_io_terminal_resize_mutex != NULL) {
		tsrm_mutex_free(php_io_terminal_resize_mutex);
		php_io_terminal_resize_mutex = NULL;
	}
#  endif
#  ifndef PHP_IO_TERMINAL_HAVE_PTSNAME_R
	if (php_io_terminal_ptsname_mutex != NULL) {
		tsrm_mutex_free(php_io_terminal_ptsname_mutex);
		php_io_terminal_ptsname_mutex = NULL;
	}
#  endif
# endif
#endif

	return SUCCESS;
}

PHP_RINIT_FUNCTION(terminal)
{
	php_io_terminal_active_shared_modes = NULL;
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(terminal)
{
	while (php_io_terminal_active_shared_modes != NULL) {
		php_io_terminal_shared_mode *shared = php_io_terminal_active_shared_modes;
		php_io_terminal_restore_shared_mode(shared);
		php_io_terminal_destroy_shared_mode(shared);
	}
	return SUCCESS;
}
