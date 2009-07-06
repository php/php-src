--TEST--
mysqli_debug() - mysqlnd only control strings
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
require_once('connect.inc');

if (!function_exists('mysqli_debug'))
 	die("skip mysqli_debug() not available");

if (!defined('MYSQLI_DEBUG_TRACE_ENABLED'))
	die("skip: can't say for sure if mysqli_debug works");

if (defined('MYSQLI_DEBUG_TRACE_ENABLED') && !MYSQLI_DEBUG_TRACE_ENABLED)
	die("skip: debug functionality not enabled");

if (!$IS_MYSQLND)
	die("skip mysqlnd only test");
?>
--FILE--
<?php
	require_once('connect.inc');;
	require_once('table.inc');

	function try_control_string($link, $control_string, $trace_file, $offset) {

		@unlink($trace_file);
		if (true !== ($tmp = @mysqli_debug($control_string))) {
			printf("[%03d][control string '%s'] Expecting boolean/true, got %s/%s.\n",
				$offset + 1,
				$control_string,
				gettype($tmp),
				$tmp);
			return false;
		}

		if (!$res = mysqli_query($link, 'SELECT * FROM test')) {
			printf("[%03d][control string '%s'] [%d] %s.\n",
				$offset + 2,
				$control_string,
				mysqli_errno($link),
				mysqli_error($link));
			return false;
		}
		while ($row = mysqli_fetch_assoc($res))
			;
		mysqli_free_result($res);

		clearstatcache();
		if (!file_exists($trace_file)) {
			printf("[%03d][control string '%s'] Trace file has not been written.\n",
				$offset + 3,
				$control_string,
				gettype($tmp),
				$tmp);
			return false;
		}

		return trim(substr(file_get_contents($trace_file), 0, 100024));
	}

	$memory_funcs = array(
		'_mysqlnd_ecalloc',
		'_mysqlnd_emalloc',
		'_mysqlnd_palloc_free_thd_cache_reference',
		'_mysqlnd_pecalloc',
		'_mysqlnd_pefree',
		'_mysqlnd_pemalloc',
		'_mysqlnd_perealloc',
	);
	$trace_file = sprintf('%s%s%s', sys_get_temp_dir(), DIRECTORY_SEPARATOR, 'mysqli_debug_phpt.trace');

	$trace = try_control_string($link, 't:m:O,' . $trace_file, $trace_file, 10);
	if (!strstr($trace, 'SELECT * FROM test') && !strstr($trace, 'mysql_real_query'))
		printf("[015] SELECT query cannot be found in trace. Trace contents seems wrong.\n");

	$lines_trace = explode("\n", $trace);
	$functions_trace = array();
	foreach ($lines_trace as $k => $line) {
		$line = trim($line);
		if (preg_match("@^[|\s]*>([\w:]+)@ism", $line, $matches)) {
			$functions_trace[$matches[1]] = $matches[1];
		}
	}

	$found = 0;
	foreach ($memory_funcs as $k => $name)
		if (isset($functions_trace[$name]))
			$found++;

	if ($found < (count($memory_funcs) - 2))
		printf("[016] Only %d memory functions have been found, expecting at least %d.\n",
			$found, count($memory_funcs) - 2);

	$trace = try_control_string($link, 't:O,' . $trace_file, $trace_file, 20);
	if (!strstr($trace, 'SELECT * FROM test') && !strstr($trace, 'mysql_real_query'))
		printf("[025] SELECT query cannot be found in trace. Trace contents seems wrong.\n");

	$lines_trace = explode("\n", $trace);
	$functions_trace = array();
	foreach ($lines_trace as $k => $line) {
		$line = trim($line);
		if (preg_match("@^[|\s]*>([\w:]+)@ism", $line, $matches)) {
			$functions_trace[$matches[1]] = $matches[1];
		}
	}

	$found = 0;
	foreach ($memory_funcs as $k => $name)
		if (isset($functions_trace[$name]))
			$found++;

	if ($found > 2)
		printf("[026] More than %d memory functions have been recorded, that's strange.\n",
			$found);

	mysqli_close($link);
	@unlink($trace_file);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
done!