<?php
/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Preston L. Bannister <pbannister@php.net>                   |
   |          Sander Roobol <sander@php.net>                              |
   |          Marcus Boerger <helly@php.net>                              |
   | (based on version by: Stig Bakken <ssb@fast.no>)                     |
   | (based on the PHP 3 test framework by Rasmus Lerdorf)                |
   +----------------------------------------------------------------------+
 */

/*

	Require exact specification of PHP executable to test (no guessing!).

	Die if any internal errors encountered in test script.

	Regularized output for simpler post-processing of output.

	Optionally output error lines indicating the failing test source and log
	for direct jump with MSVC or Emacs.

*/

/*
 * TODO:
 * - do not test PEAR components if base class and/or component class cannot be instanciated
 */

$cwd = getcwd();
set_time_limit(0);
ob_implicit_flush();
error_reporting(E_ALL);

if (ini_get('safe_mode')) {
	echo <<< SAFE_MODE_WARNING

+-----------------------------------------------------------+
|                       ! WARNING !                         |
| You are running the test-suite with "safe_mode" ENABLED ! |
|                                                           |
| Chances are high that no test will work at all,           |
| depending on how you configured "safe_mode" !             |
+-----------------------------------------------------------+


SAFE_MODE_WARNING;
}

// Don't ever guess at the PHP executable location.
// Require the explicit specification.
// Otherwise we could end up testing the wrong file!

if (getenv('TEST_PHP_EXECUTABLE')) {
	$php = getenv('TEST_PHP_EXECUTABLE');
} else {
	error("environment variable TEST_PHP_EXECUTABLE must be set to specify PHP executable!");
}

if (getenv('TEST_PHP_LOG_FORMAT')) {
	$log_format = strtoupper(getenv('TEST_PHP_LOG_FORMAT'));
} else {
	$log_format = 'LEOD';
}

if (function_exists('is_executable') && !@is_executable($php)) {
	error("invalid PHP executable specified by TEST_PHP_EXECUTABLE  = " . $php);
}

// Check whether a detailed log is wanted.

if (getenv('TEST_PHP_DETAILED')) {
	define('DETAILED', getenv('TEST_PHP_DETAILED'));
} else {
	define('DETAILED', 0);
}

// Write test context information.

echo "
=====================================================================
CWD         : $cwd
PHP         : $php
PHP_SAPI    : " . PHP_SAPI . "
PHP_VERSION : " . PHP_VERSION . "
PHP_OS      : " . PHP_OS . "
INI actual  : " . realpath(get_cfg_var('cfg_file_path')) . "
More .INIs  : " . str_replace("\n","", php_ini_scanned_files()) . "
=====================================================================
";

// Determine the tests to be run.

$test_to_run = array();
$test_files = array();
$test_results = array();

// If parameters given assume they represent selected tests to run.
if (isset($argc) && $argc > 1) {
	for ($i=1; $i<$argc; $i++) {
		$testfile = realpath($argv[$i]);
		$test_to_run[$testfile] = 1;
	}

	// Run selected tests.
	if (count($test_to_run)) {
		echo "Running selected tests.\n";
		foreach($test_to_run AS $name=>$runnable) {
			if(!preg_match("/\.phpt$/", $name))
				continue;
			echo "test: $name runnable: $runnable\n";
			if ($runnable) {
				$test_results[$name] = run_test($php,$name);
			}
		}
		exit(0);
	}
}

// Compile a list of all test files (*.phpt).
$test_files = array();
$exts_to_test = get_loaded_extensions();
$exts_tested = count($exts_to_test);
$exts_skipped = 0;
$ignored_by_ext = 0;
sort($exts_to_test);
$test_dirs = array('tests', 'pear', 'ext');

foreach ($test_dirs as $dir) {
	find_files("{$cwd}/{$dir}", ($dir == 'ext'));
}

