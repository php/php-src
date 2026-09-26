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
#include "php_streams.h"
#include "ext/standard/basic_functions.h"
#include "ext/date/php_time.h"
#include "ext/spl/spl_exceptions.h"
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
# include <sys/select.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <termios.h>
# include <time.h>
# include <unistd.h>
# include <sys/ioctl.h>
#endif

#define TERMINAL_MODE_TOKEN_MAGIC "PHPTTY1"
#define TERMINAL_MODE_TOKEN_MAGIC_LEN (sizeof(TERMINAL_MODE_TOKEN_MAGIC) - 1)
#define TERMINAL_SEQUENCE_TIMEOUT_MS 25

#ifdef PHP_WIN32
typedef HANDLE terminal_native_stream;
ZEND_TLS INPUT_RECORD terminal_pending_key;
ZEND_TLS WCHAR terminal_pending_high_surrogate;
#else
typedef int terminal_native_stream;

typedef struct _terminal_utf8_pending {
	unsigned char bytes[4];
	size_t length;
	size_t expected;
} terminal_utf8_pending;
#endif

typedef struct _terminal_saved_mode {
	char magic[TERMINAL_MODE_TOKEN_MAGIC_LEN];
	terminal_native_stream stream;
#ifdef PHP_WIN32
	DWORD mode;
#else
	struct termios mode;
#endif
} terminal_saved_mode;

typedef struct _terminal_mode_token_object {
	terminal_saved_mode saved;
	zval stream_resource;
	struct _terminal_mode_token_object *active_prev;
	struct _terminal_mode_token_object *active_next;
	bool valid;
	bool tracked;
	zend_object std;
} terminal_mode_token_object;

typedef struct _terminal_object {
	zval input_stream_val;
	zval output_stream_val;
	zend_object *active_mode_token;
#ifdef PHP_WIN32
	WCHAR pending_high_surrogate;
#else
	terminal_utf8_pending pending_utf8;
#endif
	zend_object std;
} terminal_object;

typedef struct _terminal_stream_target {
	terminal_native_stream native_stream;
	php_stream *php_stream;
	zval *stream_resource;
	bool is_default;
} terminal_stream_target;

static zend_class_entry *terminal_key_ce;
static zend_class_entry *terminal_terminal_size_ce;
static zend_class_entry *terminal_mode_token_ce;
static zend_class_entry *terminal_terminal_ce;
static zend_object_handlers terminal_mode_token_handlers;
static zend_object_handlers terminal_object_handlers;
ZEND_TLS terminal_mode_token_object *terminal_active_mode_tokens;

#if !defined(PHP_WIN32)
#define TERMINAL_READ_RESIZE 2

#ifdef SIGWINCH
static volatile sig_atomic_t terminal_resize_generation = 0;
static unsigned int terminal_resize_readers = 0;
static struct sigaction terminal_previous_resize_action;
#ifdef ZTS
static MUTEX_T terminal_resize_mutex = NULL;
#endif

static void terminal_resize_lock(void)
{
#ifdef ZTS
	if (terminal_resize_mutex != NULL) {
		tsrm_mutex_lock(terminal_resize_mutex);
	}
#endif
}

static void terminal_resize_unlock(void)
{
#ifdef ZTS
	if (terminal_resize_mutex != NULL) {
		tsrm_mutex_unlock(terminal_resize_mutex);
	}
#endif
}

static void terminal_sigwinch_handler(int signo)
{
	(void) signo;

	terminal_resize_generation = terminal_resize_generation == SIG_ATOMIC_MAX
		? 0
		: terminal_resize_generation + 1;
}

static bool terminal_install_resize_handler(sig_atomic_t *generation)
{
	struct sigaction action;
	bool installed = true;
	sig_atomic_t current_generation;

	terminal_resize_lock();
	current_generation = terminal_resize_generation;

	if (terminal_resize_readers == 0) {
		memset(&action, 0, sizeof(action));
		action.sa_handler = terminal_sigwinch_handler;
		sigemptyset(&action.sa_mask);

		installed = sigaction(SIGWINCH, &action, &terminal_previous_resize_action) == 0;
	}

	if (installed) {
		terminal_resize_readers++;
		*generation = current_generation;
	}

	terminal_resize_unlock();

	return installed;
}

static void terminal_restore_resize_handler(void)
{
	terminal_resize_lock();

	if (terminal_resize_readers > 0 && --terminal_resize_readers == 0) {
		sigaction(SIGWINCH, &terminal_previous_resize_action, NULL);
	}

	terminal_resize_unlock();
}
#endif /* SIGWINCH */
#endif /* !PHP_WIN32 */

static inline terminal_mode_token_object *terminal_mode_token_from_obj(zend_object *obj)
{
	return (terminal_mode_token_object *) ((char *) obj - offsetof(terminal_mode_token_object, std));
}

#define Z_TERMINAL_MODE_TOKEN_P(zv) terminal_mode_token_from_obj(Z_OBJ_P(zv))

static inline terminal_object *terminal_from_obj(zend_object *obj)
{
	return (terminal_object *) ((char *) obj - offsetof(terminal_object, std));
}

#define Z_TERMINAL_P(zv) terminal_from_obj(Z_OBJ_P(zv))

static bool terminal_native_stream_is_valid(terminal_native_stream stream)
{
#ifdef PHP_WIN32
	return stream != INVALID_HANDLE_VALUE && stream != NULL;
#else
	return stream >= 0;
#endif
}

static bool terminal_mode_streams_match(terminal_native_stream first, terminal_native_stream second)
{
	return first == second;
}

static bool terminal_mode_token_stream_is_valid(const terminal_mode_token_object *mode)
{
	if (!Z_ISUNDEF(mode->stream_resource)) {
		if (Z_TYPE(mode->stream_resource) != IS_RESOURCE || Z_RES(mode->stream_resource) == NULL) {
			return false;
		}
	}

	return terminal_native_stream_is_valid(mode->saved.stream);
}

