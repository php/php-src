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
   | license@php.net so we can mail you a copy immediately.                |
   +----------------------------------------------------------------------+
   | Authors: Edmond <EdmondDantes@users.noreply.github.com>              |
   |          Jakub Zelenka <bukka@php.net>                               |
   +----------------------------------------------------------------------+
*/

/* Windows: the overlapped named pipes proc_open() makes for a provider (PHP_IO_OP_F_PIPE). An
 * anonymous pipe takes no overlapped I/O, so a provider could perform nothing on it and a read of
 * a child's output blocked the thread; a named pipe made as libuv makes it has an overlapped
 * parent end, which a completion port completes, and a synchronous child end. */

#include "php.h"
#include "php_io_hooks.h"
#include "ext/standard/file.h"
#include "php_io_internal.h"
#include "main/streams/php_stream_plain_wrapper.h"
#include <io.h>
#include <fcntl.h>

#ifdef PHP_WIN32

#include "ext/random/php_random_csprng.h"

/* PHP_IO_OVERLAPPED_PIPES=1 in the environment makes every proc_open() pipe overlapped, hooks or
 * not: the test suite run that decides whether that becomes the default (as for files, see
 * plain_wrapper.c) */
static bool php_io_pipe_forced(void)
{
	static int forced = -1;
	if (forced < 0) {
		const char *env = getenv("PHP_IO_OVERLAPPED_PIPES");
		forced = env != NULL && *env != '\0' && *env != '0';
	}
	return forced == 1;
}

/* The rule files follow: overlapped when a provider is installed at the time the pipe is made,
 * since a pipe made before set_hooks() is as good as one a script without hooks reads, and plain
 * otherwise. Any provider: a Read or Write op on a pipe is handed over whatever its data flags, and
 * one that answers Unsupported leaves the wait to the thread, which is what a plain pipe costs. */
PHPAPI bool php_io_pipe_wanted(void)
{
	return php_io_pipe_forced() || FG(io_hooks) != NULL;
}

/* A child's pipe as libuv's uv_spawn() makes it: the parent's end is an overlapped named pipe,
 * which an I/O completion port can complete, and the child's end is synchronous and inheritable.
 * One instance and a name that cannot be guessed, so no other client can take the child's place.
 * pair is filled as pipe() fills it: the read end, then the write end. */
static zend_result php_io_pipe_create_overlapped(HANDLE pair[2], bool parent_reads)
{
	const DWORD parent_access = (parent_reads ? PIPE_ACCESS_INBOUND : PIPE_ACCESS_OUTBOUND)
			| FILE_FLAG_OVERLAPPED | FILE_FLAG_FIRST_PIPE_INSTANCE | WRITE_DAC;
	const DWORD child_access = (parent_reads ? GENERIC_WRITE | FILE_READ_ATTRIBUTES : GENERIC_READ | FILE_WRITE_ATTRIBUTES)
			| WRITE_DAC;
	wchar_t name[64];
	HANDLE server = INVALID_HANDLE_VALUE;
	DWORD err = ERROR_SUCCESS;

	for (int attempt = 0; attempt < 16; attempt++) {
		uint64_t name_bits;
		if (php_random_bytes_silent(&name_bits, sizeof(name_bits)) == FAILURE) {
			errno = EIO;
			return FAILURE;
		}

		_snwprintf_s(name, sizeof(name) / sizeof(name[0]), _TRUNCATE, L"\\\\.\\pipe\\php-%lu-%016llx",
				GetCurrentProcessId(), (unsigned long long) name_bits);
		server = CreateNamedPipeW(name, parent_access,
				PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS, 1,
				PHP_IO_PIPE_BUFFER_SIZE, PHP_IO_PIPE_BUFFER_SIZE, 0, NULL);
		if (server != INVALID_HANDLE_VALUE) {
			break;
		}

		err = GetLastError();
		/* A taken name: ERROR_ACCESS_DENIED under FILE_FLAG_FIRST_PIPE_INSTANCE, else ERROR_PIPE_BUSY */
		if (err != ERROR_ACCESS_DENIED && err != ERROR_PIPE_BUSY) {
			break;
		}
	}

	if (server == INVALID_HANDLE_VALUE) {
		SET_ERRNO_FROM_WIN32_CODE(err);
		return FAILURE;
	}

	SECURITY_ATTRIBUTES inheritable = { .nLength = sizeof(SECURITY_ATTRIBUTES), .bInheritHandle = TRUE };
	const HANDLE client = CreateFileW(name, child_access, 0, &inheritable, OPEN_EXISTING, 0, NULL);
	if (client == INVALID_HANDLE_VALUE) {
		SET_ERRNO_FROM_WIN32_CODE(GetLastError());
		CloseHandle(server);
		return FAILURE;
	}

	/* Both ends exist, so the connect completes at once; the wait keeps a late completion off the
	 * stack */
	OVERLAPPED ov;
	memset(&ov, 0, sizeof(ov));
	BOOL connected = ConnectNamedPipe(server, &ov);
	if (!connected) {
		const DWORD connect_err = GetLastError();
		DWORD unused;
		connected = connect_err == ERROR_PIPE_CONNECTED
				|| (connect_err == ERROR_IO_PENDING && GetOverlappedResult(server, &ov, &unused, TRUE));
	}

	if (!connected) {
		SET_ERRNO_FROM_WIN32_CODE(GetLastError());
		CloseHandle(client);
		CloseHandle(server);
		return FAILURE;
	}

	pair[0] = parent_reads ? server : client;
	pair[1] = parent_reads ? client : server;
	return SUCCESS;
}