function find_files($dir,$is_ext_dir=FALSE,$ignore=FALSE)
{
	global $test_files, $exts_to_test, $ignored_by_ext, $exts_skipped, $exts_tested;

	$o = opendir($dir) or error("cannot open directory: $dir");
	while (($name = readdir($o)) !== FALSE) {
		if (is_dir("{$dir}/{$name}") && !in_array($name, array('.', '..', 'CVS'))) {
			$skip_ext = ($is_ext_dir && !in_array($name, $exts_to_test));
			if ($skip_ext) {
				$exts_skipped++;
			}
			find_files("{$dir}/{$name}", FALSE, $ignore || $skip_ext);
		}

		// Cleanup any left-over tmp files from last run.
		if (substr($name, -4) == '.tmp') {
			@unlink("$dir/$name");
			continue;
		}

		// Otherwise we're only interested in *.phpt files.
		if (substr($name, -5) == '.phpt') {
			if ($ignore) {
				$ignored_by_ext++;
			} else {
				$testfile = realpath("{$dir}/{$name}");
				$test_files[] = $testfile;
			}
		}
	}
	closedir($o);
}

sort($test_files);

$start_time = time();

echo "TIME START " . date('Y-m-d H:i:s', $start_time) . "
=====================================================================
";

foreach ($test_files as $name) {
	$test_results[$name] = run_test($php,$name);
}

$end_time = time();

// Summarize results

if (0 == count($test_results)) {
	echo "No tests were run.\n";
	return;
}

$n_total = count($test_results);
$n_total += $ignored_by_ext;

$sum_results = array('PASSED'=>0, 'SKIPPED'=>0, 'FAILED'=>0);
foreach ($test_results as $v) {
	$sum_results[$v]++;
}
$sum_results['SKIPPED'] += $ignored_by_ext;
$percent_results = array();
while (list($v,$n) = each($sum_results)) {
	$percent_results[$v] = (100.0 * $n) / $n_total;
}

echo "
=====================================================================
TIME END " . date('Y-m-d H:i:s', $end_time) . "
=====================================================================
TEST RESULT SUMMARY
---------------------------------------------------------------------
Exts skipped    : " . sprintf("%4d",$exts_skipped) . "
Exts tested     : " . sprintf("%4d",$exts_tested) . "
---------------------------------------------------------------------
Number of tests : " . sprintf("%4d",$n_total) . "
Tests skipped   : " . sprintf("%4d (%2.1f%%)",$sum_results['SKIPPED'],$percent_results['SKIPPED']) . "
Tests failed    : " . sprintf("%4d (%2.1f%%)",$sum_results['FAILED'],$percent_results['FAILED']) . "
Tests passed    : " . sprintf("%4d (%2.1f%%)",$sum_results['PASSED'],$percent_results['PASSED']) . "
---------------------------------------------------------------------
Time taken      : " . sprintf("%4d seconds", $end_time - $start_time) . "
=====================================================================
";

//
//  Write the given text to a temporary file, and return the filename.
//

function save_text($filename,$text)
{
	$fp = @fopen($filename,'w') or error("Cannot open file '" . $filename . "' (save_text)");
	fwrite($fp,$text);
	fclose($fp);
	if (1 < DETAILED) echo "
FILE $filename {{{
$text
}}} 
";
}

//
//  Write an error in a format recognizable to Emacs or MSVC.
//

function error_report($testname,$logname,$tested) 
{
	$testname = realpath($testname);
	$logname  = realpath($logname);
	switch (strtoupper(getenv('TEST_PHP_ERROR_STYLE'))) {
	case 'MSVC':
		echo $testname . "(1) : $tested\n";
		echo $logname . "(1) :  $tested\n";
		break;
	case 'EMACS':
		echo $testname . ":1: $tested\n";
		echo $logname . ":1:  $tested\n";
		break;
	}
}

//
//  Run an individual test case.
//