static bool terminal_restore_stream_mode(const terminal_saved_mode *saved)
{
	if (!terminal_native_stream_is_valid(saved->stream)) {
		return false;
	}

#ifdef PHP_WIN32
	return SetConsoleMode(saved->stream, saved->mode) != 0;
#else
	return tcsetattr(saved->stream, TCSANOW, &saved->mode) == 0;
#endif
}

static void terminal_untrack_mode_token(terminal_mode_token_object *mode)
{
	if (!mode->tracked) {
		return;
	}

	if (mode->active_prev != NULL) {
		mode->active_prev->active_next = mode->active_next;
	} else {
		terminal_active_mode_tokens = mode->active_next;
	}

	if (mode->active_next != NULL) {
		mode->active_next->active_prev = mode->active_prev;
	}

	mode->active_prev = NULL;
	mode->active_next = NULL;
	mode->tracked = false;
}

static void terminal_track_mode_token(terminal_mode_token_object *mode)
{
	mode->active_prev = NULL;
	mode->active_next = terminal_active_mode_tokens;
	if (terminal_active_mode_tokens != NULL) {
		terminal_active_mode_tokens->active_prev = mode;
	}
	terminal_active_mode_tokens = mode;
	mode->tracked = true;
}

static bool terminal_release_mode_token(terminal_mode_token_object *mode)
{
	terminal_mode_token_object *candidate = terminal_active_mode_tokens;
	terminal_mode_token_object *newer = NULL;
	bool restored;

	while (candidate != NULL && candidate != mode) {
		if (candidate->valid && terminal_mode_streams_match(candidate->saved.stream, mode->saved.stream)) {
			newer = candidate;
		}
		candidate = candidate->active_next;
	}

	if (candidate != mode) {
		newer = NULL;
	}

	if (newer != NULL) {
		newer->saved.mode = mode->saved.mode;
		terminal_untrack_mode_token(mode);
		memset(&mode->saved, 0, sizeof(mode->saved));
		mode->valid = false;
		return true;
	}

	if (!terminal_mode_token_stream_is_valid(mode)) {
		return false;
	}

	restored = terminal_restore_stream_mode(&mode->saved);
	if (restored) {
		terminal_untrack_mode_token(mode);
		memset(&mode->saved, 0, sizeof(mode->saved));
		mode->valid = false;
	}

	return restored;
}

static void terminal_mode_token_free_obj(zend_object *object)
{
	terminal_mode_token_object *intern = terminal_mode_token_from_obj(object);

	if (intern->valid && memcmp(intern->saved.magic, TERMINAL_MODE_TOKEN_MAGIC, TERMINAL_MODE_TOKEN_MAGIC_LEN) == 0) {
		terminal_release_mode_token(intern);
	}
	terminal_untrack_mode_token(intern);

	if (!Z_ISUNDEF(intern->stream_resource)) {
		zval_ptr_dtor(&intern->stream_resource);
		ZVAL_UNDEF(&intern->stream_resource);
	}

	memset(&intern->saved, 0, sizeof(intern->saved));
	intern->valid = false;

	zend_object_std_dtor(&intern->std);
}

static zend_object *terminal_mode_token_create_object(zend_class_entry *ce)
{
	terminal_mode_token_object *intern = zend_object_alloc(sizeof(terminal_mode_token_object), ce);

	memset(&intern->saved, 0, sizeof(intern->saved));
	ZVAL_UNDEF(&intern->stream_resource);
	intern->active_prev = NULL;
	intern->active_next = NULL;
	intern->valid = false;
	intern->tracked = false;

	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);
	intern->std.handlers = &terminal_mode_token_handlers;

	return &intern->std;
}

static void terminal_create_mode_token(zval *return_value, const terminal_saved_mode *saved, zval *stream_resource)
{
	terminal_mode_token_object *intern;

	object_init_ex(return_value, terminal_mode_token_ce);
	intern = Z_TERMINAL_MODE_TOKEN_P(return_value);
	intern->saved = *saved;
	if (stream_resource != NULL && !Z_ISUNDEF_P(stream_resource)) {
		ZVAL_COPY(&intern->stream_resource, stream_resource);
	}
	intern->valid = true;
	terminal_track_mode_token(intern);
}

static void terminal_free_obj(zend_object *object)
{
	terminal_object *intern = terminal_from_obj(object);

	if (intern->active_mode_token != NULL) {
		terminal_mode_token_object *mode = terminal_mode_token_from_obj(intern->active_mode_token);
		if (mode->valid && memcmp(mode->saved.magic, TERMINAL_MODE_TOKEN_MAGIC, TERMINAL_MODE_TOKEN_MAGIC_LEN) == 0) {
			terminal_release_mode_token(mode);
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

static zend_object *terminal_create_object(zend_class_entry *ce)
{
	terminal_object *intern = zend_object_alloc(sizeof(terminal_object), ce);

	ZVAL_UNDEF(&intern->input_stream_val);
	ZVAL_UNDEF(&intern->output_stream_val);
	intern->active_mode_token = NULL;
#ifdef PHP_WIN32
	intern->pending_high_surrogate = 0;
#else
	memset(&intern->pending_utf8, 0, sizeof(intern->pending_utf8));
#endif

	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);
	intern->std.handlers = &terminal_object_handlers;

	return &intern->std;
}

static terminal_native_stream terminal_native_stream_from_php_stream(php_stream *stream)
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

static bool terminal_stream_target_init(
	zval *stream_arg,
	bool is_input,
	terminal_stream_target *target
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
		target->native_stream = terminal_native_stream_from_php_stream(target->php_stream);
		return true;
	}

	return false;
}

#ifdef PHP_WIN32
static DWORD terminal_make_raw_mode(DWORD mode)
{
	return (mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT));
}

