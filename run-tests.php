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
   | Authors: Ilia Alshanetsky <iliaa@php.net>                            |
   |          Preston L. Bannister <pbannister@php.net>                   |
   |          Marcus Boerger <helly@php.net>                              |
   |          Derick Rethans <derick@php.net>                             |
   |          Sander Roobol <sander@php.net>                              |
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

// change into the PHP source directory.

if (getenv('TEST_PHP_SRCDIR')) {
	@chdir(getenv('TEST_PHP_SRCDIR'));
}

$cwd = getcwd();
set_time_limit(0);
while(ob_get_level()) {
	ob_end_clean();
}
error_reporting(E_ALL);
ini_set('magic_quotes_runtime',0); // this would break tests by modifying EXPECT sections

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

// Check whether user test dirs are requested.
if (getenv('TEST_PHP_USER')) {
	$user_tests = explode (',', getenv('TEST_PHP_USER'));
} else {
	$user_tests = array();
}

// Get info from php
$info_file = realpath(dirname(__FILE__)) . '/run-test-info.php';
@unlink($info_file);
$php_info = '<?php echo "
PHP_SAPI    : " . PHP_SAPI . "
PHP_VERSION : " . phpversion() . "
ZEND_VERSION: " . zend_version() . "
PHP_OS      : " . PHP_OS . "
INI actual  : " . realpath(get_cfg_var("cfg_file_path")) . "
More .INIs  : " . (function_exists(\'php_ini_scanned_files\') ? str_replace("\n","", php_ini_scanned_files()) : "** not determined **"); ?>';
save_text($info_file, $php_info);
$ini_overwrites = array(
		'output_handler=',
		'zlib.output_compression=Off',
		'open_basedir=',
		'safe_mode=0',
		'disable_functions=',
		'output_buffering=Off',
		'error_reporting=2047',
		'display_errors=1',
		'log_errors=0',
		'html_errors=0',
		'track_errors=1',
		'report_memleaks=1',
		'report_zend_debug=0',
		'docref_root=/phpmanual/',
		'docref_ext=.html',
		'error_prepend_string=',
		'error_append_string=',
		'auto_prepend_file=',
		'auto_append_file=',
		'magic_quotes_runtime=0',
	);
$info_params = array();
settings2array($ini_overwrites,$info_params);
settings2params($info_params);
$php_info = `$php $info_params $info_file`;
@unlink($info_file);

// Write test context information.

echo "
=====================================================================
CWD         : $cwd
PHP         : $php $php_info
Extra dirs  : ";
foreach ($user_tests as $test_dir) {
	echo "{$test_dir}\n              ";
}
echo "
=====================================================================
";

// Determine the tests to be run.

$test_files = array();
$test_results = array();
$GLOBALS['__PHP_FAILED_TESTS__'] = array();

