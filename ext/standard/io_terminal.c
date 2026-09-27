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
# include <signal.h>
# include <sys/stat.h>
# include <termios.h>
# include <time.h>
# include <unistd.h>
# include <sys/ioctl.h>
#endif

#define PHP_IO_TERMINAL_MODE_TOKEN_MAGIC "PHPTTY1"
#define PHP_IO_TERMINAL_MODE_TOKEN_MAGIC_LEN (sizeof(PHP_IO_TERMINAL_MODE_TOKEN_MAGIC) - 1)
#define PHP_IO_TERMINAL_SEQUENCE_TIMEOUT_MS 25

#ifdef PHP_WIN32
typedef HANDLE php_io_terminal_native_stream;
#else
typedef int php_io_terminal_native_stream;

typedef struct php_io_terminal_utf8_pending {
	unsigned char bytes[4];
	size_t length;
	size_t expected;
} php_io_terminal_utf8_pending;
#endif

typedef struct php_io_terminal_saved_mode {
	char magic[PHP_IO_TERMINAL_MODE_TOKEN_MAGIC_LEN];
	php_io_terminal_native_stream stream;
#ifdef PHP_WIN32
	DWORD mode;
#else
	struct termios mode;
#endif
} php_io_terminal_saved_mode;

typedef struct php_io_terminal_mode_token_object {
	php_io_terminal_saved_mode saved;
	zval stream_resource;
	struct php_io_terminal_mode_token_object *active_prev;
	struct php_io_terminal_mode_token_object *active_next;
	bool valid;
	bool tracked;
	zend_object std;
} php_io_terminal_mode_token_object;

