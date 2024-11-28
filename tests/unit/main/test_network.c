#include "php.h"
#include "php_network.h"
#include <cmocka.h>

// Mocked poll
int __wrap_poll(struct pollfd *ufds, nfds_t nfds, int timeout)
{
	function_called();
	check_expected(timeout);

	int n = mock_type(int);
	if (n > 0) {
		ufds->revents = 1;
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

// Mocked gettimeofday
int __wrap_gettimeofday(struct timeval *time_Info, struct timezone *timezone_Info)
{
	function_called();
	struct timeval *now = mock_ptr_type(struct timeval *);
	if (now) {
		time_Info->tv_sec = now->tv_sec;
		time_Info->tv_usec = now->tv_usec;
	}
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

	// Mock time setting - ignored
	expect_function_call(__wrap_gettimeofday);
	will_return(__wrap_gettimeofday, NULL);
	will_return(__wrap_gettimeofday, 0);

	// Mock poll to return success
	expect_function_call(__wrap_poll);
	expect_value(__wrap_poll, timeout, 2500);
	will_return(__wrap_poll, 1);

	// Mock no socket error
	expect_function_call(__wrap_getsockopt);
	will_return(__wrap_getsockopt, 0); // optval saved result
	will_return(__wrap_getsockopt, 0); // actual return value

	int result = php_network_connect_socket(sockfd, NULL, 0, 0, &timeout_tv, NULL, &error_code);

	assert_int_equal(result, 0);
	assert_int_equal(error_code, 0);
}

static void test_php_network_connect_socket_eintr_t1(void **state) {
	struct timeval timeout_tv = { .tv_sec = 2, .tv_usec = 500000 };
	struct timeval start_time = { .tv_sec = 1000, .tv_usec = 0 };  // Initial time
	struct timeval retry_time = { .tv_sec = 1001, .tv_usec = 200000 };  // Time after EINTR
	php_socket_t sockfd = 12;
	int error_code = 0;

	// Mock connect to set EINPROGRESS
	expect_function_call(__wrap_connect);
	will_return(__wrap_connect, EINPROGRESS);

	// Mock gettimeofday for initial call
	expect_function_call(__wrap_gettimeofday);
	will_return(__wrap_gettimeofday, &start_time);
	will_return(__wrap_gettimeofday, 0);

	// Mock poll to return EINTR first
	expect_function_call(__wrap_poll);
	expect_value(__wrap_poll, timeout, 2500);
	will_return(__wrap_poll, -EINTR);

	// Mock gettimeofday after EINTR
	expect_function_call(__wrap_gettimeofday);
	will_return(__wrap_gettimeofday, &retry_time);
	will_return(__wrap_gettimeofday, 0);

	// Mock poll to succeed on retry
	expect_function_call(__wrap_poll);
	expect_value(__wrap_poll, timeout, 1300);
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

static void test_php_network_connect_socket_eintr_t2(void **state) {
	struct timeval timeout_tv = { .tv_sec = 2, .tv_usec = 1500000 };
	struct timeval start_time = { .tv_sec = 1000, .tv_usec = 300000 };  // Initial time
	struct timeval retry_time = { .tv_sec = 1001, .tv_usec = 200000 };  // Time after EINTR
	php_socket_t sockfd = 12;
	int error_code = 0;

	// Mock connect to set EINPROGRESS
	expect_function_call(__wrap_connect);
	will_return(__wrap_connect, EINPROGRESS);

	// Mock gettimeofday for initial call
	expect_function_call(__wrap_gettimeofday);
	will_return(__wrap_gettimeofday, &start_time);
	will_return(__wrap_gettimeofday, 0);

	// Mock poll to return EINTR first
	expect_function_call(__wrap_poll);
	expect_value(__wrap_poll, timeout, 3500);
	will_return(__wrap_poll, -EINTR);

	// Mock gettimeofday after EINTR
	expect_function_call(__wrap_gettimeofday);
	will_return(__wrap_gettimeofday, &retry_time);
	will_return(__wrap_gettimeofday, 0);

	// Mock poll to succeed on retry
	expect_function_call(__wrap_poll);
	expect_value(__wrap_poll, timeout, 2600);
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

static void test_php_network_connect_socket_eintr_t3(void **state) {
	struct timeval timeout_tv = { .tv_sec = 2, .tv_usec = 500000 };
	struct timeval start_time = { .tv_sec = 1002, .tv_usec = 300000 };  // Initial time
	struct timeval retry_time = { .tv_sec = 1001, .tv_usec = 2200000 };  // Time after EINTR
	php_socket_t sockfd = 12;
	int error_code = 0;

	// Mock connect to set EINPROGRESS
	expect_function_call(__wrap_connect);
	will_return(__wrap_connect, EINPROGRESS);

	// Mock gettimeofday for initial call
	expect_function_call(__wrap_gettimeofday);
	will_return(__wrap_gettimeofday, &start_time);
	will_return(__wrap_gettimeofday, 0);

	// Mock poll to return EINTR first
	expect_function_call(__wrap_poll);
	expect_value(__wrap_poll, timeout, 2500);
	will_return(__wrap_poll, -EINTR);

	// Mock gettimeofday after EINTR
	expect_function_call(__wrap_gettimeofday);
	will_return(__wrap_gettimeofday, &retry_time);
	will_return(__wrap_gettimeofday, 0);

	// Mock poll to succeed on retry
	expect_function_call(__wrap_poll);
	expect_value(__wrap_poll, timeout, 1600);
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


int main(void) {
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_php_network_connect_socket_immediate_success),
		cmocka_unit_test(test_php_network_connect_socket_progress_success),
		cmocka_unit_test(test_php_network_connect_socket_eintr_t1),
		cmocka_unit_test(test_php_network_connect_socket_eintr_t2),
		cmocka_unit_test(test_php_network_connect_socket_eintr_t3),
		cmocka_unit_test(test_php_network_connect_socket_connect_error),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}