// If parameters given assume they represent selected tests to run.
if (isset($argc) && $argc > 1) {
	for ($i=1; $i<$argc; $i++) {
		$testfile = realpath($argv[$i]);
		if (is_dir($testfile)) {
			find_files($testfile);
		} else if(preg_match("/\.phpt$/", $testfile)) {
			$test_files[] = $testfile;
		}
	}

	// Run selected tests.
	if (count($test_files)) {
		usort($test_files, "test_sort");
		echo "Running selected tests.\n";
		foreach($test_files AS $name) {
			$test_results[$name] = run_test($php,$name);
		}
		if(getenv('REPORT_EXIT_STATUS') == 1 and ereg('FAILED( |$)', implode(' ', $test_results))) {
			exit(1);
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

foreach ($user_tests as $dir) {
	find_files($dir, ($dir == 'ext'));
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

function test_sort($a, $b) {
	global $cwd;

	$ta = strpos($a, "{$cwd}/tests")===0 ? 1 + (strpos($a, "{$cwd}/tests/run-test")===0 ? 1 : 0) : 0;
	$tb = strpos($b, "{$cwd}/tests")===0 ? 1 + (strpos($b, "{$cwd}/tests/run-test")===0 ? 1 : 0) : 0;
	if ($ta == $tb) {
		return strcmp($a, $b);
	} else {
		return $tb - $ta;
	}
}

usort($test_files, "test_sort");

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

$failed_test_summary = '';
if (count($GLOBALS['__PHP_FAILED_TESTS__'])) {
$failed_test_summary .= "
=====================================================================
FAILED TEST SUMMARY
---------------------------------------------------------------------
";
foreach ($GLOBALS['__PHP_FAILED_TESTS__'] as $failed_test_data) {
$failed_test_summary .=  $failed_test_data['test_name'] . "\n";
}
$failed_test_summary .=  "=====================================================================
";
}

if ($failed_test_summary && !getenv('NO_PHPTEST_SUMMARY')) {
	echo $failed_test_summary;
}

define('PHP_QA_EMAIL', 'php-qa@lists.php.net');
define('QA_SUBMISSION_PAGE', 'http://qa.php.net/buildtest-process.php');

/* We got failed Tests, offer the user to send and e-mail to QA team, unless NO_INTERACTION is set */
if ($sum_results['FAILED'] && !getenv('NO_INTERACTION')) {
	$fp = fopen("php://stdin", "r+");
	echo "Some tests have failed, would you like to send the\nreport to PHP's QA team\n";
	echo "(choose \"s\" to just save the results to a file)? [Yns]: ";
	flush();
	$user_input = fgets($fp, 10);
	$just_save_results = (strtolower($user_input[0]) == 's');
	
	if ($just_save_results || strlen(trim($user_input)) == 0 || strtolower($user_input[0]) == 'y') {
		/*  
		 * Collect information about the host system for our report
		 * Fetch phpinfo() output so that we can see the PHP enviroment
		 * Make an archive of all the failed tests
		 * Send an email
		 */
		
		$failed_tests_data = '';
		$sep = "\n" . str_repeat('=', 80) . "\n";
		
		$failed_tests_data .= $failed_test_summary . "\n";
		
		foreach ($GLOBALS['__PHP_FAILED_TESTS__'] as $test_info) {
			$failed_tests_data .= $sep . $test_info['name'];
			$failed_tests_data .= $sep . file_get_contents(realpath($test_info['output']));
			$failed_tests_data .= $sep . file_get_contents(realpath($test_info['diff']));
			$failed_tests_data .= $sep . "\n\n";
		}
		
		$failed_tests_data .= "\n" . $sep . 'BUILD ENVIRONMENT' . $sep;
		$failed_tests_data .= "OS:\n". PHP_OS. "\n\n";
		$automake = $autoconf = $libtool = $compiler = 'N/A';
		if (substr(PHP_OS, 0, 3) != "WIN") {
			$automake = shell_exec('automake --version');
			$autoconf = shell_exec('autoconf --version');
			$libtool = shell_exec('libtool --version');
			/* Try the most common flags for 'version' */
			$flags = array('-v', '-V', '--version');
			$cc_status=0;
			foreach($flags AS $flag) {
				system(getenv('CC')." $flag >/dev/null 2>&1", $cc_status);
				if($cc_status == 0) {
					$compiler = shell_exec(getenv('CC')." $flag 2>&1");
					break;
				}
			}
		}
		$failed_tests_data .= "Automake:\n$automake\n";
		$failed_tests_data .= "Autoconf:\n$autoconf\n";
		$failed_tests_data .= "Libtool:\n$libtool\n";
		$failed_tests_data .= "Compiler:\n$compiler\n";
		$failed_tests_data .= "Bison:\n". @shell_exec('bison --version'). "\n";
		$failed_tests_data .= "\n\n";
		
		$failed_tests_data .= $sep . "PHPINFO" . $sep;
		$failed_tests_data .= shell_exec($php.' -dhtml_errors=0 -i');
		
		$compression = 0;
		
		if ($just_save_results || !mail_qa_team($failed_tests_data, $compression)) {
			$output_file = 'php_test_results_' . date('Ymd') . ( $compression ? '.txt.gz' : '.txt' );
			$fp = fopen($output_file, "w");
			fwrite($fp, $failed_tests_data);
			fclose($fp);
		
			if (!$just_save_results)
			    echo "\nThe test script was unable to automatically send the report to PHP's QA Team\n";
			echo "Please send ".$output_file." to ".PHP_QA_EMAIL." manually, thank you.\n";
		} else {
			fwrite($fp, "\nThank you for helping to make PHP better.\n");
			fclose($fp);
		}	
	}
}
 
if(getenv('REPORT_EXIT_STATUS') == 1 and $sum_results['FAILED']) {
	exit(1);
}

//
// Send Email to QA Team
//

function mail_qa_team($data, $compression)
{
	$url_bits = parse_url(QA_SUBMISSION_PAGE);
	if (empty($url_bits['port'])) $url_bits['port'] = 80;
	
	$data = "php_test_data=" . urlencode(base64_encode(preg_replace("/[\\x00]/", "[0x0]", $data)));
	$data_length = strlen($data);
	
	$fs = fsockopen($url_bits['host'], $url_bits['port'], $errno, $errstr, 10);
	if (!$fs) {
		return FALSE;
	}

	echo "Posting to {$url_bits['host']} {$url_bits['path']}\n";
	fwrite($fs, "POST ".$url_bits['path']." HTTP/1.1\r\n");
	fwrite($fs, "Host: ".$url_bits['host']."\r\n");
	fwrite($fs, "User-Agent: QA Browser 0.1\r\n");
	fwrite($fs, "Content-Type: application/x-www-form-urlencoded\r\n");
	fwrite($fs, "Content-Length: ".$data_length."\r\n\r\n");
	fwrite($fs, $data);
	fwrite($fs, "\r\n\r\n");
	fclose($fs);

	return 1;
} 
 
 
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
	global $log_format, $info_params, $ini_overwrites;

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
	$tmp_file   = ereg_replace('\.phpt$','.php',$file);
	$tmp_post   = $tmp . uniqid('/phpt.');

	@unlink($tmp_skipif);
	@unlink($tmp_file);
	@unlink($tmp_post);

	// unlink old test results	
	@unlink(ereg_replace('\.phpt$','.diff',$file));
	@unlink(ereg_replace('\.phpt$','.log',$file));
	@unlink(ereg_replace('\.phpt$','.exp',$file));
	@unlink(ereg_replace('\.phpt$','.out',$file));

	// Reset environment from any previous test.
	putenv("REDIRECT_STATUS=");
	putenv("QUERY_STRING=");
	putenv("PATH_TRANSLATED=");
	putenv("SCRIPT_FILENAME=");
	putenv("REQUEST_METHOD=");
	putenv("CONTENT_TYPE=");
	putenv("CONTENT_LENGTH=");

	// Check if test should be skipped.
	if (array_key_exists('SKIPIF', $section_text)) {
		if (trim($section_text['SKIPIF'])) {
			save_text($tmp_skipif, $section_text['SKIPIF']);
			$output = `$php $info_params $tmp_skipif`;
			@unlink($tmp_skipif);
			if (eregi("^skip", trim($output))) {
				echo "SKIP $tested";
				$reason = (ereg("^skip[[:space:]]*(.+)\$", trim($output))) ? ereg_replace("^skip[[:space:]]*(.+)\$", "\\1", trim($output)) : FALSE;
				if ($reason) {
					echo " (reason: $reason)\n";
				} else {
					echo "\n";
				}
				return 'SKIPPED';
			}
			if (eregi("^info", trim($output))) {
				$reason = (ereg("^info[[:space:]]*(.+)\$", trim($output))) ? ereg_replace("^info[[:space:]]*(.+)\$", "\\1", trim($output)) : FALSE;
				if ($reason) {
					$tested .= " (info: $reason)";
				}
			}
		}
	}

	// Default ini settings
	$ini_settings = array();
	// additional ini overwrites
	//$ini_overwrites[] = 'setting=value';
	settings2array($ini_overwrites, $ini_settings);

	// Any special ini settings 
	// these may overwrite the test defaults...
	if (array_key_exists('INI', $section_text)) {
		settings2array(preg_split( "/[\n\r]+/", $section_text['INI']), $ini_settings);
	}
	settings2params($ini_settings);

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

	// Does the output match what is expected?
	$output = trim($out);
	$output = preg_replace('/\r\n/',"\n",$output);

	if (isset($section_text['EXPECTF']) || isset($section_text['EXPECTREGEX'])) {
		if (isset($section_text['EXPECTF'])) {
			$wanted = trim($section_text['EXPECTF']);
		} else {
			$wanted = trim($section_text['EXPECTREGEX']);
		}
		$wanted_re = preg_replace('/\r\n/',"\n",$wanted);
		if (isset($section_text['EXPECTF'])) {
			$wanted_re = preg_quote($wanted_re, '/');
			// Stick to basics
			$wanted_re = str_replace("%s", ".+?", $wanted_re); //not greedy
			$wanted_re = str_replace("%i", "[+\-]?[0-9]+", $wanted_re);
			$wanted_re = str_replace("%d", "[0-9]+", $wanted_re);
			$wanted_re = str_replace("%x", "[0-9a-fA-F]+", $wanted_re);
			$wanted_re = str_replace("%f", "[+\-]?\.?[0-9]+\.?[0-9]*", $wanted_re);
			$wanted_re = str_replace("%c", ".", $wanted_re);
			// %f allows two points "-.0.0" but that is the best *simple* expression
		}
/* DEBUG YOUR REGEX HERE
		var_dump($wanted_re);
		print(str_repeat('=', 80) . "\n");
		var_dump($output);
*/
		if (preg_match("/^$wanted_re\$/s", $output)) {
			@unlink($tmp_file);
			echo "PASS $tested\n";
			return 'PASSED';
		}

	} else {
		$wanted = trim($section_text['EXPECT']);
		$wanted = preg_replace('/\r\n/',"\n",$wanted);
	// compare and leave on success
		$ok = (0 == strcmp($output,$wanted));
		if ($ok) {
			@unlink($tmp_file);
			echo "PASS $tested\n";
			return 'PASSED';
		}
	}

	// Test failed so we need to report details.
	echo "FAIL $tested\n";

	$GLOBALS['__PHP_FAILED_TESTS__'][] = array(
						'name' => $file,
						'test_name' => $tested,
						'output' => ereg_replace('\.phpt$','.log', $file),
						'diff'   => ereg_replace('\.phpt$','.diff', $file)
						);

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
	$w1 = array_diff_assoc($w,$o);
	$o1 = array_diff_assoc($o,$w);
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

function settings2array($settings, &$ini_settings)
{
	foreach($settings as $setting) {
		if (strpos($setting, '=')!==false) {
			$setting = explode("=", $setting, 2);
			$name = trim(strtolower($setting[0]));
			$value = trim($setting[1]);
			$ini_settings[$name] = $value;
		}
	}
}

function settings2params(&$ini_settings)
{
	if (count($ini_settings)) {
		$settings = '';
		foreach($ini_settings as $name => $value) {
			$value = addslashes($value);
			$settings .= " -d \"$name=$value\"";
		}
		$ini_settings = $settings;
	} else {
		$ini_settings = '';
	}
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