static bool terminal_enable_stream_raw_mode(terminal_native_stream handle, terminal_saved_mode *saved)
{
	DWORD mode;
	DWORD raw_mode;

	if (!terminal_native_stream_is_valid(handle) || !GetConsoleMode(handle, &mode)) {
		return false;
	}

	memcpy(saved->magic, TERMINAL_MODE_TOKEN_MAGIC, TERMINAL_MODE_TOKEN_MAGIC_LEN);
	saved->stream = handle;
	saved->mode = mode;

	raw_mode = terminal_make_raw_mode(mode) | ENABLE_WINDOW_INPUT;
	if (!SetConsoleMode(handle, raw_mode)) {
		return false;
	}

	return true;
}

static bool terminal_stream_size(terminal_native_stream handle, zend_long *columns, zend_long *rows)
{
	CONSOLE_SCREEN_BUFFER_INFO info;

	if (!terminal_native_stream_is_valid(handle) || !GetConsoleScreenBufferInfo(handle, &info)) {
		return false;
	}

	*columns = (zend_long) (info.srWindow.Right - info.srWindow.Left + 1);
	*rows = (zend_long) (info.srWindow.Bottom - info.srWindow.Top + 1);

	return true;
}

static zend_string *terminal_key_from_virtual_key(WORD vk)
{
	switch (vk) {
		case VK_UP:
			return zend_string_init("up", sizeof("up") - 1, false);
		case VK_DOWN:
			return zend_string_init("down", sizeof("down") - 1, false);
		case VK_RIGHT:
			return zend_string_init("right", sizeof("right") - 1, false);
		case VK_LEFT:
			return zend_string_init("left", sizeof("left") - 1, false);
		case VK_RETURN:
			return zend_string_init("enter", sizeof("enter") - 1, false);
		case VK_BACK:
			return zend_string_init("backspace", sizeof("backspace") - 1, false);
		case VK_ESCAPE:
			return zend_string_init("escape", sizeof("escape") - 1, false);
		case VK_TAB:
			return zend_string_init("tab", sizeof("tab") - 1, false);
		case VK_HOME:
			return zend_string_init("home", sizeof("home") - 1, false);
		case VK_END:
			return zend_string_init("end", sizeof("end") - 1, false);
		case VK_DELETE:
			return zend_string_init("delete", sizeof("delete") - 1, false);
		case VK_PRIOR:
			return zend_string_init("pageup", sizeof("pageup") - 1, false);
		case VK_NEXT:
			return zend_string_init("pagedown", sizeof("pagedown") - 1, false);
		case VK_F1:
			return zend_string_init("f1", sizeof("f1") - 1, false);
		case VK_F2:
			return zend_string_init("f2", sizeof("f2") - 1, false);
		case VK_F3:
			return zend_string_init("f3", sizeof("f3") - 1, false);
		case VK_F4:
			return zend_string_init("f4", sizeof("f4") - 1, false);
		case VK_F5:
			return zend_string_init("f5", sizeof("f5") - 1, false);
		case VK_F6:
			return zend_string_init("f6", sizeof("f6") - 1, false);
		case VK_F7:
			return zend_string_init("f7", sizeof("f7") - 1, false);
		case VK_F8:
			return zend_string_init("f8", sizeof("f8") - 1, false);
		case VK_F9:
			return zend_string_init("f9", sizeof("f9") - 1, false);
		case VK_F10:
			return zend_string_init("f10", sizeof("f10") - 1, false);
		case VK_F11:
			return zend_string_init("f11", sizeof("f11") - 1, false);
		case VK_F12:
			return zend_string_init("f12", sizeof("f12") - 1, false);
		default:
			return NULL;
	}
}

static zend_string *terminal_key_from_wchar(WCHAR ch, WCHAR *high_surrogate)
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

static zend_string *terminal_key_from_input_record(const KEY_EVENT_RECORD *key, WCHAR *high_surrogate)
{
	zend_string *named_key = terminal_key_from_virtual_key(key->wVirtualKeyCode);
	if (named_key != NULL) {
		*high_surrogate = 0;
		return named_key;
	}
	return terminal_key_from_wchar(key->uChar.UnicodeChar, high_surrogate);
}

static bool terminal_read_console_record(HANDLE handle, DWORD wait_ms, INPUT_RECORD *record, WCHAR *high_surrogate)
{
	DWORD records_read;

	if (terminal_pending_key.Event.KeyEvent.wRepeatCount > 0) {
		*record = terminal_pending_key;
		*high_surrogate = terminal_pending_high_surrogate;
		terminal_pending_key.Event.KeyEvent.wRepeatCount = 0;
		return true;
	}

	return WaitForSingleObject(handle, wait_ms) == WAIT_OBJECT_0
		&& ReadConsoleInputW(handle, record, 1, &records_read) && records_read == 1;
}

static zend_string *terminal_read_stream_key(terminal_native_stream input, php_stream *stream, DWORD wait_ms, WCHAR *pending_high_surrogate)
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

	raw_mode = terminal_make_raw_mode(mode);
	if (raw_mode != mode && !SetConsoleMode(handle, raw_mode)) {
		return NULL;
	}
	mode_changed = raw_mode != mode;

	for (;;) {
		INPUT_RECORD record;
		DWORD remaining_ms = INFINITE;

		if (wait_ms != INFINITE) {
			ULONGLONG now = GetTickCount64();
			if (now >= deadline_ms) {
				break;
			}
			remaining_ms = (DWORD) (deadline_ms - now);
		}

		if (!terminal_read_console_record(handle, remaining_ms, &record, &high_surrogate)) {
			break;
		}

		if (record.EventType == WINDOW_BUFFER_SIZE_EVENT) {
			result = zend_string_init("resize", sizeof("resize") - 1, false);
			break;
		}

		if (record.EventType == KEY_EVENT) {
			KEY_EVENT_RECORD *key = &record.Event.KeyEvent;
			if (!key->bKeyDown) {
				continue;
			}

			result = terminal_key_from_input_record(key, &high_surrogate);
			if (result != NULL) {
				if (key->wRepeatCount > 1) {
					key->wRepeatCount--;
					terminal_pending_key = record;
					terminal_pending_high_surrogate = high_surrogate;
				}
				break;
			}
		}
	}

	*pending_high_surrogate = high_surrogate;

	if (mode_changed) {
		SetConsoleMode(handle, mode);
	}

	return result;
}

