/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
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

#ifdef HAVE_CONFIG_H
# include "php_config.h"
# undef HAVE_CONFIG_H
#endif

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include "sapi/embed/php_embed.h"
#include "zend_static_cache_internal.h"

#ifdef ZEND_WIN32
# error "This helper requires the POSIX static cache lock implementation"
#endif

#define READER_HOLD_USEC 100000
#define WRITER_EARLY_CHECK_USEC 20000
#define WRITER_MIN_BLOCK_USEC 40000

static const char opcache_test_ini[] =
	"html_errors=0\n"
	"implicit_flush=1\n"
	"output_buffering=0\n"
	"max_execution_time=0\n"
	"max_input_time=-1\n"
	"opcache.enable=1\n"
	"opcache.enable_cli=1\n"
	"opcache.memory_consumption=64\n"
	"opcache.max_accelerated_files=200\n"
	"opcache.static_cache.volatile_size_mb=32\n\0";

static int zend_opcache_test_startup(int argc, char **argv)
{
#ifdef ZTS
	if (!php_tsrm_startup_ex(2)) {
		return FAILURE;
	}
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	zend_signal_startup();
	sapi_startup(&php_embed_module);
	/* Static Cache is opt-in per SAPI; this embed-based test enables it. */
	extern void zend_opcache_static_cache_opt_in(void);
	zend_opcache_static_cache_opt_in();
	php_embed_module.ini_entries = opcache_test_ini;
	if (argv != NULL) {
		php_embed_module.executable_location = argv[0];
	}

	if (php_embed_module.startup(&php_embed_module) == FAILURE) {
		sapi_shutdown();
#ifdef ZTS
		tsrm_shutdown();
#endif
		return FAILURE;
	}

	SG(options) |= SAPI_OPTION_NO_CHDIR;

	return SUCCESS;
}

static void zend_opcache_test_shutdown(void)
{
	php_module_shutdown();
	sapi_shutdown();
#ifdef ZTS
	tsrm_shutdown();
#endif
}

static void fail_with_message(const char *message)
{
	fprintf(stderr, "%s\n", message);
	exit(1);
}

static bool verify_lock_ready(void)
{
	if (!zend_opcache_static_cache_wlock()) {
		return false;
	}

	if (!zend_opcache_static_cache_header_init_locked()) {
		zend_opcache_static_cache_unlock();
		return false;
	}

	zend_opcache_static_cache_unlock();

	return true;
}

static bool read_exact(int fd, void *buffer, size_t length)
{
	unsigned char *cursor = (unsigned char *) buffer;
	ssize_t bytes_read;
	size_t total = 0;

	while (total < length) {
		bytes_read = read(fd, cursor + total, length - total);
		if (bytes_read <= 0) {
			return false;
		}

		total += (size_t) bytes_read;
	}

	return true;
}

static void reader_process(int write_fd)
{
	char signal = 'r';

	if (!zend_opcache_static_cache_rlock()) {
		_exit(10);
	}

	if (write(write_fd, &signal, sizeof(signal)) != (ssize_t) sizeof(signal)) {
		zend_opcache_static_cache_unlock();
		_exit(11);
	}

	usleep(READER_HOLD_USEC);
	zend_opcache_static_cache_unlock();
	_exit(0);
}

static void writer_process(int write_fd)
{
	struct timeval start_time, end_time;
	uint64_t elapsed_usec;

	if (gettimeofday(&start_time, NULL) != 0) {
		_exit(20);
	}

	if (!zend_opcache_static_cache_wlock()) {
		_exit(21);
	}

	if (gettimeofday(&end_time, NULL) != 0) {
		zend_opcache_static_cache_unlock();
		_exit(22);
	}

	elapsed_usec = (uint64_t) (end_time.tv_sec - start_time.tv_sec) * 1000000ULL;
	elapsed_usec += (uint64_t) (end_time.tv_usec - start_time.tv_usec);

	if (write(write_fd, &elapsed_usec, sizeof(elapsed_usec)) != (ssize_t) sizeof(elapsed_usec)) {
		zend_opcache_static_cache_unlock();
		_exit(23);
	}

	zend_opcache_static_cache_unlock();
	_exit(0);
}