function run_test($php,$file)
{
	global $log_format;

	if (DETAILED) echo "
=================
TEST $file
";

	// Load the sections of the test file.
	$section_text = array(
		'TEST'   => '(unnamed test)',
		'SKIPIF' => '',
		'GET'    => '',
		'ARGS'   => '',
	);

	$fp = @fopen($file, "r") or error("Cannot open test file: $file");

	$section = '';
	while (!feof($fp)) {
		$line = fgets($fp);

		// Match the beginning of a section.
		if (ereg('^--([A-Z]+)--',$line,$r)) {
			$section = $r[1];
			$section_text[$section] = '';
			continue;
		}
		
		// Add to the section text.
		$section_text[$section] .= $line;
	}
	fclose($fp);

	$shortname = str_replace($GLOBALS['cwd'].'/', '', $file);
	$tested = trim($section_text['TEST'])." [$shortname]";

	$tmp = realpath(dirname($file));
	$tmp_skipif = $tmp . uniqid('/phpt.');
	$tmp_file   = $tmp . uniqid('/phpt.');
	$tmp_post   = $tmp . uniqid('/phpt.');

	@unlink($tmp_skipif);
	@unlink($tmp_file);
	@unlink($tmp_post);

	// Reset environment from any previous test.

	putenv("REDIRECT_STATUS=");
	putenv("QUERY_STRING=");
	putenv("PATH_TRANSLATED=");
	putenv("SCRIPT_FILENAME=");
	putenv("REQUEST_METHOD=");
	putenv("CONTENT_TYPE=");
	putenv("CONTENT_LENGTH=");

	// unlink old test results	
	@unlink(ereg_replace('\.phpt$','.diff',$file));
	@unlink(ereg_replace('\.phpt$','.log',$file));
	@unlink(ereg_replace('\.phpt$','.exp',$file));
	@unlink(ereg_replace('\.phpt$','.out',$file));

	// Check if test should be skipped.
	if (array_key_exists('SKIPIF', $section_text)) {
		if (trim($section_text['SKIPIF'])) {
			save_text($tmp_skipif, $section_text['SKIPIF']);
			$output = `$php $tmp_skipif`;
			@unlink($tmp_skipif);
			if (trim($output) == 'skip') {
				echo "SKIP $tested\n";
				return 'SKIPPED';
			}
		}
	}

	// Any special ini settings
	$ini_settings = '';
	if (array_key_exists('INI', $section_text)) {
		foreach(preg_split( "/[\n\r]+/", $section_text['INI']) as $setting) {
			if (strlen($setting)) {
				$ini_settings .= " -d '$setting'";
			}
		}
	}

	// We've satisfied the preconditions - run the test!
	save_text($tmp_file,$section_text['FILE']);
	if (array_key_exists('GET', $section_text)) {
		$query_string = trim($section_text['GET']);
	} else {
		$query_string = '';
	}

	putenv("REDIRECT_STATUS=1");
	putenv("QUERY_STRING=$query_string");
	putenv("PATH_TRANSLATED=$tmp_file");
	putenv("SCRIPT_FILENAME=$tmp_file");

	$args = $section_text['ARGS'] ? ' -- '.$section_text['ARGS'] : '';

	if (array_key_exists('POST', $section_text) && !empty($section_text['POST'])) {

		$post = trim($section_text['POST']);
		save_text($tmp_post,$post);
		$content_length = strlen($post);

		putenv("REQUEST_METHOD=POST");
		putenv("CONTENT_TYPE=application/x-www-form-urlencoded");
		putenv("CONTENT_LENGTH=$content_length");

		$cmd = "$php$ini_settings -f $tmp_file 2>&1 < $tmp_post";

	} else {

		putenv("REQUEST_METHOD=GET");
		putenv("CONTENT_TYPE=");
		putenv("CONTENT_LENGTH=");

		$cmd = "$php$ini_settings -f $tmp_file$args 2>&1";
	}

	if (DETAILED) echo "
CONTENT_LENGTH  = " . getenv("CONTENT_LENGTH") . "
CONTENT_TYPE    = " . getenv("CONTENT_TYPE") . "
PATH_TRANSLATED = " . getenv("PATH_TRANSLATED") . "
QUERY_STRING    = " . getenv("QUERY_STRING") . "
REDIRECT_STATUS = " . getenv("REDIRECT_STATUS") . "
REQUEST_METHOD  = " . getenv("REQUEST_METHOD") . "
SCRIPT_FILENAME = " . getenv("SCRIPT_FILENAME") . "
COMMAND $cmd
";

	$out = `$cmd`;

	@unlink($tmp_post);
	@unlink($tmp_file);

	// Does the output match what is expected?
	$output = trim($out);
	$output = preg_replace('/\r\n/',"\n",$output);

	if (isset($section_text['EXPECTF'])) {
		$wanted = trim($section_text['EXPECTF']);
		$wanted_re = preg_replace('/\r\n/',"\n",$wanted);
		$wanted_re = preg_quote($wanted_re, '/');
		// Stick to basics
		$wanted_re = str_replace("%s", ".+?", $wanted_re); //not greedy
		$wanted_re = str_replace("%i", "[0-9]+", $wanted_re);
		$wanted_re = str_replace("%f", "[0-9\.+\-]+", $wanted_re);
/* DEBUG YOUR REGEX HERE
		var_dump($wanted);
		print(str_repeat('=', 80) . "\n");
		var_dump($output);
*/
		if (preg_match("/$wanted_re/s", $output)) {
			echo "PASS $tested\n";
			return 'PASSED';
		}

	} else {
		$wanted = trim($section_text['EXPECT']);
		$wanted = preg_replace('/\r\n/',"\n",$wanted);
	// compare and leave on success
		$ok = (0 == strcmp($output,$wanted));
		if ($ok) {
			echo "PASS $tested\n";
			return 'PASSED';
		}
	}

	// Test failed so we need to report details.
	echo "FAIL $tested\n";


	// write .exp
	if (strpos($log_format,'E') !== FALSE) {
		$logname = ereg_replace('\.phpt$','.exp',$file);
		$log = fopen($logname,'w') or error("Cannot create test log - $logname");
		fwrite($log,$wanted);
		fclose($log);
	}

	// write .out
	if (strpos($log_format,'O') !== FALSE) {
		$logname = ereg_replace('\.phpt$','.out',$file);
		$log = fopen($logname,'w') or error("Cannot create test log - $logname");
		fwrite($log,$output);
		fclose($log);
	}

	// write .diff
	if (strpos($log_format,'D') !== FALSE) {
		$logname = ereg_replace('\.phpt$','.diff',$file);
		$log = fopen($logname,'w') or error("Cannot create test log - $logname");
		fwrite($log,generate_diff($wanted,$output));
		fclose($log);
	}

	// write .log
	if (strpos($log_format,'L') !== FALSE) {
		$logname = ereg_replace('\.phpt$','.log',$file);
		$log = fopen($logname,'w') or error("Cannot create test log - $logname");
		fwrite($log,"
---- EXPECTED OUTPUT
$wanted
---- ACTUAL OUTPUT
$output
---- FAILED
");
		fclose($log);
		error_report($file,$logname,$tested);
	}

	return 'FAILED';
}

function generate_diff($wanted,$output)
{
	$w = explode("\n", $wanted);
	$o = explode("\n", $output);
	$w1 = array_diff($w,$o);
	$o1 = array_diff($o,$w);
	$w2 = array();
	$o2 = array();
	foreach($w1 as $idx => $val) $w2[sprintf("%03d<",$idx)] = sprintf("%03d- ", $idx+1).$val;
	foreach($o1 as $idx => $val) $o2[sprintf("%03d>",$idx)] = sprintf("%03d+ ", $idx+1).$val;
	$diff = array_merge($w2, $o2);
	ksort($diff);
	return implode("\r\n", $diff);
}

function error($message)
{
	echo "ERROR: {$message}\n";
	exit(1);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
?>
