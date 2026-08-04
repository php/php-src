#include "php.h"
#include "io/php_io_internal.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <cmocka.h>

/* Mocked syscalls return the value queued via will_return(); a negative value
 * -E is translated to a -1 return with errno set to E. Must expand inside the
 * wrapper body because cmocka keys the queued values by __func__. */
#define MOCK_IO_RESULT(result_var) \
	ssize_t result_var = mock_type(ssize_t); \
	if (result_var < 0) { \
		errno = (int) -result_var; \
		result_var = -1; \
	}

ssize_t __wrap_copy_file_range(int fd_in, off_t *off_in, int fd_out, off_t *off_out, size_t len, unsigned int flags)
{
	function_called();
	MOCK_IO_RESULT(result);
	return result;
}

ssize_t __wrap_sendfile(int out_fd, int in_fd, off_t *offset, size_t count)
{
	function_called();
	MOCK_IO_RESULT(result);
	return result;
}

ssize_t __wrap_splice(int fd_in, off_t *off_in, int fd_out, off_t *off_out, size_t len, unsigned int flags)
{
	function_called();
	check_expected(len);
	check_expected(flags);
	MOCK_IO_RESULT(result);
	return result;
}

ssize_t __wrap_read(int fd, void *buf, size_t count)
{
	function_called();
	MOCK_IO_RESULT(result);
	if (result > 0) {
		memset(buf, 'x', result);
	}
	return result;
}

ssize_t __wrap_write(int fd, const void *buf, size_t count)
{
	function_called();
	MOCK_IO_RESULT(result);
	return result;
}

int __wrap_poll(struct pollfd *ufds, nfds_t nfds, int timeout)
{
	function_called();
	check_expected(timeout);

	int n = mock_type(int);
	if (n > 0) {
		ufds->revents = POLLIN;
	} else if (n < 0) {
		errno = -n;
		n = -1;
	}

	return n;
}

int __wrap_setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
	function_called();
	check_expected(optname);
	return 0;
}

/* Kernel MAX_RW_COUNT, must match PHP_IO_SPLICE_MAX in php_io_copy_linux.c */
#define TEST_SPLICE_MAX ((size_t) 0x7ffff000)

static php_io_fd make_io_fd(int fd, php_io_fd_type fd_type)
{
	php_io_fd io_fd = {
		.fd = fd,
		.fd_type = fd_type,
		.timeout = { .tv_sec = -1, .tv_usec = 0 },
		.is_blocked = 0,
	};
	return io_fd;
}

/* file -> file: a hard error after partial progress must report FAILURE
 * together with the bytes already copied */
static void test_file_to_file_error_after_partial(void **state)
{
	php_io_fd src = make_io_fd(10, PHP_IO_FD_FILE);
	php_io_fd dest = make_io_fd(11, PHP_IO_FD_FILE);
	size_t copied = 0;

	expect_function_calls(__wrap_copy_file_range, 2);
	will_return(__wrap_copy_file_range, 4096);
	will_return(__wrap_copy_file_range, -ENOSPC);

	assert_int_equal(php_io_linux_copy(&src, &dest, PHP_IO_COPY_ALL, &copied), FAILURE);
	assert_int_equal(copied, 4096);
}

/* file -> file: EXDEV with no progress falls back to the read/write loop */
static void test_file_to_file_exdev_fallback(void **state)
{
	php_io_fd src = make_io_fd(10, PHP_IO_FD_FILE);
	php_io_fd dest = make_io_fd(11, PHP_IO_FD_FILE);
	size_t copied = 0;

	expect_function_call(__wrap_copy_file_range);
	will_return(__wrap_copy_file_range, -EXDEV);

	expect_function_call(__wrap_read);
	will_return(__wrap_read, 100);
	expect_function_call(__wrap_write);
	will_return(__wrap_write, 100);
	expect_function_call(__wrap_read);
	will_return(__wrap_read, 0);

	assert_int_equal(php_io_linux_copy(&src, &dest, PHP_IO_COPY_ALL, &copied), SUCCESS);
	assert_int_equal(copied, 100);
}

/* the generic fallback loop must retry interrupted read() and write() */
static void test_generic_fallback_eintr_retry(void **state)
{
	size_t copied = 0;

	expect_function_calls(__wrap_read, 2);
	will_return(__wrap_read, -EINTR);
	will_return(__wrap_read, 50);
	expect_function_calls(__wrap_write, 2);
	will_return(__wrap_write, -EINTR);
	will_return(__wrap_write, 50);
	expect_function_call(__wrap_read);
	will_return(__wrap_read, 0);

	assert_int_equal(php_io_generic_copy_fallback(10, 11, PHP_IO_COPY_ALL, &copied), SUCCESS);
	assert_int_equal(copied, 50);
}

/* the generic fallback loop must report FAILURE with the partial count when a
 * write fails mid-copy */
static void test_generic_fallback_write_error_after_partial(void **state)
{
	size_t copied = 0;

	expect_function_call(__wrap_read);
	will_return(__wrap_read, 50);
	expect_function_call(__wrap_write);
	will_return(__wrap_write, 50);
	expect_function_call(__wrap_read);
	will_return(__wrap_read, 50);
	expect_function_call(__wrap_write);
	will_return(__wrap_write, -ENOSPC);

	assert_int_equal(php_io_generic_copy_fallback(10, 11, PHP_IO_COPY_ALL, &copied), FAILURE);
	assert_int_equal(copied, 50);
}