static zend_string *terminal_read_stream_secret(terminal_native_stream input, php_stream *stream)
{
	HANDLE handle = input;
	DWORD mode;
	WCHAR high_surrogate = 0;
	DWORD raw_mode;
	smart_str secret = {0};
	bool success = false;
	bool failed = false;
	bool mode_changed = false;

	if (handle == INVALID_HANDLE_VALUE || handle == NULL
		|| (stream != NULL && stream->writepos > stream->readpos)
		|| !GetConsoleMode(handle, &mode)) {
		return NULL;
	}

	raw_mode = terminal_make_raw_mode(mode);
	if (raw_mode != mode && !SetConsoleMode(handle, raw_mode)) {
		return NULL;
	}
	mode_changed = raw_mode != mode;

	for (;;) {
		INPUT_RECORD record;
		KEY_EVENT_RECORD *key;
		WORD repeats;

		if (!terminal_read_console_record(handle, INFINITE, &record, &high_surrogate)) {
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
			|| (key->wVirtualKeyCode == 'C' && (key->dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) != 0)) {
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
				zend_string *utf8 = terminal_key_from_wchar(key->uChar.UnicodeChar, &high_surrogate);
				if (utf8 != NULL) {
					smart_str_append(&secret, utf8);
					zend_string_release(utf8);
				}
			}
		}
	}

	if (mode_changed) {
		SetConsoleMode(handle, mode);
	}

	if (success && !failed) {
		return secret.s != NULL ? smart_str_extract(&secret) : zend_empty_string;
	}

	smart_str_free(&secret);
	return NULL;
}
#else
/* POSIX implementation */

static void terminal_make_raw_mode(struct termios *mode)
{
	mode->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	mode->c_oflag &= ~OPOST;
	mode->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	mode->c_cflag &= ~(CSIZE | PARENB);
	mode->c_cflag |= CS8;
	mode->c_cc[VMIN] = 1;
	mode->c_cc[VTIME] = 0;
}

static bool terminal_enable_stream_raw_mode(terminal_native_stream fd, terminal_saved_mode *saved)
{
	struct termios mode;
	struct termios raw_mode;

	if (!terminal_native_stream_is_valid(fd) || isatty(fd) != 1 || tcgetattr(fd, &mode) != 0) {
		return false;
	}

	memcpy(saved->magic, TERMINAL_MODE_TOKEN_MAGIC, TERMINAL_MODE_TOKEN_MAGIC_LEN);
	saved->stream = fd;
	saved->mode = mode;

	raw_mode = mode;
	terminal_make_raw_mode(&raw_mode);

	if (tcsetattr(fd, TCSANOW, &raw_mode) != 0) {
		return false;
	}

	return true;
}

static bool terminal_stream_size(terminal_native_stream fd, zend_long *columns, zend_long *rows)
{
	struct winsize ws;

	if (!terminal_native_stream_is_valid(fd)) {
		return false;
	}

	if (ioctl(fd, TIOCGWINSZ, &ws) != 0 || ws.ws_col == 0 || ws.ws_row == 0) {
		return false;
	}

	*columns = (zend_long) ws.ws_col;
	*rows = (zend_long) ws.ws_row;

	return true;
}

static size_t terminal_utf8_sequence_len(unsigned char byte)
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

static void terminal_buffer_remove_last_utf8_char(smart_str *secret)
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