typedef struct php_io_terminal_object {
	zval input_stream_val;
	zval output_stream_val;
	zend_object *active_mode_token;
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
static zend_object_handlers php_io_terminal_mode_token_handlers;
static zend_object_handlers php_io_terminal_object_handlers;
ZEND_TLS php_io_terminal_mode_token_object *php_io_terminal_active_mode_tokens;

#if !defined(PHP_WIN32)
#define PHP_IO_TERMINAL_READ_RESIZE 2

#ifdef SIGWINCH
static volatile sig_atomic_t php_io_terminal_resize_generation = 0;
static unsigned int php_io_terminal_resize_readers = 0;
static struct sigaction php_io_terminal_previous_resize_action;
#ifdef ZTS
static MUTEX_T php_io_terminal_resize_mutex = NULL;
#endif

static void php_io_terminal_resize_lock(void)
{
#ifdef ZTS
	if (php_io_terminal_resize_mutex != NULL) {
		tsrm_mutex_lock(php_io_terminal_resize_mutex);
	}
#endif
}

static void php_io_terminal_resize_unlock(void)
{
#ifdef ZTS
	if (php_io_terminal_resize_mutex != NULL) {
		tsrm_mutex_unlock(php_io_terminal_resize_mutex);
	}
#endif
}

static void php_io_terminal_sigwinch_handler(int signo)
{
	(void) signo;

	php_io_terminal_resize_generation = php_io_terminal_resize_generation == SIG_ATOMIC_MAX
		? 0
		: php_io_terminal_resize_generation + 1;
}

static bool php_io_terminal_install_resize_handler(sig_atomic_t *generation)
{
	struct sigaction action;
	bool installed = true;
	sig_atomic_t current_generation;

	php_io_terminal_resize_lock();
	current_generation = php_io_terminal_resize_generation;

	if (php_io_terminal_resize_readers == 0) {
		memset(&action, 0, sizeof(action));
		action.sa_handler = php_io_terminal_sigwinch_handler;
		sigemptyset(&action.sa_mask);

		installed = sigaction(SIGWINCH, &action, &php_io_terminal_previous_resize_action) == 0;
	}

	if (installed) {
		php_io_terminal_resize_readers++;
		*generation = current_generation;
	}

	php_io_terminal_resize_unlock();

	return installed;
}

static void php_io_terminal_restore_resize_handler(void)
{
	php_io_terminal_resize_lock();

	if (php_io_terminal_resize_readers > 0 && --php_io_terminal_resize_readers == 0) {
		sigaction(SIGWINCH, &php_io_terminal_previous_resize_action, NULL);
	}

	php_io_terminal_resize_unlock();
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

static bool php_io_terminal_mode_streams_match(php_io_terminal_native_stream first, php_io_terminal_native_stream second)
{
	return first == second;
}

static bool php_io_terminal_mode_token_stream_is_valid(const php_io_terminal_mode_token_object *mode)
{
	if (!Z_ISUNDEF(mode->stream_resource)) {
		php_stream *stream;
		php_io_terminal_native_stream native_stream;

		if (Z_TYPE(mode->stream_resource) != IS_RESOURCE) {
			return false;
		}

		stream = (php_stream *) zend_fetch_resource2(
			Z_RES(mode->stream_resource),
			NULL,
			php_file_le_stream(),
			php_file_le_pstream()
		);
		if (stream == NULL) {
			return false;
		}

		native_stream = php_io_terminal_native_stream_from_php_stream(stream);
		if (!php_io_terminal_native_stream_is_valid(native_stream)
			|| !php_io_terminal_mode_streams_match(native_stream, mode->saved.stream)) {
			return false;
		}
	}

	return php_io_terminal_native_stream_is_valid(mode->saved.stream);
}

static bool php_io_terminal_restore_stream_mode(const php_io_terminal_saved_mode *saved)
{
	if (!php_io_terminal_native_stream_is_valid(saved->stream)) {
		return false;
	}

#ifdef PHP_WIN32
	return SetConsoleMode(saved->stream, saved->mode) != 0;
#else
	return tcsetattr(saved->stream, TCSANOW, &saved->mode) == 0;
#endif
}

static void php_io_terminal_untrack_mode_token(php_io_terminal_mode_token_object *mode)
{
	if (!mode->tracked) {
		return;
	}

	if (mode->active_prev != NULL) {
		mode->active_prev->active_next = mode->active_next;
	} else {
		php_io_terminal_active_mode_tokens = mode->active_next;
	}

	if (mode->active_next != NULL) {
		mode->active_next->active_prev = mode->active_prev;
	}

	mode->active_prev = NULL;
	mode->active_next = NULL;
	mode->tracked = false;
}

static void php_io_terminal_track_mode_token(php_io_terminal_mode_token_object *mode)
{
	mode->active_prev = NULL;
	mode->active_next = php_io_terminal_active_mode_tokens;
	if (php_io_terminal_active_mode_tokens != NULL) {
		php_io_terminal_active_mode_tokens->active_prev = mode;
	}
	php_io_terminal_active_mode_tokens = mode;
	mode->tracked = true;
}

static bool php_io_terminal_release_mode_token(php_io_terminal_mode_token_object *mode)
{
	php_io_terminal_mode_token_object *candidate = php_io_terminal_active_mode_tokens;
	php_io_terminal_mode_token_object *newer = NULL;
	bool restored;

	while (candidate != NULL && candidate != mode) {
		if (candidate->valid && php_io_terminal_mode_streams_match(candidate->saved.stream, mode->saved.stream)) {
			newer = candidate;
		}
		candidate = candidate->active_next;
	}

	if (candidate != mode) {
		newer = NULL;
	}

	if (newer != NULL) {
		newer->saved.mode = mode->saved.mode;
		php_io_terminal_untrack_mode_token(mode);
		memset(&mode->saved, 0, sizeof(mode->saved));
		mode->valid = false;
		return true;
	}

	if (!php_io_terminal_mode_token_stream_is_valid(mode)) {
		return false;
	}

	restored = php_io_terminal_restore_stream_mode(&mode->saved);
	if (restored) {
		php_io_terminal_untrack_mode_token(mode);
		memset(&mode->saved, 0, sizeof(mode->saved));
		mode->valid = false;
	}

	return restored;
}

static void php_io_terminal_mode_token_free_obj(zend_object *object)
{
	php_io_terminal_mode_token_object *intern = PHP_IO_TERMINAL_MODE_TOKEN_OBJ_FROM_ZOBJ(object);

	if (intern->valid && memcmp(intern->saved.magic, PHP_IO_TERMINAL_MODE_TOKEN_MAGIC, PHP_IO_TERMINAL_MODE_TOKEN_MAGIC_LEN) == 0) {
		php_io_terminal_release_mode_token(intern);
	}
	php_io_terminal_untrack_mode_token(intern);

	if (!Z_ISUNDEF(intern->stream_resource)) {
		zval_ptr_dtor(&intern->stream_resource);
		ZVAL_UNDEF(&intern->stream_resource);
	}

	memset(&intern->saved, 0, sizeof(intern->saved));
	intern->valid = false;

	zend_object_std_dtor(&intern->std);
}

static zend_object *php_io_terminal_mode_token_create_object(zend_class_entry *ce)
{
	php_io_terminal_mode_token_object *intern = zend_object_alloc(sizeof(*intern), ce);

	memset(&intern->saved, 0, sizeof(intern->saved));
	ZVAL_UNDEF(&intern->stream_resource);
	intern->active_prev = NULL;
	intern->active_next = NULL;
	intern->valid = false;
	intern->tracked = false;

	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);
	intern->std.handlers = &php_io_terminal_mode_token_handlers;

	return &intern->std;
}

static void php_io_terminal_create_mode_token(zval *return_value, const php_io_terminal_saved_mode *saved, zval *stream_resource)
{
	php_io_terminal_mode_token_object *intern;

	object_init_ex(return_value, php_io_terminal_mode_token_ce);
	intern = PHP_IO_TERMINAL_MODE_TOKEN_OBJ_FROM_ZV(return_value);
	intern->saved = *saved;
	if (stream_resource != NULL && !Z_ISUNDEF_P(stream_resource)) {
		ZVAL_COPY(&intern->stream_resource, stream_resource);
	}
	intern->valid = true;
	php_io_terminal_track_mode_token(intern);
}

static void php_io_terminal_free_obj(zend_object *object)
{
	php_io_terminal_object *intern = PHP_IO_TERMINAL_OBJ_FROM_ZOBJ(object);

	if (intern->active_mode_token != NULL) {
		php_io_terminal_mode_token_object *mode = PHP_IO_TERMINAL_MODE_TOKEN_OBJ_FROM_ZOBJ(intern->active_mode_token);
		if (mode->valid && memcmp(mode->saved.magic, PHP_IO_TERMINAL_MODE_TOKEN_MAGIC, PHP_IO_TERMINAL_MODE_TOKEN_MAGIC_LEN) == 0) {
			php_io_terminal_release_mode_token(mode);
		}
		OBJ_RELEASE(intern->active_mode_token);
		intern->active_mode_token = NULL;
	}

	if (!Z_ISUNDEF(intern->input_stream_val)) {
		zval_ptr_dtor(&intern->input_stream_val);
		ZVAL_UNDEF(&intern->input_stream_val);
	}

	if (!Z_ISUNDEF(intern->output_stream_val)) {
		zval_ptr_dtor(&intern->output_stream_val);
		ZVAL_UNDEF(&intern->output_stream_val);
	}

	zend_object_std_dtor(&intern->std);
}

static zend_object *php_io_terminal_create_object(zend_class_entry *ce)
{
	php_io_terminal_object *intern = zend_object_alloc(sizeof(*intern), ce);

	ZVAL_UNDEF(&intern->input_stream_val);
	ZVAL_UNDEF(&intern->output_stream_val);
	intern->active_mode_token = NULL;
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

static bool php_io_terminal_stream_target_init(
	zval *stream_arg,
	bool is_input,
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
		return true;
	}

	if (Z_TYPE_P(stream_arg) == IS_RESOURCE) {
		target->php_stream = (php_stream *) zend_fetch_resource2(
			Z_RES_P(stream_arg),
			"stream",
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

#ifdef PHP_WIN32
static DWORD php_io_terminal_make_raw_mode(DWORD mode)
{
	return (mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT));
}

static bool php_io_terminal_enable_stream_raw_mode(php_io_terminal_native_stream handle, php_io_terminal_saved_mode *saved)
{
	DWORD mode;
	DWORD raw_mode;

	if (!php_io_terminal_native_stream_is_valid(handle) || !GetConsoleMode(handle, &mode)) {
		return false;
	}

	memcpy(saved->magic, PHP_IO_TERMINAL_MODE_TOKEN_MAGIC, PHP_IO_TERMINAL_MODE_TOKEN_MAGIC_LEN);
	saved->stream = handle;
	saved->mode = mode;

	raw_mode = php_io_terminal_make_raw_mode(mode) | ENABLE_WINDOW_INPUT;
	if (!SetConsoleMode(handle, raw_mode)) {
		return false;
	}

	return true;
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
	DWORD records_read;

	if (pending_key->Event.KeyEvent.wRepeatCount > 0) {
		*record = *pending_key;
		*high_surrogate = *pending_key_high_surrogate;
		pending_key->Event.KeyEvent.wRepeatCount = 0;
		return true;
	}

	return WaitForSingleObject(handle, wait_ms) == WAIT_OBJECT_0
		&& ReadConsoleInputW(handle, record, 1, &records_read) && records_read == 1;
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
	DWORD raw_mode;
	ULONGLONG deadline_ms = wait_ms == INFINITE ? 0 : GetTickCount64() + wait_ms;
	zend_string *result = NULL;
	bool mode_changed = false;

	if (handle == INVALID_HANDLE_VALUE || handle == NULL
		|| (stream != NULL && stream->writepos > stream->readpos)) {
		return NULL;
	}

	if (!GetConsoleMode(handle, &mode)) {
		return NULL;
	}

	raw_mode = php_io_terminal_make_raw_mode(mode) | ENABLE_WINDOW_INPUT;
	if (raw_mode != mode && !SetConsoleMode(handle, raw_mode)) {
		return NULL;
	}
	mode_changed = raw_mode != mode;

	for (;;) {
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

			result = php_io_terminal_key_from_input_record(key, &high_surrogate);
			if (result != NULL) {
				if (key->wRepeatCount > 1) {
					key->wRepeatCount--;
					*pending_key = record;
					*pending_key_high_surrogate = high_surrogate;
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
		return NULL;
	}

	return result;
}

static zend_string *php_io_terminal_read_stream_secret(
	php_io_terminal_native_stream input,
	php_stream *stream,
	INPUT_RECORD *pending_key,
	WCHAR *pending_key_high_surrogate
)
{
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

	for (;;) {
		INPUT_RECORD record;
		KEY_EVENT_RECORD *key;
		WORD repeats;

		if (!php_io_terminal_read_console_record(
				handle,
				INFINITE,
				&record,
				&high_surrogate,
				pending_key,
				pending_key_high_surrogate)) {
			failed = true;
			break;
		}

		if (record.EventType != KEY_EVENT) {
			continue;
		}

		key = &record.Event.KeyEvent;
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

		repeats = key->wRepeatCount > 0 ? key->wRepeatCount : 1;
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

static bool php_io_terminal_enable_stream_raw_mode(php_io_terminal_native_stream fd, php_io_terminal_saved_mode *saved)
{
	struct termios mode;
	struct termios raw_mode;

	if (!php_io_terminal_native_stream_is_valid(fd) || isatty(fd) != 1 || tcgetattr(fd, &mode) != 0) {
		return false;
	}

	memcpy(saved->magic, PHP_IO_TERMINAL_MODE_TOKEN_MAGIC, PHP_IO_TERMINAL_MODE_TOKEN_MAGIC_LEN);
	saved->stream = fd;
	saved->mode = mode;

	raw_mode = mode;
	php_io_terminal_make_raw_mode(&raw_mode);

	if (tcsetattr(fd, TCSANOW, &raw_mode) != 0) {
		return false;
	}

	return true;
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
	struct timespec remaining;
	ssize_t bytes_read;

	if (stream != NULL && stream->writepos > stream->readpos) {
		return php_stream_read(stream, (char *) byte, 1) == 1 ? 1 : -1;
	}

	if (poll_ctx == NULL) {
		return -1;
	}

	if (timeout != NULL) {
		remaining = *timeout;
	}

	for (;;) {
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
			return 0;
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
					return 0;
				}
			}
			continue;
		}

		return -1;
	}

	bytes_read = read(fd, byte, 1);
	if (bytes_read == 0) {
		return 0;
	}
	if (bytes_read < 0) {
		return -1;
	}

	return 1;
}

static zend_string *php_io_terminal_finish_utf8_sequence(
	int fd,
	php_stream *stream,
	php_poll_ctx *poll_ctx,
	php_io_terminal_utf8_pending *pending,
	const struct timespec *first_timeout,
	const struct timespec *sequence_timeout
)
{
	size_t initial_length = pending->length;

	while (pending->length < pending->expected) {
		unsigned char key;
		const struct timespec *timeout = pending->length == initial_length ? first_timeout : sequence_timeout;
		int result = php_io_terminal_read_byte(fd, stream, poll_ctx, &key, timeout, false, NULL);

		if (result != 1) {
			return NULL;
		}

		pending->bytes[pending->length++] = key;
		if ((key & 0xc0) != 0x80) {
			zend_string *invalid = zend_string_init((const char *) pending->bytes, pending->length, false);
			memset(pending, 0, sizeof(*pending));
			return invalid;
		}
	}

	zend_string *result = zend_string_init((const char *) pending->bytes, pending->length, false);
	memset(pending, 0, sizeof(*pending));
	return result;
}

static zend_string *php_io_terminal_key_from_utf8_sequence(
	int fd,
	php_stream *stream,
	php_poll_ctx *poll_ctx,
	unsigned char key,
	const struct timespec *sequence_timeout,
	php_io_terminal_utf8_pending *pending
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

	return php_io_terminal_finish_utf8_sequence(fd, stream, poll_ctx, pending, sequence_timeout, sequence_timeout);
}

static zend_string *php_io_terminal_key_from_escape_sequence(
	int fd,
	php_stream *stream,
	php_poll_ctx *poll_ctx,
	const struct timespec *sequence_timeout
)
{
	unsigned char seq[32];
	size_t seq_len = 0;
	int read_result;

	seq[seq_len++] = 0x1b;

	read_result = php_io_terminal_read_byte(fd, stream, poll_ctx, &seq[seq_len], sequence_timeout, false, NULL);
	if (read_result != 1) {
		return ZSTR_INIT_LITERAL("escape", false);
	}
	seq_len++;

	if (seq[1] == '[') {
		while (seq_len < sizeof(seq)) {
			unsigned char next;

			read_result = php_io_terminal_read_byte(fd, stream, poll_ctx, &next, sequence_timeout, false, NULL);
			if (read_result != 1) {
				break;
			}

			seq[seq_len++] = next;
			if (next >= 0x40 && next <= 0x7e) {
				break;
			}
		}

#define PHP_IO_TERMINAL_CSI_IS(literal) \
	(seq_len == sizeof(literal) - 1 && memcmp(seq, literal, sizeof(literal) - 1) == 0)

		if (PHP_IO_TERMINAL_CSI_IS("\x1b[A")) return ZSTR_INIT_LITERAL("up", false);
		if (PHP_IO_TERMINAL_CSI_IS("\x1b[B")) return ZSTR_INIT_LITERAL("down", false);
		if (PHP_IO_TERMINAL_CSI_IS("\x1b[C")) return ZSTR_INIT_LITERAL("right", false);
		if (PHP_IO_TERMINAL_CSI_IS("\x1b[D")) return ZSTR_INIT_LITERAL("left", false);
		if (PHP_IO_TERMINAL_CSI_IS("\x1b[H") || PHP_IO_TERMINAL_CSI_IS("\x1b[1~")) return ZSTR_INIT_LITERAL("home", false);
		if (PHP_IO_TERMINAL_CSI_IS("\x1b[F") || PHP_IO_TERMINAL_CSI_IS("\x1b[4~")) return ZSTR_INIT_LITERAL("end", false);
		if (PHP_IO_TERMINAL_CSI_IS("\x1b[3~")) return ZSTR_INIT_LITERAL("delete", false);
		if (PHP_IO_TERMINAL_CSI_IS("\x1b[5~")) return ZSTR_INIT_LITERAL("pageup", false);
		if (PHP_IO_TERMINAL_CSI_IS("\x1b[6~")) return ZSTR_INIT_LITERAL("pagedown", false);
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

#undef PHP_IO_TERMINAL_CSI_IS
	} else if (seq[1] == 'O' && seq_len < sizeof(seq)) {
		read_result = php_io_terminal_read_byte(fd, stream, poll_ctx, &seq[seq_len], sequence_timeout, false, NULL);
		if (read_result == 1) {
			seq_len++;
			switch (seq[2]) {
				case 'P': return ZSTR_INIT_LITERAL("f1", false);
				case 'Q': return ZSTR_INIT_LITERAL("f2", false);
				case 'R': return ZSTR_INIT_LITERAL("f3", false);
				case 'S': return ZSTR_INIT_LITERAL("f4", false);
				case 'H': return ZSTR_INIT_LITERAL("home", false);
				case 'F': return ZSTR_INIT_LITERAL("end", false);
			}
		}
	}

	return zend_string_init((const char *) seq, seq_len, false);
}

static zend_string *php_io_terminal_key_from_byte(
	int fd,
	php_stream *stream,
	php_poll_ctx *poll_ctx,
	unsigned char key,
	const struct timespec *sequence_timeout,
	php_io_terminal_utf8_pending *pending
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
			return php_io_terminal_key_from_escape_sequence(fd, stream, poll_ctx, sequence_timeout);
		default:
			return php_io_terminal_key_from_utf8_sequence(fd, stream, poll_ctx, key, sequence_timeout, pending);
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
	struct termios mode;
	struct termios raw_mode;
	unsigned char key;
	int read_result;
	zend_string *result = NULL;
	bool mode_changed;
	sig_atomic_t resize_generation = 0;
	bool resize_handler_installed;
	php_poll_ctx *poll_ctx;

	if (fd < 0 || isatty(fd) != 1 || tcgetattr(fd, &mode) != 0) {
		return NULL;
	}

	poll_ctx = php_io_terminal_create_poll_context(fd);
	if (poll_ctx == NULL) {
		return NULL;
	}

#if defined(SIGWINCH)
	resize_handler_installed = php_io_terminal_install_resize_handler(&resize_generation);
#else
	resize_handler_installed = false;
#endif

	raw_mode = mode;
	php_io_terminal_make_raw_mode(&raw_mode);
	mode_changed = memcmp(&raw_mode, &mode, sizeof(mode)) != 0;

	if (mode_changed && tcsetattr(fd, TCSANOW, &raw_mode) != 0) {
#if defined(SIGWINCH)
		if (resize_handler_installed) {
			php_io_terminal_restore_resize_handler();
		}
#endif
		php_poll_destroy(poll_ctx);
		return NULL;
	}

	if (pending->length > 0) {
		result = php_io_terminal_finish_utf8_sequence(fd, stream, poll_ctx, pending, timeout, sequence_timeout);
	} else {
		read_result = php_io_terminal_read_byte(fd, stream, poll_ctx, &key, timeout, true,
			resize_handler_installed ? &resize_generation : NULL);
		if (read_result == PHP_IO_TERMINAL_READ_RESIZE) {
			result = ZSTR_INIT_LITERAL("resize", false);
		} else if (read_result == 1) {
			result = php_io_terminal_key_from_byte(fd, stream, poll_ctx, key, sequence_timeout, pending);
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

static zend_string *php_io_terminal_read_stream_secret(php_io_terminal_native_stream input, php_stream *stream)
{
	const struct timespec sequence_timeout = {0, PHP_IO_TERMINAL_SEQUENCE_TIMEOUT_MS * 1000000L};
	int fd = input;
	struct termios mode;
	struct termios raw_mode;
	smart_str secret = {0};
	bool success = false;
	bool mode_changed;
	php_poll_ctx *poll_ctx;

	if (fd < 0 || isatty(fd) != 1 || tcgetattr(fd, &mode) != 0) {
		return NULL;
	}

	poll_ctx = php_io_terminal_create_poll_context(fd);
	if (poll_ctx == NULL) {
		return NULL;
	}

	raw_mode = mode;
	php_io_terminal_make_raw_mode(&raw_mode);
	mode_changed = memcmp(&raw_mode, &mode, sizeof(mode)) != 0;

	if (mode_changed && tcsetattr(fd, TCSANOW, &raw_mode) != 0) {
		php_poll_destroy(poll_ctx);
		return NULL;
	}

	for (;;) {
		unsigned char key;
		int result = php_io_terminal_read_byte(fd, stream, poll_ctx, &key, NULL, false, NULL);
		if (result != 1) {
			break;
		}

		switch (key) {
			case '\r':
			case '\n':
				success = true;
				goto restore;
			case 0x7f:
			case '\b':
				php_io_terminal_buffer_remove_last_utf8_char(&secret);
				break;
			case 0x03:
			case 0x04:
				goto restore;
			case 0x1b:
			{
				zend_string *escape_key = php_io_terminal_key_from_escape_sequence(fd, stream, poll_ctx, &sequence_timeout);
				bool is_escape = zend_string_equals_literal(escape_key, "escape");
				zend_string_release(escape_key);
				if (is_escape) {
					goto restore;
				}
				break;
			}
			default:
			{
				size_t seq_len = php_io_terminal_utf8_sequence_len(key);
				if (seq_len == 1) {
					smart_str_appendc(&secret, (char) key);
				} else {
					unsigned char seq[4];
					size_t i;
					seq[0] = key;
					for (i = 1; i < seq_len; i++) {
						if (php_io_terminal_read_byte(fd, stream, poll_ctx, &seq[i], &sequence_timeout, false, NULL) != 1) {
							break;
						}
					}
					smart_str_appendl(&secret, (const char *) seq, i);
				}
				break;
			}
		}
	}

restore:
	if (mode_changed && tcsetattr(fd, TCSANOW, &mode) != 0) {
		success = false;
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

static void php_io_terminal_create_terminal_size(zval *return_value, zend_long cols, zend_long rows)
{
	object_init_ex(return_value, php_io_terminal_terminal_size_ce);
	zend_update_property_long(php_io_terminal_terminal_size_ce, Z_OBJ_P(return_value), "cols", sizeof("cols") - 1, cols);
	zend_update_property_long(php_io_terminal_terminal_size_ce, Z_OBJ_P(return_value), "rows", sizeof("rows") - 1, rows);
}

/* Io\Terminal\TerminalSize methods */
PHP_METHOD(Io_Terminal_TerminalSize, __construct)
{
	zend_throw_error(NULL, "Cannot directly construct Io\\Terminal\\TerminalSize");
}

/* Io\Terminal\ModeToken methods */
PHP_METHOD(Io_Terminal_ModeToken, __construct)
{
	zend_throw_error(NULL, "Cannot directly construct Io\\Terminal\\ModeToken");
}

/* Io\Terminal\Terminal methods */
PHP_METHOD(Io_Terminal_Terminal, __construct)
{
	zend_throw_error(NULL, "Cannot directly construct Io\\Terminal\\Terminal");
}

PHP_METHOD(Io_Terminal_Terminal, create)
{
	ZEND_PARSE_PARAMETERS_NONE();

	object_init_ex(return_value, php_io_terminal_terminal_ce);
}

PHP_METHOD(Io_Terminal_Terminal, fromStreams)
{
	zval *input_arg;
	zval *output_arg = NULL;
	php_io_terminal_object *intern;
	php_io_terminal_stream_target target;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_RESOURCE(input_arg)
		Z_PARAM_OPTIONAL
		Z_PARAM_RESOURCE_OR_NULL(output_arg)
	ZEND_PARSE_PARAMETERS_END();

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

	object_init_ex(return_value, php_io_terminal_terminal_ce);
	intern = PHP_IO_TERMINAL_OBJ_FROM_ZV(return_value);
	ZVAL_COPY(&intern->input_stream_val, input_arg);

	if (output_arg != NULL && !Z_ISNULL_P(output_arg)) {
		ZVAL_COPY(&intern->output_stream_val, output_arg);
	} else {
		ZVAL_COPY(&intern->output_stream_val, input_arg);
	}
}

PHP_METHOD(Io_Terminal_Terminal, getSize)
{
	php_io_terminal_object *intern;
	php_io_terminal_stream_target stream;
	zend_long columns = 0;
	zend_long rows = 0;

	ZEND_PARSE_PARAMETERS_NONE();

	intern = PHP_IO_TERMINAL_OBJ_FROM_ZV(ZEND_THIS);

	if (!php_io_terminal_stream_target_init(&intern->output_stream_val, false, &stream)) {
		RETURN_THROWS();
	}

	if (!php_io_terminal_stream_size(stream.native_stream, &columns, &rows)) {
		RETURN_FALSE;
	}

	php_io_terminal_create_terminal_size(return_value, columns, rows);
}

PHP_METHOD(Io_Terminal_Terminal, enableRawMode)
{
	php_io_terminal_object *intern;
	php_io_terminal_stream_target stream;
	php_io_terminal_saved_mode saved;

	ZEND_PARSE_PARAMETERS_NONE();

	intern = PHP_IO_TERMINAL_OBJ_FROM_ZV(ZEND_THIS);

	if (intern->active_mode_token != NULL) {
		php_io_terminal_mode_token_object *mode = PHP_IO_TERMINAL_MODE_TOKEN_OBJ_FROM_ZOBJ(intern->active_mode_token);
		if (mode->valid) {
			RETURN_OBJ_COPY(intern->active_mode_token);
		}
		OBJ_RELEASE(intern->active_mode_token);
		intern->active_mode_token = NULL;
	}

	if (!php_io_terminal_stream_target_init(&intern->input_stream_val, true, &stream)) {
		RETURN_THROWS();
	}

	if (!php_io_terminal_enable_stream_raw_mode(stream.native_stream, &saved)) {
		RETURN_FALSE;
	}

	php_io_terminal_create_mode_token(return_value, &saved, stream.stream_resource);

	if (Z_TYPE_P(return_value) == IS_OBJECT && instanceof_function(Z_OBJCE_P(return_value), php_io_terminal_mode_token_ce)) {
		intern->active_mode_token = Z_OBJ_P(return_value);
		GC_ADDREF(intern->active_mode_token);
	}
}

PHP_METHOD(Io_Terminal_Terminal, restoreMode)
{
	zval *mode_token = NULL;
	php_io_terminal_object *intern;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS_OR_NULL(mode_token, php_io_terminal_mode_token_ce)
	ZEND_PARSE_PARAMETERS_END();

	intern = PHP_IO_TERMINAL_OBJ_FROM_ZV(ZEND_THIS);

	if (mode_token != NULL && !Z_ISNULL_P(mode_token)) {
		php_io_terminal_mode_token_object *mode = PHP_IO_TERMINAL_MODE_TOKEN_OBJ_FROM_ZV(mode_token);
		bool restored;

		if (!mode->valid || memcmp(mode->saved.magic, PHP_IO_TERMINAL_MODE_TOKEN_MAGIC, PHP_IO_TERMINAL_MODE_TOKEN_MAGIC_LEN) != 0) {
			zend_argument_value_error(1, "must be an active terminal mode token returned by Io\\Terminal\\Terminal::enableRawMode()");
			RETURN_THROWS();
		}

		restored = php_io_terminal_release_mode_token(mode);
		if (restored && intern->active_mode_token != NULL && intern->active_mode_token == Z_OBJ_P(mode_token)) {
			OBJ_RELEASE(intern->active_mode_token);
			intern->active_mode_token = NULL;
		}

		RETURN_BOOL(restored);
	}

	if (intern->active_mode_token != NULL) {
		php_io_terminal_mode_token_object *mode = PHP_IO_TERMINAL_MODE_TOKEN_OBJ_FROM_ZOBJ(intern->active_mode_token);
		bool restored;

		if (!mode->valid || memcmp(mode->saved.magic, PHP_IO_TERMINAL_MODE_TOKEN_MAGIC, PHP_IO_TERMINAL_MODE_TOKEN_MAGIC_LEN) != 0) {
			OBJ_RELEASE(intern->active_mode_token);
			intern->active_mode_token = NULL;
			RETURN_FALSE;
		}

		restored = php_io_terminal_release_mode_token(mode);
		if (restored) {
			OBJ_RELEASE(intern->active_mode_token);
			intern->active_mode_token = NULL;
		}

		RETURN_BOOL(restored);
	}

	RETURN_FALSE;
}

PHP_METHOD(Io_Terminal_Terminal, readKey)
{
	php_date_time_duration *timeout_duration = NULL;
	php_date_time_duration *seq_timeout_duration = NULL;
	php_io_terminal_object *intern;
	php_io_terminal_stream_target stream;
	zend_string *key;
	zend_object *key_case;

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

	intern = PHP_IO_TERMINAL_OBJ_FROM_ZV(ZEND_THIS);

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
		} else if (sec >= (DWORD) INFINITE / 1000) {
			wait_ms = INFINITE;
		} else {
			uint64_t ms = sec * 1000 + (nsec + 999999) / 1000000;
			wait_ms = ms >= (DWORD) INFINITE ? INFINITE : (DWORD) ms;
		}
	}

	key = php_io_terminal_read_stream_key(
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

	if (timeout_duration != NULL) {
		timeout.tv_sec = timeout_duration->duration.seconds;
		timeout.tv_nsec = timeout_duration->duration.nanoseconds;
		timeout_ptr = &timeout;
	}

	if (seq_timeout_duration != NULL) {
		sequence_timeout.tv_sec = seq_timeout_duration->duration.seconds;
		sequence_timeout.tv_nsec = seq_timeout_duration->duration.nanoseconds;
	}

	key = php_io_terminal_read_stream_key(
		stream.native_stream,
		stream.php_stream,
		timeout_ptr,
		&sequence_timeout,
		&intern->pending_utf8
	);
#endif

	if (key == NULL) {
		RETURN_FALSE;
	}

	key_case = php_io_terminal_key_enum_from_string(key);
	if (key_case != NULL) {
		zend_string_release(key);
		RETURN_OBJ_COPY(key_case);
	}

	RETURN_STR(key);
}

PHP_METHOD(Io_Terminal_Terminal, readSecret)
{
	php_io_terminal_object *intern;
	php_io_terminal_stream_target stream;
	zend_string *secret;

	ZEND_PARSE_PARAMETERS_NONE();

	intern = PHP_IO_TERMINAL_OBJ_FROM_ZV(ZEND_THIS);

#ifdef PHP_WIN32
	memset(&intern->pending_key, 0, sizeof(intern->pending_key));
	intern->pending_key_high_surrogate = 0;
	intern->pending_high_surrogate = 0;
#else
	memset(&intern->pending_utf8, 0, sizeof(intern->pending_utf8));
#endif

	if (!php_io_terminal_stream_target_init(&intern->input_stream_val, true, &stream)) {
		RETURN_THROWS();
	}

#ifdef PHP_WIN32
	secret = php_io_terminal_read_stream_secret(
		stream.native_stream,
		stream.php_stream,
		&intern->pending_key,
		&intern->pending_key_high_surrogate
	);
#else
	secret = php_io_terminal_read_stream_secret(stream.native_stream, stream.php_stream);
#endif
	if (secret != NULL) {
		RETURN_STR(secret);
	}

	zend_throw_exception(php_io_terminal_exception_ce, "Unable to read secret from terminal", 0);
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
	php_io_terminal_mode_token_ce->create_object = php_io_terminal_mode_token_create_object;
	memcpy(&php_io_terminal_mode_token_handlers, zend_get_std_object_handlers(), sizeof(php_io_terminal_mode_token_handlers));
	php_io_terminal_mode_token_handlers.offset = offsetof(php_io_terminal_mode_token_object, std);
	php_io_terminal_mode_token_handlers.free_obj = php_io_terminal_mode_token_free_obj;
	php_io_terminal_mode_token_handlers.clone_obj = NULL;

	php_io_terminal_terminal_ce = register_class_Io_Terminal_Terminal();
	php_io_terminal_terminal_ce->create_object = php_io_terminal_create_object;
	memcpy(&php_io_terminal_object_handlers, zend_get_std_object_handlers(), sizeof(php_io_terminal_object_handlers));
	php_io_terminal_object_handlers.offset = offsetof(php_io_terminal_object, std);
	php_io_terminal_object_handlers.free_obj = php_io_terminal_free_obj;
	php_io_terminal_object_handlers.clone_obj = NULL;

#if !defined(PHP_WIN32) && defined(SIGWINCH) && defined(ZTS)
	php_io_terminal_resize_mutex = tsrm_mutex_alloc();
#endif

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(terminal)
{
#if !defined(PHP_WIN32) && defined(SIGWINCH) && defined(ZTS)
	if (php_io_terminal_resize_mutex != NULL) {
		tsrm_mutex_free(php_io_terminal_resize_mutex);
		php_io_terminal_resize_mutex = NULL;
	}
#endif

	return SUCCESS;
}

PHP_RINIT_FUNCTION(terminal)
{
	php_io_terminal_active_mode_tokens = NULL;
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(terminal)
{
	while (php_io_terminal_active_mode_tokens != NULL) {
		php_io_terminal_mode_token_object *mode = php_io_terminal_active_mode_tokens;
		if (mode->valid && memcmp(mode->saved.magic, PHP_IO_TERMINAL_MODE_TOKEN_MAGIC, PHP_IO_TERMINAL_MODE_TOKEN_MAGIC_LEN) == 0) {
			if (!php_io_terminal_release_mode_token(mode)) {
				php_io_terminal_untrack_mode_token(mode);
				memset(&mode->saved, 0, sizeof(mode->saved));
				mode->valid = false;
			}
		} else {
			php_io_terminal_untrack_mode_token(mode);
		}
	}
	return SUCCESS;
}
