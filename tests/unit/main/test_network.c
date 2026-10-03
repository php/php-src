#include "php.h"
#include "php_network.h"
#include "sapi/embed/php_embed.h"
#include <cmocka.h>

/* The connect waits on the core's operation queue, which needs a started
 * engine; the poll(2) it ends in is mocked */

// Mocked poll
int __wrap_poll(struct pollfd *ufds, nfds_t nfds, int timeout)
{
	function_called();
	check_expected(timeout);

	int n = mock_type(int);
	if (n > 0) {
		ufds->revents = ufds->events;
	} else if (n < 0) {
		errno = -n;
		n = -1;
	}

	return n;
}

// Mocked connect
int __wrap_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	function_called();
	errno = mock_type(int);
	return errno != 0 ? -1 : 0;
}

// Mocked getsockopt
int __wrap_getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlen)
{
	function_called();
	int *error = (int *) optval;
	*error = mock_type(int);
	return mock_type(int);
}

// Test successful connection
static void test_php_network_connect_socket_immediate_success(void **state) {
	struct timeval timeout = { .tv_sec = 2, .tv_usec = 500000 };
	php_socket_t sockfd = 12;
	int error_code = 0;

	expect_function_call(__wrap_connect);
	will_return(__wrap_connect, 0);

	int result = php_network_connect_socket(sockfd, NULL, 0, 0, &timeout, NULL, &error_code);

	assert_int_equal(result, 0);
	assert_int_equal(error_code, 0);
}

// Test successful connection in progress followed by poll
static void test_php_network_connect_socket_progress_success(void **state) {
	struct timeval timeout_tv = { .tv_sec = 2, .tv_usec = 500000 };
	php_socket_t sockfd = 12;
	int error_code = 0;

	// Mock connect setting EINPROGRESS errno
	expect_function_call(__wrap_connect);
	will_return(__wrap_connect, EINPROGRESS);

	// Mock poll to return success
	expect_function_call(__wrap_poll);
	expect_in_range(__wrap_poll, timeout, 2400, 2500);
	will_return(__wrap_poll, 1);

	// Mock no socket error
	expect_function_call(__wrap_getsockopt);
	will_return(__wrap_getsockopt, 0); // optval saved result
	will_return(__wrap_getsockopt, 0); // actual return value

	int result = php_network_connect_socket(sockfd, NULL, 0, 0, &timeout_tv, NULL, &error_code);

	assert_int_equal(result, 0);
	assert_int_equal(error_code, 0);
}

// Test a poll interrupted by a signal restarting with the time left
static void test_php_network_connect_socket_eintr(void **state) {
	struct timeval timeout_tv = { .tv_sec = 2, .tv_usec = 500000 };
	php_socket_t sockfd = 12;
	int error_code = 0;

	// Mock connect to set EINPROGRESS
	expect_function_call(__wrap_connect);
	will_return(__wrap_connect, EINPROGRESS);

	// Mock poll to return EINTR first
	expect_function_call(__wrap_poll);
	expect_in_range(__wrap_poll, timeout, 2400, 2500);
	will_return(__wrap_poll, -EINTR);

	// Mock poll to succeed on retry
	expect_function_call(__wrap_poll);
	expect_in_range(__wrap_poll, timeout, 2400, 2500);
	will_return(__wrap_poll, 1);

	// Mock no socket error
	expect_function_call(__wrap_getsockopt);
	will_return(__wrap_getsockopt, 0);
	will_return(__wrap_getsockopt, 0);

	int result = php_network_connect_socket(sockfd, NULL, 0, 0, &timeout_tv, NULL, &error_code);

	// Ensure the function succeeds
	assert_int_equal(result, 0);
	assert_int_equal(error_code, 0);
}

// Test microseconds beyond a second counting towards the timeout
static void test_php_network_connect_socket_usec_overflow(void **state) {
	struct timeval timeout_tv = { .tv_sec = 2, .tv_usec = 1500000 };
	php_socket_t sockfd = 12;
	int error_code = 0;

	// Mock connect to set EINPROGRESS
	expect_function_call(__wrap_connect);
	will_return(__wrap_connect, EINPROGRESS);

	// Mock poll to succeed
	expect_function_call(__wrap_poll);
	expect_in_range(__wrap_poll, timeout, 3400, 3500);
	will_return(__wrap_poll, 1);

	// Mock no socket error
	expect_function_call(__wrap_getsockopt);
	will_return(__wrap_getsockopt, 0);  // optval saved result
	will_return(__wrap_getsockopt, 0);  // actual return value

	int result = php_network_connect_socket(sockfd, NULL, 0, 0, &timeout_tv, NULL, &error_code);

	// Ensure the function succeeds
	assert_int_equal(result, 0);
	assert_int_equal(error_code, 0);
}

// Test a connect in progress that fails
static void test_php_network_connect_socket_progress_error(void **state) {
	struct timeval timeout_tv = { .tv_sec = 2, .tv_usec = 500000 };
	php_socket_t sockfd = 12;
	int error_code = 0;

	// Mock connect to set EINPROGRESS
	expect_function_call(__wrap_connect);
	will_return(__wrap_connect, EINPROGRESS);

	// Mock poll to report the outcome
	expect_function_call(__wrap_poll);
	expect_in_range(__wrap_poll, timeout, 2400, 2500);
	will_return(__wrap_poll, 1);

	// Mock the socket error
	expect_function_call(__wrap_getsockopt);
	will_return(__wrap_getsockopt, ECONNREFUSED);  // optval saved result
	will_return(__wrap_getsockopt, 0);  // actual return value

	int result = php_network_connect_socket(sockfd, NULL, 0, 0, &timeout_tv, NULL, &error_code);

	assert_int_equal(result, -1);
	assert_int_equal(error_code, ECONNREFUSED);
}

// Test connection error (ECONNREFUSED)
static void test_php_network_connect_socket_connect_error(void **state) {
	struct timeval timeout = { .tv_sec = 2, .tv_usec = 500000 };
	php_socket_t sockfd = 12;
	int error_code = 0;

	// Mock connect to set ECONNREFUSED
	expect_function_call(__wrap_connect);
	will_return(__wrap_connect, ECONNREFUSED);

	int result = php_network_connect_socket(sockfd, NULL, 0, 0, &timeout, NULL, &error_code);

	// Ensure the function returns an error
	assert_int_equal(result, -1);
	assert_int_equal(error_code, ECONNREFUSED);
}

static int setup(void **state) {
	return php_embed_init(0, NULL) == SUCCESS ? 0 : -1;
}

static int teardown(void **state) {
	php_embed_shutdown();
	return 0;
}

int main(void) {
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_php_network_connect_socket_immediate_success),
		cmocka_unit_test(test_php_network_connect_socket_progress_success),
		cmocka_unit_test(test_php_network_connect_socket_eintr),
		cmocka_unit_test(test_php_network_connect_socket_usec_overflow),
		cmocka_unit_test(test_php_network_connect_socket_progress_error),
		cmocka_unit_test(test_php_network_connect_socket_connect_error),
	};
	return cmocka_run_group_tests(tests, setup, teardown);
}