static int terminal_read_byte(int fd, php_stream *stream, unsigned char *byte, int timeout_ms, bool allow_resize, const sig_atomic_t *generation)
{
	fd_set read_fds;
	struct timeval tv;
	struct timeval *tv_ptr = NULL;
	int select_result;
	ssize_t bytes_read;

	if (stream != NULL && stream->writepos > stream->readpos) {
		return php_stream_read(stream, (char *) byte, 1) == 1 ? 1 : -1;
	}

	for (;;) {
#if defined(SIGWINCH)
		if (allow_resize && generation != NULL && *generation != terminal_resize_generation) {
			return TERMINAL_READ_RESIZE;
		}
#endif

		FD_ZERO(&read_fds);
		FD_SET(fd, &read_fds);

		if (timeout_ms >= 0) {
			tv.tv_sec = timeout_ms / 1000;
			tv.tv_usec = (timeout_ms % 1000) * 1000;
			tv_ptr = &tv;
		} else {
			tv_ptr = NULL;
		}

		select_result = select(fd + 1, &read_fds, NULL, NULL, tv_ptr);
		if (select_result > 0) {
			break;
		}

		if (select_result == 0) {
			return 0;
		}

		if (errno == EINTR) {
#if defined(SIGWINCH)
			if (allow_resize && generation != NULL && *generation != terminal_resize_generation) {
				return TERMINAL_READ_RESIZE;
			}
#endif
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

static zend_string *terminal_finish_utf8_sequence(int fd, php_stream *stream, terminal_utf8_pending *pending, int first_timeout_ms, int sequence_timeout_ms)
{
	size_t initial_length = pending->length;

	while (pending->length < pending->expected) {
		unsigned char key;
		int timeout_ms = pending->length == initial_length ? first_timeout_ms : sequence_timeout_ms;
		int result = terminal_read_byte(fd, stream, &key, timeout_ms, false, NULL);

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

static zend_string *terminal_key_from_utf8_sequence(int fd, php_stream *stream, unsigned char key, int sequence_timeout_ms, terminal_utf8_pending *pending)
{
	size_t sequence_len = terminal_utf8_sequence_len(key);

	if (sequence_len == 1) {
		return zend_string_init((const char *) &key, 1, false);
	}

	memset(pending, 0, sizeof(*pending));
	pending->bytes[0] = key;
	pending->length = 1;
	pending->expected = sequence_len;

	return terminal_finish_utf8_sequence(fd, stream, pending, sequence_timeout_ms, sequence_timeout_ms);
}

static zend_string *terminal_key_from_escape_sequence(int fd, php_stream *stream, int sequence_timeout_ms)
{
	unsigned char seq[16];
	size_t seq_len = 0;
	int read_result;

	seq[seq_len++] = 0x1b;

	read_result = terminal_read_byte(fd, stream, &seq[seq_len], sequence_timeout_ms, false, NULL);
	if (read_result != 1) {
		return zend_string_init("escape", sizeof("escape") - 1, false);
	}
	seq_len++;

	if (seq[1] == '[') {
		read_result = terminal_read_byte(fd, stream, &seq[seq_len], sequence_timeout_ms, false, NULL);
		if (read_result != 1) {
			return zend_string_init((const char *) seq, seq_len, false);
		}
		seq_len++;

		switch (seq[2]) {
			case 'A': return zend_string_init("up", sizeof("up") - 1, false);
			case 'B': return zend_string_init("down", sizeof("down") - 1, false);
			case 'C': return zend_string_init("right", sizeof("right") - 1, false);
			case 'D': return zend_string_init("left", sizeof("left") - 1, false);
			case 'H': return zend_string_init("home", sizeof("home") - 1, false);
			case 'F': return zend_string_init("end", sizeof("end") - 1, false);
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			{
				unsigned char next;
				read_result = terminal_read_byte(fd, stream, &next, sequence_timeout_ms, false, NULL);
				if (read_result == 1) {
					seq[seq_len++] = next;
					if (next == '~') {
						switch (seq[2]) {
							case '1': return zend_string_init("home", sizeof("home") - 1, false);
							case '2': return zend_string_init("insert", sizeof("insert") - 1, false);
							case '3': return zend_string_init("delete", sizeof("delete") - 1, false);
							case '4': return zend_string_init("end", sizeof("end") - 1, false);
							case '5': return zend_string_init("pageup", sizeof("pageup") - 1, false);
							case '6': return zend_string_init("pagedown", sizeof("pagedown") - 1, false);
						}
					} else if (seq[2] == '1' || seq[2] == '2') {
						unsigned char terminator;
						read_result = terminal_read_byte(fd, stream, &terminator, sequence_timeout_ms, false, NULL);
						if (read_result == 1 && terminator == '~') {
							seq[seq_len++] = terminator;
							if (seq[2] == '1') {
								switch (next) {
									case '1': return zend_string_init("f1", sizeof("f1") - 1, false);
									case '2': return zend_string_init("f2", sizeof("f2") - 1, false);
									case '3': return zend_string_init("f3", sizeof("f3") - 1, false);
									case '4': return zend_string_init("f4", sizeof("f4") - 1, false);
									case '5': return zend_string_init("f5", sizeof("f5") - 1, false);
									case '7': return zend_string_init("f6", sizeof("f6") - 1, false);
									case '8': return zend_string_init("f7", sizeof("f7") - 1, false);
									case '9': return zend_string_init("f8", sizeof("f8") - 1, false);
								}
							} else if (seq[2] == '2') {
								switch (next) {
									case '0': return zend_string_init("f9", sizeof("f9") - 1, false);
									case '1': return zend_string_init("f10", sizeof("f10") - 1, false);
									case '3': return zend_string_init("f11", sizeof("f11") - 1, false);
									case '4': return zend_string_init("f12", sizeof("f12") - 1, false);
								}
							}
						}
					}
				}
				break;
			}
		}
	} else if (seq[1] == 'O') {
		read_result = terminal_read_byte(fd, stream, &seq[seq_len], sequence_timeout_ms, false, NULL);
		if (read_result == 1) {
			seq_len++;
			switch (seq[2]) {
				case 'P': return zend_string_init("f1", sizeof("f1") - 1, false);
				case 'Q': return zend_string_init("f2", sizeof("f2") - 1, false);
				case 'R': return zend_string_init("f3", sizeof("f3") - 1, false);
				case 'S': return zend_string_init("f4", sizeof("f4") - 1, false);
				case 'H': return zend_string_init("home", sizeof("home") - 1, false);
				case 'F': return zend_string_init("end", sizeof("end") - 1, false);
			}
		}
	}

	return zend_string_init((const char *) seq, seq_len, false);
}

static zend_string *terminal_key_from_byte(int fd, php_stream *stream, unsigned char key, int sequence_timeout_ms, terminal_utf8_pending *pending)
{
	switch (key) {
		case '\r':
		case '\n':
			return zend_string_init("enter", sizeof("enter") - 1, false);
		case '\t':
			return zend_string_init("tab", sizeof("tab") - 1, false);
		case 0x7f:
		case '\b':
			return zend_string_init("backspace", sizeof("backspace") - 1, false);
		case 0x1b:
			return terminal_key_from_escape_sequence(fd, stream, sequence_timeout_ms);
		default:
			return terminal_key_from_utf8_sequence(fd, stream, key, sequence_timeout_ms, pending);
	}
}

static zend_string *terminal_read_stream_key(terminal_native_stream input, php_stream *stream, int timeout_ms, int sequence_timeout_ms, terminal_utf8_pending *pending)
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

	if (fd < 0 || isatty(fd) != 1 || tcgetattr(fd, &mode) != 0) {
		return NULL;
	}

#if defined(SIGWINCH)
	resize_handler_installed = terminal_install_resize_handler(&resize_generation);
#else
	resize_handler_installed = false;
#endif

	raw_mode = mode;
	terminal_make_raw_mode(&raw_mode);
	mode_changed = memcmp(&raw_mode, &mode, sizeof(mode)) != 0;

	if (mode_changed && tcsetattr(fd, TCSANOW, &raw_mode) != 0) {
#if defined(SIGWINCH)
		if (resize_handler_installed) {
			terminal_restore_resize_handler();
		}
#endif
		return NULL;
	}

	if (pending->length > 0) {
		result = terminal_finish_utf8_sequence(fd, stream, pending, timeout_ms, sequence_timeout_ms);
	} else {
		read_result = terminal_read_byte(fd, stream, &key, timeout_ms, true,
			resize_handler_installed ? &resize_generation : NULL);
		if (read_result == TERMINAL_READ_RESIZE) {
			result = zend_string_init("resize", sizeof("resize") - 1, false);
		} else if (read_result == 1) {
			result = terminal_key_from_byte(fd, stream, key, sequence_timeout_ms, pending);
		}
	}

	if (mode_changed && tcsetattr(fd, TCSANOW, &mode) != 0) {
#if defined(SIGWINCH)
		if (resize_handler_installed) {
			terminal_restore_resize_handler();
		}
#endif
		if (result != NULL) {
			zend_string_release(result);
		}
		return NULL;
	}

#if defined(SIGWINCH)
	if (resize_handler_installed) {
		terminal_restore_resize_handler();
	}
#endif

	return result;
}

static zend_string *terminal_read_stream_secret(terminal_native_stream input, php_stream *stream)
{
	int fd = input;
	struct termios mode;
	struct termios raw_mode;
	smart_str secret = {0};
	bool success = false;
	bool mode_changed;

	if (fd < 0 || isatty(fd) != 1 || tcgetattr(fd, &mode) != 0) {
		return NULL;
	}

	raw_mode = mode;
	terminal_make_raw_mode(&raw_mode);
	mode_changed = memcmp(&raw_mode, &mode, sizeof(mode)) != 0;

	if (mode_changed && tcsetattr(fd, TCSANOW, &raw_mode) != 0) {
		return NULL;
	}

	for (;;) {
		unsigned char key;
		int result = terminal_read_byte(fd, stream, &key, -1, false, NULL);
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
				terminal_buffer_remove_last_utf8_char(&secret);
				break;
			case 0x03:
			case 0x04:
				goto restore;
			case 0x1b:
			{
				zend_string *escape_key = terminal_key_from_escape_sequence(fd, stream, TERMINAL_SEQUENCE_TIMEOUT_MS);
				bool is_escape = zend_string_equals_literal(escape_key, "escape");
				zend_string_release(escape_key);
				if (is_escape) {
					goto restore;
				}
				break;
			}
			default:
			{
				size_t seq_len = terminal_utf8_sequence_len(key);
				if (seq_len == 1) {
					smart_str_appendc(&secret, (char) key);
				} else {
					unsigned char seq[4];
					size_t i;
					seq[0] = key;
					for (i = 1; i < seq_len; i++) {
						if (terminal_read_byte(fd, stream, &seq[i], TERMINAL_SEQUENCE_TIMEOUT_MS, false, NULL) != 1) {
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
	if (mode_changed) {
		tcsetattr(fd, TCSANOW, &mode);
	}

	if (success) {
		return secret.s != NULL ? smart_str_extract(&secret) : zend_empty_string;
	}

	smart_str_free(&secret);
	return NULL;
}
#endif /* !PHP_WIN32 */

static zend_object *terminal_key_enum_from_string(zend_string *key)
{
	if (zend_string_equals_literal(key, "up")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "Up");
	}
	if (zend_string_equals_literal(key, "down")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "Down");
	}
	if (zend_string_equals_literal(key, "right")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "Right");
	}
	if (zend_string_equals_literal(key, "left")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "Left");
	}
	if (zend_string_equals_literal(key, "enter")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "Enter");
	}
	if (zend_string_equals_literal(key, "backspace")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "Backspace");
	}
	if (zend_string_equals_literal(key, "escape")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "Escape");
	}
	if (zend_string_equals_literal(key, "tab")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "Tab");
	}
	if (zend_string_equals_literal(key, "home")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "Home");
	}
	if (zend_string_equals_literal(key, "end")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "End");
	}
	if (zend_string_equals_literal(key, "delete")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "Delete");
	}
	if (zend_string_equals_literal(key, "pageup")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "PageUp");
	}
	if (zend_string_equals_literal(key, "pagedown")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "PageDown");
	}
	if (zend_string_equals_literal(key, "resize")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "Resize");
	}
	if (zend_string_equals_literal(key, "f1")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F1");
	}
	if (zend_string_equals_literal(key, "f2")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F2");
	}
	if (zend_string_equals_literal(key, "f3")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F3");
	}
	if (zend_string_equals_literal(key, "f4")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F4");
	}
	if (zend_string_equals_literal(key, "f5")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F5");
	}
	if (zend_string_equals_literal(key, "f6")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F6");
	}
	if (zend_string_equals_literal(key, "f7")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F7");
	}
	if (zend_string_equals_literal(key, "f8")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F8");
	}
	if (zend_string_equals_literal(key, "f9")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F9");
	}
	if (zend_string_equals_literal(key, "f10")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F10");
	}
	if (zend_string_equals_literal(key, "f11")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F11");
	}
	if (zend_string_equals_literal(key, "f12")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F12");
	}
	return NULL;
}

