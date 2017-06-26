--TEST--
mysqli_debug()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');

if (!function_exists('mysqli_debug'))
 	die("skip: mysqli_debug() not available");

if (!defined('MYSQLI_DEBUG_TRACE_ENABLED'))
	die("skip: can't say for sure if mysqli_debug works");

if (defined('MYSQLI_DEBUG_TRACE_ENABLED') && !MYSQLI_DEBUG_TRACE_ENABLED)
	die("skip: debug functionality not enabled");
?>
--FILE--
<?php
	require_once('connect.inc');

	if (NULL !== ($tmp = @mysqli_debug()))
		printf("[001] Expecting NULL/NULL, got %s/%s\n", gettype($tmp), $tmp);

	// NOTE: documentation is not clear on this: function always return NULL or TRUE
	if (true !== ($tmp = mysqli_debug(sprintf('d:t:O,%s/mysqli_debug_phpt.trace', sys_get_temp_dir()))))
		printf("[002] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	if ($IS_MYSQLND) {
		// let's make this mysqlnd only - for libmysql we need debug installation

		// table.inc will create a database connection and run some SQL queries, therefore
		// the debug file should have entries
		require_once('table.inc');

		clearstatcache();
		$trace_file = sprintf('%s/mysqli_debug_phpt.trace', sys_get_temp_dir());
		if (!file_exists($trace_file))
			printf("[003] Trace file '%s' has not been created\n", $trace_file);
		if (filesize($trace_file) < 50)
			printf("[004] Trace file '%s' is very small. filesize() reports only %d bytes. Please check.\n",
				$trace_file,
				filesize($trace_file));

		// will mysqli_debug() mind if the trace file gets removed?
		unlink($trace_file);
		clearstatcache();

		if (!$res = mysqli_query($link, 'SELECT * FROM test'))
				printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
			else
				mysqli_free_result($res);

		mysqli_close($link);

		clearstatcache();
		if (!file_exists($trace_file))
			printf("[006] Trace file '%s' does not exist\n", $trace_file);
		unlink($trace_file);
	}

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
done%s