/* Bytes the pipe holds now; *eof at the other end's close */
PHPAPI bool php_io_pipe_peek(int fd, DWORD *avail, bool *eof)
{
	*avail = 0;
	*eof = false;
	const HANDLE h = (HANDLE) _get_osfhandle(fd);
	if (h == INVALID_HANDLE_VALUE) {
		_set_errno(EBADF);
		return false;
	}

	if (PeekNamedPipe(h, NULL, 0, NULL, avail, NULL)) {
		return true;
	}

	const DWORD err = GetLastError();
	if (err == ERROR_BROKEN_PIPE) {
		*eof = true;
		return true;
	}

	/* ERROR_ACCESS_DENIED: fd is the write end */
	_set_errno(err == ERROR_ACCESS_DENIED ? EBADF : EIO);
	return false;
}

PHPAPI bool php_io_pipe_readable(int fd)
{
	DWORD avail;
	bool eof;
	return !php_io_pipe_peek(fd, &avail, &eof) || eof || avail > 0;
}

/* The platform's pipe (php_io_pipe.c) */

zend_result php_io_windows_pipe_create(HANDLE pair[2], bool parent_reads, bool *overlapped)
{
	*overlapped = php_io_pipe_wanted();
	if (*overlapped) {
		return php_io_pipe_create_overlapped(pair, parent_reads);
	}
	/* An anonymous pipe, both ends inheritable; proc_open() keeps its own end from the child */
	SECURITY_ATTRIBUTES inheritable = { .nLength = sizeof(SECURITY_ATTRIBUTES), .bInheritHandle = TRUE };
	if (!CreatePipe(&pair[0], &pair[1], &inheritable, 0)) {
		SET_ERRNO_FROM_WIN32_CODE(GetLastError());
		return FAILURE;
	}
	return SUCCESS;
}

php_stream *php_io_windows_pipe_stream(HANDLE end, int mode_flags, const char *mode, bool overlapped)
{
	if (overlapped) {
		/* Its reads and writes go around the CRT, so no text mode */
		mode_flags |= O_BINARY;
	}
	const int fd = _open_osfhandle((intptr_t) end, mode_flags);
	if (fd < 0) {
		return NULL;
	}
	return overlapped ? php_stream_fopen_from_overlapped_pipe(fd, mode) : php_stream_fopen_from_fd(fd, mode, NULL);
}

zend_result php_io_windows_pipe_stream_release(php_stream *stream)
{
	/* NOTIMPL for anything but an overlapped pipe: nothing ties another stream's descriptor */
	return php_stream_set_option(stream, PHP_STREAM_OPTION_OVERLAPPED_PIPE, PHP_STREAM_OVERLAPPED_PIPE_RELEASE, NULL)
			== PHP_STREAM_OPTION_RETURN_ERR ? FAILURE : SUCCESS;
}

void php_io_windows_pipe_stream_handed_out(php_stream *stream)
{
	php_stream_set_option(stream, PHP_STREAM_OPTION_OVERLAPPED_PIPE, PHP_STREAM_OVERLAPPED_PIPE_HAND_OUT, NULL);
}

#endif /* PHP_WIN32 */