static bool terminal_size_from_environment(zend_long *columns, zend_long *rows)
{
	zend_string *lines = php_getenv("LINES", sizeof("LINES") - 1);
	zend_string *cols = php_getenv("COLUMNS", sizeof("COLUMNS") - 1);
	bool success = false;

	if (lines != NULL && cols != NULL) {
		zend_long r = ZEND_STRTOL(ZSTR_VAL(lines), NULL, 10);
		zend_long c = ZEND_STRTOL(ZSTR_VAL(cols), NULL, 10);
		if (r > 0 && c > 0) {
			*rows = r;
			*columns = c;
			success = true;
		}
	}

	if (lines != NULL) {
		zend_string_release(lines);
	}
	if (cols != NULL) {
		zend_string_release(cols);
	}

	return success;
}

static void terminal_create_terminal_size(zval *return_value, zend_long cols, zend_long rows)
{
	object_init_ex(return_value, terminal_terminal_size_ce);
	zend_update_property_long(terminal_terminal_size_ce, Z_OBJ_P(return_value), "cols", sizeof("cols") - 1, cols);
	zend_update_property_long(terminal_terminal_size_ce, Z_OBJ_P(return_value), "rows", sizeof("rows") - 1, rows);
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

	object_init_ex(return_value, terminal_terminal_ce);
}

