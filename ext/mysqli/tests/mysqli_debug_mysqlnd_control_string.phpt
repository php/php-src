--TEST--
mysqli_debug() - all control string options supported by both mysqlnd and libmysql except oOaA
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

	$trace_file = sprintf('%s%s%s', sys_get_temp_dir(), DIRECTORY_SEPARATOR, 'mysqli_debug_phpt.trace');

	$trace = try_control_string($link, 't:O,' . $trace_file, $trace_file, 10);
	if (!strstr($trace, 'SELECT * FROM test') && !strstr($trace, 'mysql_real_query'))
		printf("[015] SELECT query cannot be found in trace. Trace contents seems wrong.\n");

	// T - gettimeofday() system call, system dependent format
	// 16:57:03.350734 >mysql_real_query
	$trace = try_control_string($link, 't:O,' . $trace_file . ':T', $trace_file, 20);
	if (!preg_match('@^[012]{0,1}[0-9]{1}:[0-5]{0,1}[0-9]{1}:[0-5]{0,1}[0-9]{1}@ismU', $trace))
		printf("[025] Timestamp not found. One reason could be that the test is borked and does not recognize the format of the gettimeofday() system call. Check manually (and fix the test, if needed :-)). First characters from trace are '%s'\n", substr($trace, 0, 80));

	// i - add PID of the current process
	// currently PHP is not multi-threaded, so it should be save to test for the PID of this PHP process
	if (false === ($pid = getmypid()))
		$pid = "[\d]+";

	$trace = try_control_string($link, 't:O,' . $trace_file . ':i', $trace_file, 30);
	if (!preg_match("@^" . $pid . "*@ismU", $trace))
		printf("[035] Process ID has not been found, first characters from trace are '%s'\n", substr($trace, 0, 80));

	// L - line numbers
	$trace = try_control_string($link, 't:O,' . $trace_file . ':L', $trace_file, 40);
	if (!preg_match("@^[\d]+@ismU", $trace))
		printf("[045] Line numbers have not been found, first characters from trace are '%s'\n", substr($trace, 0, 80));

	// F - file name
	$trace = try_control_string($link, 't:O,' . $trace_file . ':F', $trace_file, 50);
	// hopefully we'll never see a file name that's not covered by this regular expression...
	if (!preg_match("@^\s*[/\w\\\\d\.\-]+\.[ch]@ismU", $trace))
		printf("[055] File names seem to be missing, first characters from trace are '%s'\n", substr($trace, 0, 80));

	// -n - print function nesting depth
	$trace = try_control_string($link, 't:O,' . $trace_file . ':n', $trace_file, 60);
	if (!preg_match("@^\d+:@ismU", $trace))
		printf("[065] Nesting level seem to be missing, first characters from trace are '%s'\n", substr($trace, 0, 80));

	// -t,[N] - maximum nesting level
	$trace = try_control_string($link, 't,1:n:O,' . $trace_file, $trace_file, 70);
	$lines = explode("\n", $trace);
	foreach ($lines as $k => $line) {
		$line = trim($line);
		if (!preg_match("@^(\d+):+@ismU", $line, $matches)) {
			printf("[075] Nesting level seem to be missing, first characters from trace are '%s'\n", substr($line, 0, 80));
		} else {
			if (!isset($matches[1]) || ((int)$matches[1] > 1)) {
				printf("[076] Nesting level seem to be %d, should not be higher than 1, first characters from trace are '%s'\n",
					$matches[1],
					substr($line, 0, 80));
			}
		}
	}

	// omitting t
	$trace = try_control_string($link, 'n:O,' . $trace_file, $trace_file, 80);
	$lines = explode("\n", $trace);
	foreach ($lines as $k => $line) {
		$line = trim($line);
		if (preg_match("@^[|\s]*>[\w]+@ism", $line, $matches)) {
			printf("[085] Looks like a function call, but there should be none in the trace file, first characters from trace are '%s'\n",
				substr($line, 0, 80));
		}
	}

	// -f[,functions] - Limit debugger list to specified functions. Empty list -> all functions
	$lines_all_funcs = explode("\n", try_control_string($link, 't:O,' . $trace_file, $trace_file, 90));
	$functions_all_funcs = array();
	foreach ($lines_all_funcs as $k => $line) {
		$line = trim($line);
		if (preg_match("@^[|\s]*>([\w:]+)@ism", $line, $matches)) {
			$functions_all_funcs[$matches[1]] = $matches[1];
		}
	}

	$lines_trace = explode("\n", try_control_string($link, 't:f:O,' . $trace_file, $trace_file, 100));
	$functions_trace = array();
	foreach ($lines_trace as $k => $line) {
		$line = trim($line);
		if (preg_match("@^[|\s]*>([\w:]+)@ism", $line, $matches)) {
			$functions_trace[$matches[1]] = $matches[1];
		}
	}

	$tmp = array_diff($functions_all_funcs, $functions_trace);
	if (!empty($tmp)) {
		printf("[105] Looks like not all functions are listed in the trace. Check manually, dumping diff.");
		var_dump($tmp);
	}

	// get two or three function names to play with...
	$test_functions = array('simple' => array(), 'doubledot' => array());

	foreach ($functions_all_funcs as $func) {
		if (count($test_functions['simple']) < 2 && !strstr($func, '::'))
			$test_functions['simple'][$func] = $func;
		else if (count($test_functions['doubledot']) < 2 && strstr($func, '::'))
			$test_functions['doubledot'][$func] = $func;
	}

	$control_string = '';
	if ($func = reset($test_functions['simple']))
			$control_string .= sprintf('%s,', $func);
	if ($func = reset($test_functions['doubledot']))
			$control_string .= sprintf('%s,', $func);
	if ($func = next($test_functions['simple']))
			$control_string .= sprintf('%s,', $func);
	if ($func = next($test_functions['doubledot']))
			$control_string .= sprintf('%s,', $func);
	$control_string = sprintf('t:f,%s:O,%s', $control_string, $trace_file);

	$lines_trace = explode("\n", try_control_string($link, $control_string, $trace_file, 110));
	$functions_trace = array();
	foreach ($lines_trace as $k => $line) {
		$line = trim($line);
		if (preg_match("@^[|\s]*>([\w:]+)@ism", $line, $matches)) {
			$functions_trace[$matches[1]] = $matches[1];
		}
	}

	foreach ($test_functions['simple'] as $func)
		if (isset($functions_trace[$func])) {
			unset($functions_trace[$func]);
			unset($test_functions['simple'][$func]);
		}

	foreach ($test_functions['doubledot'] as $func)
		if (isset($functions_trace[$func])) {
			unset($functions_trace[$func]);
			unset($test_functions['doubledot'][$func]);
		}

	if (!empty($functions_trace)) {
		printf("[115] Dumping list of unexpected functions which should have not been shown when using control string '%s'.\n",
			$control_string);
		var_dump($functions_trace);
	}
	$tmp = array_merge($test_functions['doubledot'], $test_functions['simple']);
	if (!empty($tmp)) {
		printf("[116] Dumping list of functions which should have been shown when using control string '%s'.\n",
			$control_string);
		var_dump($tmp);
	}

	if ($IS_MYSQLND) {
		// mysqlnd only option
		// m - trace memory allocations
		$trace = try_control_string($link, 't:O,' . $trace_file . ':m', $trace_file, 120);
		if (!preg_match("@^[|\s]*>\_mysqlnd_pefree@ismU", $trace, $matches) &&
				!preg_match("@^[|\s]*>\_mysqlnd_pemalloc@ismU", $trace, $matches)) {
			printf("[125] Memory dump does neither contain _mysqlnd_pefree nor _mysqlnd_pemalloc calls - check manually.\n");
			var_dump($trace);
		}

	}

	mysqli_close($link);
	print "done";
	if ($IS_MYSQLND)
		print "libmysql/DBUG package prints some debug info here.";
	@unlink($trace_file);
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
[083][control string 'n:O,%smysqli_debug_phpt.trace'] Trace file has not been written.
done%s
