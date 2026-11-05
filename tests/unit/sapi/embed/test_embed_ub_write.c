#include "php.h"
#include "sapi/embed/php_embed.h"
#include <unistd.h>
#include <cmocka.h>

#ifdef PHP_WRITE_STDOUT
ssize_t __wrap_write(int fd, const void *buf, size_t count)
{
	function_called();
	check_expected(fd);
	check_expected(count);
	check_expected(buf);
	return mock_type(ssize_t);
}

struct ub_write_state {
	bool ignore_user_abort;
	short connection_status;
	int output_status;
};

static int setup_ub_write(void **state)
{
	struct ub_write_state *saved = malloc(sizeof(*saved));

	assert_non_null(saved);
	saved->ignore_user_abort = PG(ignore_user_abort);
	saved->connection_status = PG(connection_status);
	saved->output_status = php_output_get_status();

	/* Keep php_handle_aborted_connection() from bailing out on failure. */
	PG(ignore_user_abort) = true;
	PG(connection_status) = PHP_CONNECTION_NORMAL;

	*state = saved;
	return 0;
}

static int teardown_ub_write(void **state)
{
	struct ub_write_state *saved = *state;

	PG(ignore_user_abort) = saved->ignore_user_abort;
	PG(connection_status) = saved->connection_status;
	php_output_set_status(saved->output_status);
	free(saved);
	return 0;
}

/* A failed write without progress must not retry forever. */
static void test_ub_write_error_without_progress(void **state)
{
	expect_function_call(__wrap_write);
	expect_value(__wrap_write, fd, STDOUT_FILENO);
	expect_value(__wrap_write, count, 16);
	expect_memory(__wrap_write, buf, "0123456789abcdef", 16);
	will_return(__wrap_write, 0);

	assert_int_equal(php_embed_module.ub_write("0123456789abcdef", 16), 0);
	assert_int_equal(PG(connection_status), PHP_CONNECTION_ABORTED);
}

/* After a partial write, a failure must return the number of bytes written. */
static void test_ub_write_partial_then_error(void **state)
{
	expect_function_calls(__wrap_write, 2);
	expect_value_count(__wrap_write, fd, STDOUT_FILENO, 2);
	expect_value(__wrap_write, count, 10);
	expect_memory(__wrap_write, buf, "0123456789", 10);
	expect_value(__wrap_write, count, 6);
	expect_memory(__wrap_write, buf, "456789", 6);
	will_return(__wrap_write, 4);
	will_return(__wrap_write, 0);

	assert_int_equal(php_embed_module.ub_write("0123456789", 10), 4);
	assert_int_equal(PG(connection_status), PHP_CONNECTION_ABORTED);
}

/* Partial writes must continue until the whole buffer is written. */
static void test_ub_write_partial_writes(void **state)
{
	expect_function_calls(__wrap_write, 2);
	expect_value_count(__wrap_write, fd, STDOUT_FILENO, 2);
	expect_value(__wrap_write, count, 10);
	expect_memory(__wrap_write, buf, "0123456789", 10);
	expect_value(__wrap_write, count, 6);
	expect_memory(__wrap_write, buf, "456789", 6);
	will_return(__wrap_write, 4);
	will_return(__wrap_write, 6);

	assert_int_equal(php_embed_module.ub_write("0123456789", 10), 10);
	assert_int_equal(PG(connection_status), PHP_CONNECTION_NORMAL);
}
#endif

int main(void)
{
#ifdef PHP_WRITE_STDOUT
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup_teardown(test_ub_write_error_without_progress, setup_ub_write, teardown_ub_write),
		cmocka_unit_test_setup_teardown(test_ub_write_partial_then_error, setup_ub_write, teardown_ub_write),
		cmocka_unit_test_setup_teardown(test_ub_write_partial_writes, setup_ub_write, teardown_ub_write),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
#else
	return 0;
#endif
}