PHP_METHOD(Io_Terminal_Terminal, fromStreams)
{
	zval *input_arg;
	zval *output_arg = NULL;
	terminal_object *intern;
	terminal_stream_target target;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_RESOURCE(input_arg)
		Z_PARAM_OPTIONAL
		Z_PARAM_RESOURCE_OR_NULL(output_arg)
	ZEND_PARSE_PARAMETERS_END();

	if (!terminal_stream_target_init(input_arg, true, &target) || target.php_stream == NULL) {
		zend_argument_value_error(1, "must be a valid stream resource");
		RETURN_THROWS();
	}

	if (output_arg != NULL && !Z_ISNULL_P(output_arg)) {
		terminal_stream_target out_target;
		if (!terminal_stream_target_init(output_arg, false, &out_target) || out_target.php_stream == NULL) {
			zend_argument_value_error(2, "must be a valid stream resource or null");
			RETURN_THROWS();
		}
	}

	object_init_ex(return_value, terminal_terminal_ce);
	intern = Z_TERMINAL_P(return_value);
	ZVAL_COPY(&intern->input_stream_val, input_arg);

	if (output_arg != NULL && !Z_ISNULL_P(output_arg)) {
		ZVAL_COPY(&intern->output_stream_val, output_arg);
	} else {
		ZVAL_COPY(&intern->output_stream_val, input_arg);
	}
}

PHP_METHOD(Io_Terminal_Terminal, getSize)
{
	terminal_object *intern;
	terminal_stream_target stream;
	zend_long columns = 0;
	zend_long rows = 0;

	ZEND_PARSE_PARAMETERS_NONE();

	intern = Z_TERMINAL_P(ZEND_THIS);

	if (!terminal_stream_target_init(&intern->output_stream_val, false, &stream)) {
		RETURN_THROWS();
	}

	if (!terminal_stream_size(stream.native_stream, &columns, &rows)
		&& !terminal_size_from_environment(&columns, &rows)) {
		RETURN_FALSE;
	}

	terminal_create_terminal_size(return_value, columns, rows);
}

PHP_METHOD(Io_Terminal_Terminal, enableRawMode)
{
	terminal_object *intern;
	terminal_stream_target stream;
	terminal_saved_mode saved;

	ZEND_PARSE_PARAMETERS_NONE();

	intern = Z_TERMINAL_P(ZEND_THIS);

	if (intern->active_mode_token != NULL) {
		terminal_mode_token_object *mode = terminal_mode_token_from_obj(intern->active_mode_token);
		if (mode->valid) {
			RETURN_OBJ_COPY(intern->active_mode_token);
		}
		OBJ_RELEASE(intern->active_mode_token);
		intern->active_mode_token = NULL;
	}

	if (!terminal_stream_target_init(&intern->input_stream_val, true, &stream)) {
		RETURN_THROWS();
	}

	if (!terminal_enable_stream_raw_mode(stream.native_stream, &saved)) {
		RETURN_FALSE;
	}

	terminal_create_mode_token(return_value, &saved, stream.stream_resource);

	if (Z_TYPE_P(return_value) == IS_OBJECT && instanceof_function(Z_OBJCE_P(return_value), terminal_mode_token_ce)) {
		intern->active_mode_token = Z_OBJ_P(return_value);
		GC_ADDREF(intern->active_mode_token);
	}
}

