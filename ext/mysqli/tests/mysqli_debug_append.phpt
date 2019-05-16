--TEST--
mysqli_debug() - append to trace file
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

if (!$IS_MYSQLND)
	die("SKIP Libmysql feature not sufficiently spec'd in MySQL C API documentation");
?>
--FILE--
<?php
	require_once('connect.inc');

	if (true !== ($tmp = mysqli_debug(sprintf('d:t:O,%s/mysqli_debug_phpt.trace', sys_get_temp_dir()))))
		printf("[001] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	// table.inc will create a database connection and run some SQL queries, therefore
	// the debug file should have entries
	require_once('table.inc');

	clearstatcache();
	$trace_file = sprintf('%s/mysqli_debug_phpt.trace', sys_get_temp_dir());
	if (!file_exists($trace_file))
		printf("[002] Trace file '%s' has not been created\n", $trace_file);
	if (filesize($trace_file) < 50)
		printf("[003] Trace file '%s' is very small. filesize() reports only %d bytes. Please check.\n",
			$trace_file,
			filesize($trace_file));

	// will mysqli_debug() mind if the trace file gets removed?
	unlink($trace_file);
	clearstatcache();

	if (!$fp = fopen($trace_file, 'w')) {
		printf("[004] Cannot create trace file to test append mode\n");
	} else {

		if (!fwrite($fp, 'mysqli_debug.phpt test line'))
			printf("[005] Cannot write to trace file.\n");
		fclose($fp);

		if (true !== ($tmp = mysqli_debug(sprintf('d:a,%s', $trace_file))))
			printf("[006] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

		if (!$res = mysqli_query($link, 'SELECT * FROM test'))
			printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		else
			mysqli_free_result($res);

		$trace = file_get_contents($trace_file);
		if (!strstr($trace, 'mysqli_debug.phpt test line'))
			printf("[008] Cannot find original file content any more. Seems that the trace file got overwritten and not appended. Please check.");

		if (true !== ($tmp = mysqli_debug(sprintf('d:A,%s', $trace_file))))
			printf("[009] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

		if (!$res = mysqli_query($link, 'SELECT * FROM test'))
			printf("[010] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		else
			mysqli_free_result($res);

		if (!strstr(file_get_contents($trace_file), $trace))
			printf("[011] Cannot find original file content any more. Seems that the trace file got overwritten and not appended. Please check.");
	}

	// what will happen if we create new trace entries...?
	unlink($trace_file);
	clearstatcache();
	if (file_exists($trace_file))
		printf("[012] Could not remove trace file '%s'.\n", $trace_file);

	mysqli_close($link);
	print "done";
	if ($IS_MYSQLND)
		print "libmysql/DBUG package prints some debug info here."
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
done%s