static void require_child_success(pid_t pid, const char *label)
{
	int status;

	if (waitpid(pid, &status, 0) != pid) {
		fail_with_message("waitpid failed");
	}

	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
		fprintf(stderr, "%s exited with status %d\n", label, WIFEXITED(status) ? WEXITSTATUS(status) : -1);
		exit(1);
	}
}

int main(int argc, char **argv)
{
	int reader_pipe[2], writer_pipe[2], flags;
	char reader_signal[2];
	uint64_t writer_elapsed = 0;
	ssize_t bytes_read;
	pid_t reader_one, reader_two, writer;

	if (zend_opcache_test_startup(argc, argv) == FAILURE) {
		fail_with_message("startup failed");
	}

	if (!verify_lock_ready()) {
		zend_opcache_test_shutdown();
		fail_with_message("initial lock verification failed");
	}

	if (pipe(reader_pipe) != 0 || pipe(writer_pipe) != 0) {
		zend_opcache_test_shutdown();
		fail_with_message("pipe creation failed");
	}

	reader_one = fork();
	if (reader_one == 0) {
		close(reader_pipe[0]);
		close(writer_pipe[0]);
		close(writer_pipe[1]);
		reader_process(reader_pipe[1]);
	}
	if (reader_one < 0) {
		zend_opcache_test_shutdown();
		fail_with_message("first reader fork failed");
	}

	reader_two = fork();
	if (reader_two == 0) {
		close(reader_pipe[0]);
		close(writer_pipe[0]);
		close(writer_pipe[1]);
		reader_process(reader_pipe[1]);
	}
	if (reader_two < 0) {
		kill(reader_one, SIGTERM);
		require_child_success(reader_one, "reader_one");
		zend_opcache_test_shutdown();
		fail_with_message("second reader fork failed");
	}

	close(reader_pipe[1]);
	if (!read_exact(reader_pipe[0], reader_signal, sizeof(reader_signal))) {
		kill(reader_one, SIGTERM);
		kill(reader_two, SIGTERM);
		require_child_success(reader_one, "reader_one");
		require_child_success(reader_two, "reader_two");
		zend_opcache_test_shutdown();
		fail_with_message("readers did not acquire the lock");
	}

	writer = fork();
	if (writer == 0) {
		close(reader_pipe[0]);
		close(writer_pipe[0]);
		writer_process(writer_pipe[1]);
	}
	if (writer < 0) {
		kill(reader_one, SIGTERM);
		kill(reader_two, SIGTERM);
		require_child_success(reader_one, "reader_one");
		require_child_success(reader_two, "reader_two");
		zend_opcache_test_shutdown();
		fail_with_message("writer fork failed");
	}

	close(writer_pipe[1]);
	usleep(WRITER_EARLY_CHECK_USEC);
	flags = fcntl(writer_pipe[0], F_GETFL, 0);
	if (flags >= 0) {
		fcntl(writer_pipe[0], F_SETFL, flags | O_NONBLOCK);
	}
	bytes_read = read(writer_pipe[0], &writer_elapsed, sizeof(writer_elapsed));
	if (bytes_read > 0) {
		kill(reader_one, SIGTERM);
		kill(reader_two, SIGTERM);
		kill(writer, SIGTERM);
		require_child_success(reader_one, "reader_one");
		require_child_success(reader_two, "reader_two");
		require_child_success(writer, "writer");
		zend_opcache_test_shutdown();
		fail_with_message("writer acquired the lock before readers released it");
	}
	if (flags >= 0) {
		fcntl(writer_pipe[0], F_SETFL, flags);
	}

	require_child_success(reader_one, "reader_one");
	require_child_success(reader_two, "reader_two");
	if (!read_exact(writer_pipe[0], &writer_elapsed, sizeof(writer_elapsed))) {
		kill(writer, SIGTERM);
		require_child_success(writer, "writer");
		zend_opcache_test_shutdown();
		fail_with_message("writer did not report timing");
	}
	close(reader_pipe[0]);
	close(writer_pipe[0]);
	require_child_success(writer, "writer");

	if (writer_elapsed < WRITER_MIN_BLOCK_USEC) {
		zend_opcache_test_shutdown();
		fail_with_message("writer was not blocked by active readers");
	}

	printf("ok\n");
	zend_opcache_test_shutdown();

	return 0;
}