PHP_METHOD(Io_Terminal_Terminal, restoreMode)
{
	zval *mode_token = NULL;
	terminal_object *intern;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS_OR_NULL(mode_token, terminal_mode_token_ce)
	ZEND_PARSE_PARAMETERS_END();

	intern = Z_TERMINAL_P(ZEND_THIS);

	if (mode_token != NULL && !Z_ISNULL_P(mode_token)) {
		terminal_mode_token_object *mode = Z_TERMINAL_MODE_TOKEN_P(mode_token);
		bool restored;

		if (!mode->valid || memcmp(mode->saved.magic, TERMINAL_MODE_TOKEN_MAGIC, TERMINAL_MODE_TOKEN_MAGIC_LEN) != 0) {
			zend_argument_value_error(1, "must be an active terminal mode token returned by Io\\Terminal\\Terminal::enableRawMode()");
			RETURN_THROWS();
		}

		restored = terminal_release_mode_token(mode);
		if (restored && intern->active_mode_token != NULL && intern->active_mode_token == Z_OBJ_P(mode_token)) {
			OBJ_RELEASE(intern->active_mode_token);
			intern->active_mode_token = NULL;
		}

		RETURN_BOOL(restored);
	}

	if (intern->active_mode_token != NULL) {
		terminal_mode_token_object *mode = terminal_mode_token_from_obj(intern->active_mode_token);
		bool restored;

		if (!mode->valid || memcmp(mode->saved.magic, TERMINAL_MODE_TOKEN_MAGIC, TERMINAL_MODE_TOKEN_MAGIC_LEN) != 0) {
			OBJ_RELEASE(intern->active_mode_token);
			intern->active_mode_token = NULL;
			RETURN_FALSE;
		}

		restored = terminal_release_mode_token(mode);
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
	terminal_object *intern;
	terminal_stream_target stream;
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

	intern = Z_TERMINAL_P(ZEND_THIS);

	if (!terminal_stream_target_init(&intern->input_stream_val, true, &stream)) {
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

	key = terminal_read_stream_key(stream.native_stream, stream.php_stream, wait_ms, &intern->pending_high_surrogate);
#else
	int timeout_ms = -1;
	if (timeout_duration != NULL) {
		uint64_t sec = timeout_duration->duration.seconds;
		uint32_t nsec = timeout_duration->duration.nanoseconds;
		if (sec == 0 && nsec == 0) {
			timeout_ms = 0;
		} else if (sec >= (uint64_t) INT_MAX / 1000) {
			timeout_ms = INT_MAX;
		} else {
			uint64_t ms = sec * 1000 + (nsec + 999999) / 1000000;
			timeout_ms = ms >= (uint64_t) INT_MAX ? INT_MAX : (int) ms;
		}
	}

	int sequence_timeout_ms = TERMINAL_SEQUENCE_TIMEOUT_MS;
	if (seq_timeout_duration != NULL) {
		uint64_t sec = seq_timeout_duration->duration.seconds;
		uint32_t nsec = seq_timeout_duration->duration.nanoseconds;
		if (sec == 0 && nsec == 0) {
			sequence_timeout_ms = 0;
		} else if (sec >= (uint64_t) INT_MAX / 1000) {
			sequence_timeout_ms = INT_MAX;
		} else {
			uint64_t ms = sec * 1000 + (nsec + 999999) / 1000000;
			sequence_timeout_ms = ms >= (uint64_t) INT_MAX ? INT_MAX : (int) ms;
		}
	}

	key = terminal_read_stream_key(stream.native_stream, stream.php_stream, timeout_ms, sequence_timeout_ms, &intern->pending_utf8);
#endif

	if (key == NULL) {
		RETURN_FALSE;
	}

	key_case = terminal_key_enum_from_string(key);
	if (key_case != NULL) {
		zend_string_release(key);
		RETURN_OBJ_COPY(key_case);
	}

	RETURN_STR(key);
}

PHP_METHOD(Io_Terminal_Terminal, readSecret)
{
	terminal_object *intern;
	terminal_stream_target stream;
	zend_string *secret;

	ZEND_PARSE_PARAMETERS_NONE();

	intern = Z_TERMINAL_P(ZEND_THIS);

#ifdef PHP_WIN32
	intern->pending_high_surrogate = 0;
#else
	memset(&intern->pending_utf8, 0, sizeof(intern->pending_utf8));
#endif

	if (!terminal_stream_target_init(&intern->input_stream_val, true, &stream)) {
		RETURN_THROWS();
	}

	secret = terminal_read_stream_secret(stream.native_stream, stream.php_stream);
	if (secret != NULL) {
		RETURN_STR(secret);
	}

	zend_throw_exception(spl_ce_RuntimeException, "Unable to read secret from terminal", 0);
	RETURN_THROWS();
}

PHP_MINIT_FUNCTION(terminal)
{
	terminal_key_ce = register_class_Io_Terminal_Key();

	terminal_terminal_size_ce = register_class_Io_Terminal_TerminalSize();

	terminal_mode_token_ce = register_class_Io_Terminal_ModeToken();
	terminal_mode_token_ce->create_object = terminal_mode_token_create_object;
	terminal_mode_token_ce->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES | ZEND_ACC_NOT_SERIALIZABLE;

	memcpy(&terminal_mode_token_handlers, zend_get_std_object_handlers(), sizeof(terminal_mode_token_handlers));
	terminal_mode_token_handlers.offset = offsetof(terminal_mode_token_object, std);
	terminal_mode_token_handlers.free_obj = terminal_mode_token_free_obj;
	terminal_mode_token_handlers.clone_obj = NULL;

	terminal_terminal_ce = register_class_Io_Terminal_Terminal();
	terminal_terminal_ce->create_object = terminal_create_object;
	terminal_terminal_ce->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES | ZEND_ACC_NOT_SERIALIZABLE;

	memcpy(&terminal_object_handlers, zend_get_std_object_handlers(), sizeof(terminal_object_handlers));
	terminal_object_handlers.offset = offsetof(terminal_object, std);
	terminal_object_handlers.free_obj = terminal_free_obj;
	terminal_object_handlers.clone_obj = NULL;

#if !defined(PHP_WIN32) && defined(SIGWINCH) && defined(ZTS)
	terminal_resize_mutex = tsrm_mutex_alloc();
#endif

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(terminal)
{
#if !defined(PHP_WIN32) && defined(SIGWINCH) && defined(ZTS)
	if (terminal_resize_mutex != NULL) {
		tsrm_mutex_free(terminal_resize_mutex);
		terminal_resize_mutex = NULL;
	}
#endif

	return SUCCESS;
}

PHP_RINIT_FUNCTION(terminal)
{
	terminal_active_mode_tokens = NULL;
#ifdef PHP_WIN32
	memset(&terminal_pending_key, 0, sizeof(terminal_pending_key));
	terminal_pending_high_surrogate = 0;
#endif
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(terminal)
{
	while (terminal_active_mode_tokens != NULL) {
		terminal_mode_token_object *mode = terminal_active_mode_tokens;
		if (mode->valid && memcmp(mode->saved.magic, TERMINAL_MODE_TOKEN_MAGIC, TERMINAL_MODE_TOKEN_MAGIC_LEN) == 0) {
			terminal_release_mode_token(mode);
		} else {
			terminal_untrack_mode_token(mode);
		}
	}
	return SUCCESS;
}