/* file -> socket: a hard sendfile error after partial progress must report
 * FAILURE together with the bytes already sent */
static void test_file_to_socket_sendfile_error_after_partial(void **state)
{
	php_io_fd src = make_io_fd(10, PHP_IO_FD_FILE);
	php_io_fd dest = make_io_fd(11, PHP_IO_FD_SOCKET);
	size_t copied = 0;

	expect_function_calls(__wrap_sendfile, 2);
	will_return(__wrap_sendfile, 8192);
	will_return(__wrap_sendfile, -EPIPE);

	assert_int_equal(php_io_linux_copy(&src, &dest, PHP_IO_COPY_ALL, &copied), FAILURE);
	assert_int_equal(copied, 8192);
}

/* file -> socket: sendfile EINVAL with no progress falls back to the
 * read/write loop */
static void test_file_to_socket_sendfile_einval_fallback(void **state)
{
	php_io_fd src = make_io_fd(10, PHP_IO_FD_FILE);
	php_io_fd dest = make_io_fd(11, PHP_IO_FD_SOCKET);
	size_t copied = 0;

	expect_function_call(__wrap_sendfile);
	will_return(__wrap_sendfile, -EINVAL);

	expect_function_call(__wrap_read);
	will_return(__wrap_read, 10);
	expect_function_call(__wrap_write);
	will_return(__wrap_write, 10);
	expect_function_call(__wrap_read);
	will_return(__wrap_read, 0);

	assert_int_equal(php_io_linux_copy(&src, &dest, PHP_IO_COPY_ALL, &copied), SUCCESS);
	assert_int_equal(copied, 10);
}

/* socket -> file: a blocking source socket must be polled with the stream
 * timeout and a timeout is a clean stop, not an error */
static void test_socket_source_poll_timeout(void **state)
{
	php_io_fd src = make_io_fd(10, PHP_IO_FD_SOCKET);
	php_io_fd dest = make_io_fd(11, PHP_IO_FD_FILE);
	size_t copied = 42;

	src.is_blocked = 1;
	src.timeout.tv_sec = 2;
	src.timeout.tv_usec = 500000;

	expect_function_call(__wrap_poll);
	expect_value(__wrap_poll, timeout, 2500);
	will_return(__wrap_poll, 0);

	assert_int_equal(php_io_linux_copy(&src, &dest, PHP_IO_COPY_ALL, &copied), SUCCESS);
	assert_int_equal(copied, 0);
}

/* pipe -> socket: splices are corked with SPLICE_F_MORE and capped at the
 * kernel MAX_RW_COUNT; an error after partial progress must report FAILURE
 * with the partial count and still clear the cork */
static void test_pipe_to_socket_splice_error_after_partial_uncorks(void **state)
{
	php_io_fd src = make_io_fd(10, PHP_IO_FD_PIPE);
	php_io_fd dest = make_io_fd(11, PHP_IO_FD_SOCKET);
	size_t copied = 0;

	expect_function_calls(__wrap_splice, 2);
	expect_value_count(__wrap_splice, len, TEST_SPLICE_MAX, 2);
	expect_value_count(__wrap_splice, flags, SPLICE_F_MORE, 2);
	will_return(__wrap_splice, 1000);
	will_return(__wrap_splice, -EPIPE);

	expect_function_call(__wrap_setsockopt);
	expect_value(__wrap_setsockopt, optname, TCP_CORK);

	assert_int_equal(php_io_linux_copy(&src, &dest, PHP_IO_COPY_ALL, &copied), FAILURE);
	assert_int_equal(copied, 1000);
}

/* socket -> file: when the outbound splice fails, the data already sitting in
 * the intermediate pipe is salvaged with a read/write loop (retrying EINTR)
 * and the copy reports FAILURE with the salvaged count */
static void test_socket_to_file_splice_out_error_drains_pipe(void **state)
{
	php_io_fd src = make_io_fd(10, PHP_IO_FD_SOCKET);
	php_io_fd dest = make_io_fd(11, PHP_IO_FD_FILE);
	size_t copied = 0;

	expect_function_calls(__wrap_splice, 2);
	/* socket -> pipe */
	expect_value(__wrap_splice, len, SSIZE_MAX);
	expect_value(__wrap_splice, flags, 0);
	will_return(__wrap_splice, 500);
	/* pipe -> file */
	expect_value(__wrap_splice, len, 500);
	expect_value(__wrap_splice, flags, 0);
	will_return(__wrap_splice, -EINVAL);

	expect_function_calls(__wrap_read, 2);
	will_return(__wrap_read, -EINTR);
	will_return(__wrap_read, 500);
	expect_function_call(__wrap_write);
	will_return(__wrap_write, 500);

	assert_int_equal(php_io_linux_copy(&src, &dest, PHP_IO_COPY_ALL, &copied), FAILURE);
	assert_int_equal(copied, 500);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_file_to_file_error_after_partial),
		cmocka_unit_test(test_file_to_file_exdev_fallback),
		cmocka_unit_test(test_generic_fallback_eintr_retry),
		cmocka_unit_test(test_generic_fallback_write_error_after_partial),
		cmocka_unit_test(test_file_to_socket_sendfile_error_after_partial),
		cmocka_unit_test(test_file_to_socket_sendfile_einval_fallback),
		cmocka_unit_test(test_socket_source_poll_timeout),
		cmocka_unit_test(test_pipe_to_socket_splice_error_after_partial_uncorks),
		cmocka_unit_test(test_socket_to_file_splice_out_error_drains_pipe),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}
