<?php
/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2004 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Ilia Alshanetsky <iliaa@php.net>                            |
   |          Preston L. Bannister <pbannister@php.net>                   |
   |          Marcus Boerger <helly@php.net>                              |
   |          Derick Rethans <derick@php.net>                             |
   |          Sander Roobol <sander@php.net>                              |
   | (based on version by: Stig Bakken <ssb@php.net>)                     |
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


/* Sanity check to ensure that pcre extension needed by this script is available.
 * In the event it is not, print a nice error message indicating that this script will
 * not run without it.
 */

if (!extension_loaded("pcre")) {
	echo <<<NO_PCRE_ERROR

+-----------------------------------------------------------+
|                       ! ERROR !                           |
| The test-suite requires that you have pcre extension      |
| enabled. To enable this extension either compile your PHP |
| with --with-pcre-regex or if you've compiled pcre as a    |
| shared module load it via php.ini.                        |
+-----------------------------------------------------------+

NO_PCRE_ERROR;
exit;
}

if (!function_exists("proc_open")) {
	echo <<<NO_PROC_OPEN_ERROR

+-----------------------------------------------------------+
|                       ! ERROR !                           |
| The test-suite requires that proc_open() is available.    |
| Please check if you disabled it in php.ini.               |
+-----------------------------------------------------------+

NO_PROC_OPEN_ERROR;
exit;
}

// change into the PHP source directory.

if (getenv('TEST_PHP_SRCDIR')) {
	@chdir(getenv('TEST_PHP_SRCDIR'));
}

// Delete some security related environment variables
putenv('SSH_CLIENT=deleted');
putenv('SSH_AUTH_SOCK=deleted');
putenv('SSH_TTY=deleted');

$cwd = getcwd();
set_time_limit(0);

// delete as much output buffers as possible
while(@ob_end_clean());
if (ob_get_level()) echo "Not all buffers were deleted.\n";

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
	if ($php=='auto') {
		$php = $cwd.'/sapi/cli/php';
		putenv("TEST_PHP_EXECUTABLE=$php");
	}
}

if (empty($php) || !file_exists($php)) {
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
PHP_OS      : " . PHP_OS . " - " . php_uname() . "
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
		'error_reporting=4095',
		'display_errors=1',
		'log_errors=0',
		'html_errors=0',
		'track_errors=1',
		'report_memleaks=1',
		'report_zend_debug=0',
		'docref_root=',
		'docref_ext=.html',
		'error_prepend_string=',
		'error_append_string=',
		'auto_prepend_file=',
		'auto_append_file=',
		'magic_quotes_runtime=0',
		'xdebug.default_enable=0',
		'session.auto_start=0'
	);
$info_params = array();
settings2array($ini_overwrites,$info_params);
settings2params($info_params);
$php_info = `$php $info_params $info_file`;
@unlink($info_file);
define('TESTED_PHP_VERSION', `$php -r 'echo PHP_VERSION;'`);

// Write test context information.
function write_information()
{
	global $cwd, $php, $php_info, $user_tests;

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
}

// Determine the tests to be run.

$test_files = array();
$test_results = array();
$PHP_FAILED_TESTS = array();

// If parameters given assume they represent selected tests to run.
$failed_tests_file= false;
$pass_option_n = false;
$pass_options = '';
if (isset($argc) && $argc > 1) {
	for ($i=1; $i<$argc; $i++) {
		if (substr($argv[$i],0,1) == '-') {   
			$switch = strtolower(substr($argv[$i],1,1));
			switch($switch) {
				case 'r':
				case 'l':
					$test_list = @file($argv[++$i]);
					if (is_array($test_list) && count($test_list)) {
						$test_files = array_merge($test_files, $test_list);
					}
					if ($switch != 'l') {
						break;
					}
					$i--;
				case 'w':
					$failed_tests_file = fopen($argv[++$i], 'w+t');
					break;
				case 'a':
					$failed_tests_file = fopen($argv[++$i], 'a+t');
					break;
				case 'n':
					if (!$pass_option_n) {
						$pass_options .= ' -n';
					}
					$pass_option_n = true;
					break;
				default:
					echo "Illegal switch specified!\n";
				case "h":
				case "help":
				case "-help":
					echo <<<HELP
Synopsis:
    php run-tests.php [options] [files] [directories]

Options:
    -l <file>   Read the testfiles to be executed from <file>. After the test 
                has finished all failed tests are written to the same <file>. 
                If the list is empty and no further test is specified then
                all tests are executed.

    -r <file>   Read the testfiles to be executed from <file>.

    -w <file>   Write a list of all failed tests to <file>.

    -a <file>   Same as -w but append rather then truncating <file>.

    -n          Pass -n option to the php binary (Do not use a php.ini).

    -h <file>   This Help.

HELP;
					exit(1);
			}
		} else {
			$testfile = realpath($argv[$i]);
			if (is_dir($testfile)) {
				find_files($testfile);
			} else if (preg_match("/\.phpt$/", $testfile)) {
				$test_files[] = $testfile;
			}
		}
	}
	$test_files = array_unique($test_files);
	for($i = 0; $i < count($test_files); $i++) {
		$test_files[$i] = trim($test_files[$i]);
	}

	// Run selected tests.
	$test_cnt = count($test_files);
	if ($test_cnt) {
		write_information();
		usort($test_files, "test_sort");
		echo "Running selected tests.\n";
		$start_time = time();
		$test_idx = 0;
		foreach($test_files AS $name) {
			$test_results[$name] = run_test($php,$name,$test_cnt,++$test_idx);
			if ($failed_tests_file && ($test_results[$name] == 'FAILED' || $test_results[$name] == 'WARNED')) {
				fwrite($failed_tests_file, "$name\n");
			}
		}
		if ($failed_tests_file) {
			fclose($failed_tests_file);
		}
		$end_time = time();
		if (count($test_files) > 1) {
			echo "
=====================================================================";
			compute_summary();
			echo get_summary(false);
		}
		if (getenv('REPORT_EXIT_STATUS') == 1 and ereg('FAILED( |$)', implode(' ', $test_results))) {
			exit(1);
		}
		exit(0);
	}
}

write_information();

// Compile a list of all test files (*.phpt).
$test_files = array();
$exts_to_test = get_loaded_extensions();
$exts_tested = count($exts_to_test);
$exts_skipped = 0;
$ignored_by_ext = 0;
sort($exts_to_test);
$test_dirs = array('tests', 'ext');
$optionals = array('pear', 'Zend', 'ZendEngine2');
foreach($optionals as $dir) {
	if (@filetype($dir) == 'dir') {
		$test_dirs[] = $dir;
	}
}

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

function test_sort($a, $b)
{
	global $cwd;

	$ta = strpos($a, "{$cwd}/tests")===0 ? 1 + (strpos($a, "{$cwd}/tests/run-test")===0 ? 1 : 0) : 0;
	$tb = strpos($b, "{$cwd}/tests")===0 ? 1 + (strpos($b, "{$cwd}/tests/run-test")===0 ? 1 : 0) : 0;
	if ($ta == $tb) {
		return strcmp($a, $b);
	} else {
		return $tb - $ta;
	}
}

$test_files = array_unique($test_files);
usort($test_files, "test_sort");

$start_time = time();

echo "TIME START " . date('Y-m-d H:i:s', $start_time) . "
=====================================================================
";

$test_cnt = count($test_files);
$test_idx = 0;
foreach ($test_files as $name) {
	$test_results[$name] = run_test($php,$name,$test_cnt,++$test_idx);
}

$end_time = time();

// Summarize results

if (0 == count($test_results)) {
	echo "No tests were run.\n";
	return;
}

compute_summary();

echo "
=====================================================================
TIME END " . date('Y-m-d H:i:s', $end_time);

$summary = get_summary(true);
echo $summary;

define('PHP_QA_EMAIL', 'qa-reports@lists.php.net');
define('QA_SUBMISSION_PAGE', 'http://qa.php.net/buildtest-process.php');

/* We got failed Tests, offer the user to send an e-mail to QA team, unless NO_INTERACTION is set */
if (!getenv('NO_INTERACTION')) {
	$fp = fopen("php://stdin", "r+");
	echo "\nYou may have found a problem in PHP.\nWe would like to send this report automatically to the\n";
	echo "PHP QA team, to give us a better understanding of how\nthe test cases are doing. If you don't want to send it\n";
	echo "immediately, you can choose \"s\" to save the report to\na file that you can send us later.\n";
	echo "Do you want to send this report now? [Yns]: ";
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
		
		/* Ask the user to provide an email address, so that QA team can contact the user */
		if (!strncasecmp($user_input, 'y', 1) || strlen(trim($user_input)) == 0) {
			echo "\nPlease enter your email address.\n(Your address will be mangled so that it will not go out on any\nmailinglist in plain text): ";
			flush();
			$user_email = trim(fgets($fp, 1024));
			$user_email = str_replace("@", " at ", str_replace(".", " dot ", $user_email));
		}
		
		$failed_tests_data = '';
		$sep = "\n" . str_repeat('=', 80) . "\n";
		
		$failed_tests_data .= $failed_test_summary . "\n";
		$failed_tests_data .= $summary . "\n";

		if ($sum_results['FAILED']) {
			foreach ($PHP_FAILED_TESTS as $test_info) {
				$failed_tests_data .= $sep . $test_info['name'] . $test_info['info'];
				$failed_tests_data .= $sep . file_get_contents(realpath($test_info['output']));
				$failed_tests_data .= $sep . file_get_contents(realpath($test_info['diff']));
				$failed_tests_data .= $sep . "\n\n";
			}
			$status = "failed";
		} else {
			$status = "success";
		}
		
		$failed_tests_data .= "\n" . $sep . 'BUILD ENVIRONMENT' . $sep;
		$failed_tests_data .= "OS:\n" . PHP_OS . " - " . php_uname() . "\n\n";
		$ldd = $automake = $autoconf = $sys_libtool = $libtool = $compiler = 'N/A';

		if (substr(PHP_OS, 0, 3) != "WIN") {
			$automake = shell_exec('automake --version');
			$autoconf = shell_exec('autoconf --version');
			/* Always use the generated libtool - Mac OSX uses 'glibtool' */
			$libtool = shell_exec($_SERVER['PWD'] . '/libtool --version');

			/* Use shtool to find out if there is glibtool present (MacOSX) */
			$sys_libtool_path = shell_exec(dirname(__FILE__) . "/build/shtool path glibtool libtool");
			$sys_libtool = shell_exec(str_replace("\n", "", $sys_libtool_path) . ' --version');

			/* Try the most common flags for 'version' */
			$flags = array('-v', '-V', '--version');
			$cc_status=0;
			foreach($flags AS $flag) {
				system(getenv('CC')." $flag >/dev/null 2>&1", $cc_status);
				if ($cc_status == 0) {
					$compiler = shell_exec(getenv('CC')." $flag 2>&1");
					break;
				}
			}
			$ldd = shell_exec("ldd $php 2>/dev/null");
		}
		$failed_tests_data .= "Automake:\n$automake\n";
		$failed_tests_data .= "Autoconf:\n$autoconf\n";
		$failed_tests_data .= "Bundled Libtool:\n$libtool\n";
		$failed_tests_data .= "System Libtool:\n$sys_libtool\n";
		$failed_tests_data .= "Compiler:\n$compiler\n";
		$failed_tests_data .= "Bison:\n". @shell_exec('bison --version'). "\n";
		$failed_tests_data .= "Libraries:\n$ldd\n";
		$failed_tests_data .= "\n";
		
		if (isset($user_email)) {
			$failed_tests_data .= "User's E-mail: ".$user_email."\n\n";
		}	
		
		$failed_tests_data .= $sep . "PHPINFO" . $sep;
		$failed_tests_data .= shell_exec($php.' -dhtml_errors=0 -i');
		
		$compression = 0;
		
		if ($just_save_results || !mail_qa_team($failed_tests_data, $compression, $status)) {
			$output_file = 'php_test_results_' . date('Ymd_Hi') . ( $compression ? '.txt.gz' : '.txt' );
			$fp = fopen($output_file, "w");
			fwrite($fp, $failed_tests_data);
			fclose($fp);
		
			if (!$just_save_results) {
			    echo "\nThe test script was unable to automatically send the report to PHP's QA Team\n";
			}

			echo "Please send ".$output_file." to ".PHP_QA_EMAIL." manually, thank you.\n";
		} else {
			fwrite($fp, "\nThank you for helping to make PHP better.\n");
			fclose($fp);
		}	
	}
}
 
if (getenv('REPORT_EXIT_STATUS') == 1 and $sum_results['FAILED']) {
	exit(1);
}

//
// Send Email to QA Team
//

function mail_qa_team($data, $compression, $status = FALSE)
{
	$url_bits = parse_url(QA_SUBMISSION_PAGE);
	if (empty($url_bits['port'])) $url_bits['port'] = 80;
	
	$data = "php_test_data=" . urlencode(base64_encode(preg_replace("/[\\x00]/", "[0x0]", $data)));
	$data_length = strlen($data);
	
	$fs = fsockopen($url_bits['host'], $url_bits['port'], $errno, $errstr, 10);
	if (!$fs) {
		return FALSE;
	}

	$php_version = urlencode(TESTED_PHP_VERSION);

	echo "\nPosting to {$url_bits['host']} {$url_bits['path']}\n";
	fwrite($fs, "POST ".$url_bits['path']."?status=$status&version=$php_version HTTP/1.1\r\n");
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

function system_with_timeout($commandline)
{
	$data = "";
	
	$proc = proc_open($commandline, array(
		0 => array('pipe', 'r'),
		1 => array('pipe', 'w'),
		2 => array('pipe', 'w')
		), $pipes, null, null, array("suppress_errors" => true));

	if (!$proc)
		return false;

	fclose($pipes[0]);

	while (true) {
		/* hide errors from interrupted syscalls */
		$r = $pipes;
		$w = null;
		$e = null;
		$n = @stream_select($r, $w, $e, 60);

		if ($n === 0) {
			/* timed out */
			$data .= "\n ** ERROR: process timed out **\n";
			proc_terminate($proc);
			return $data;
		} else if ($n > 0) {
			$line = fread($pipes[1], 8192);
			if (strlen($line) == 0) {
				/* EOF */
				break;
			}
			$data .= $line;
		}
	}
	$stat = proc_get_status($proc);
	if ($stat['signaled']) {
		$data .= "\nTermsig=".$stat['stopsig'];
	}
	$code = proc_close($proc);
	return $data;
}

//
//  Run an individual test case.
//

function run_test($php, $file, $test_cnt, $test_idx)
{
	global $log_format, $info_params, $ini_overwrites, $cwd, $PHP_FAILED_TESTS, $pass_options;

	if (DETAILED) echo "
=================
TEST $file
";

	// Load the sections of the test file.
	$section_text = array(
		'TEST'   => '',
		'SKIPIF' => '',
		'GET'    => '',
		'ARGS'   => '',
	);

	$fp = @fopen($file, "r") or error("Cannot open test file: $file");

	if (!feof($fp)) {
		$line = fgets($fp);
	} else {
		echo "BORK empty test [$file]\n";
		return 'BORKED';
	}
	if (!ereg('^--TEST--',$line,$r)) {
		echo "BORK tests must start with --TEST-- [$file]\n";
		return 'BORKED';
	}
	$section = 'TEST';
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
	if (@count($section_text['FILE']) != 1) {
		echo "BORK missing section --FILE-- [$file]\n";
		return 'BORKED';
	}
	if ((@count($section_text['EXPECT']) + @count($section_text['EXPECTF']) + @count($section_text['EXPECTREGEX'])) != 1) {
		echo "BORK missing section --EXPECT--, --EXPECTF-- or --EXPECTREGEX-- [$file]\n";
		return 'BORKED';
	}
	fclose($fp);

 	/* For GET/POST tests, check if cgi sapi is available and if it is, use it. */
 	if ((!empty($section_text['GET']) || !empty($section_text['POST']))) {
 		if (file_exists("./sapi/cgi/php")) {
 			$old_php = $php;
 			$php = realpath("./sapi/cgi/php") . ' -C ';
 		}
 	}

	$shortname = str_replace($cwd.'/', '', $file);
	$tested = trim($section_text['TEST'])." [$shortname]";

	echo "TEST $test_idx/$test_cnt [$shortname]\r";
	flush();

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
	$info = '';
	$warn = false;
	if (array_key_exists('SKIPIF', $section_text)) {
		if (trim($section_text['SKIPIF'])) {
			save_text($tmp_skipif, $section_text['SKIPIF']);
			$extra = substr(PHP_OS, 0, 3) !== "WIN" ?
				"unset REQUEST_METHOD; unset QUERY_STRING; unset PATH_TRANSLATED; unset SCRIPT_FILENAME; unset REQUEST_METHOD;": "";
			$output = system_with_timeout("$extra $php -q $info_params $tmp_skipif");
			@unlink($tmp_skipif);
			if (eregi("^skip", trim($output))) {
				echo "SKIP $tested";
				$reason = (eregi("^skip[[:space:]]*(.+)\$", trim($output))) ? eregi_replace("^skip[[:space:]]*(.+)\$", "\\1", trim($output)) : FALSE;
				if ($reason) {
					echo " (reason: $reason)\n";
				} else {
					echo "\n";
				}
				if (isset($old_php)) {
					$php = $old_php;
				}
				return 'SKIPPED';
			}
			if (eregi("^info", trim($output))) {
				$reason = (ereg("^info[[:space:]]*(.+)\$", trim($output))) ? ereg_replace("^info[[:space:]]*(.+)\$", "\\1", trim($output)) : FALSE;
				if ($reason) {
					$info = " (info: $reason)";
				}
			}
			if (eregi("^warn", trim($output))) {
				$reason = (ereg("^warn[[:space:]]*(.+)\$", trim($output))) ? ereg_replace("^warn[[:space:]]*(.+)\$", "\\1", trim($output)) : FALSE;
				if ($reason) {
					$warn = true; /* only if there is a reason */
					$info = " (warn: $reason)";
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

		$cmd = "$php$pass_options$ini_settings -f \"$tmp_file\" 2>&1 < $tmp_post";

	} else {

		putenv("REQUEST_METHOD=GET");
		putenv("CONTENT_TYPE=");
		putenv("CONTENT_LENGTH=");

		$cmd = "$php$pass_options$ini_settings -f \"$tmp_file\" $args 2>&1";
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

//	$out = `$cmd`;
	$out = system_with_timeout($cmd);

	@unlink($tmp_post);

	// Does the output match what is expected?
	$output = trim($out);
	$output = preg_replace('/\r\n/',"\n",$output);

	/* when using CGI, strip the headers from the output */
	if (isset($old_php) && ($pos = strpos($output, "\n\n")) !== FALSE) {
		$output = substr($output, ($pos + 2));
	}

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
			$wanted_re = str_replace("%e", '\\' . DIRECTORY_SEPARATOR, $wanted_re);
			$wanted_re = str_replace("%s", ".+?", $wanted_re); //not greedy
			$wanted_re = str_replace("%i", "[+\-]?[0-9]+", $wanted_re);
			$wanted_re = str_replace("%d", "[0-9]+", $wanted_re);
			$wanted_re = str_replace("%x", "[0-9a-fA-F]+", $wanted_re);
			$wanted_re = str_replace("%f", "[+\-]?\.?[0-9]+\.?[0-9]*(E-?[0-9]+)?", $wanted_re);
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
			if (isset($old_php)) {
				$php = $old_php;
			}
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
			if (isset($old_php)) {
				$php = $old_php;
			}
			return 'PASSED';
		}
		$wanted_re = NULL;
	}

	// Test failed so we need to report details.
	if ($warn) {
		echo "WARN $tested$info\n";
	} else {
		echo "FAIL $tested$info\n";
	}

	$PHP_FAILED_TESTS[] = array(
						'name' => $file,
						'test_name' => $tested,
						'output' => ereg_replace('\.phpt$','.log', $file),
						'diff'   => ereg_replace('\.phpt$','.diff', $file),
						'info'   => $info
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
		fwrite($log,generate_diff($wanted,$wanted_re,$output));
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

	if (isset($old_php)) {
		$php = $old_php;
	}

	return $warn ? 'WARNED' : 'FAILED';
}

function comp_line($l1,$l2,$is_reg)
{
	if ($is_reg) {
		return preg_match('/^'.$l1.'$/s', $l2);
	} else {
		return !strcmp($l1, $l2);
	}
}

function count_array_diff($ar1,$ar2,$is_reg,$w,$idx1,$idx2,$cnt1,$cnt2,$steps)
{
	$equal = 0;
	while ($idx1 < $cnt1 && $idx2 < $cnt2 && comp_line($ar1[$idx1], $ar2[$idx2], $is_reg)) {
		$idx1++;
		$idx2++;
		$equal++;
		$steps--;
	}
	if (--$steps > 0) {
		$eq1 = 0;
		$st = $steps / 2;
		for ($ofs1 = $idx1+1; $ofs1 < $cnt1 && $st-- > 0; $ofs1++) {
			$eq = count_array_diff($ar1,$ar2,$is_reg,$w,$ofs1,$idx2,$cnt1,$cnt2,$st);
			if ($eq > $eq1) {
				$eq1 = $eq;
			}
		}
		$eq2 = 0;
		$st = $steps;
		for ($ofs2 = $idx2+1; $ofs2 < $cnt2 && $st-- > 0; $ofs2++) {
			$eq = count_array_diff($ar1,$ar2,$is_reg,$w,$idx1,$ofs2,$cnt1,$cnt2,$st);
			if ($eq > $eq2) {
				$eq2 = $eq;
			}
		}
		if ($eq1 > $eq2) {
			$equal += $eq1;
		} else if ($eq2 > 0) {
			$equal += $eq2;
		}
	}
	return $equal;
}

function generate_array_diff($ar1,$ar2,$is_reg,$w)
{
	$idx1 = 0; $ofs1 = 0; $cnt1 = count($ar1);
	$idx2 = 0; $ofs2 = 0; $cnt2 = count($ar2);
	$diff = array();
	$old1 = array();
	$old2 = array();
	
	while ($idx1 < $cnt1 && $idx2 < $cnt2) {
		if (comp_line($ar1[$idx1], $ar2[$idx2], $is_reg)) {
			$idx1++;
			$idx2++;
			continue;
		} else {
			$c1 = count_array_diff($ar1,$ar2,$is_reg,$w,$idx1+1,$idx2,$cnt1,$cnt2,10);
			$c2 = count_array_diff($ar1,$ar2,$is_reg,$w,$idx1,$idx2+1,$cnt1,$cnt2,10);
			if ($c1 > $c2) {
				$old1[$idx1] = sprintf("%03d- ", $idx1+1).$w[$idx1++];
				$last = 1;
			} else if ($c2 > 0) {
				$old2[$idx2] = sprintf("%03d+ ", $idx2+1).$ar2[$idx2++];
				$last = 2;
			} else {
				$old1[$idx1] = sprintf("%03d- ", $idx1+1).$w[$idx1++];
				$old2[$idx2] = sprintf("%03d+ ", $idx2+1).$ar2[$idx2++];
			}
		}
	}
	
	reset($old1); $k1 = key($old1); $l1 = -2;
	reset($old2); $k2 = key($old2); $l2 = -2;  
	while ($k1 !== NULL || $k2 !== NULL) {
		if ($k1 == $l1+1 || $k2 === NULL) {
			$l1 = $k1;
			$diff[] = current($old1);
			$k1 = next($old1) ? key($old1) : NULL;
		} else if ($k2 == $l2+1 || $k1 === NULL) {
			$l2 = $k2;
			$diff[] = current($old2);
			$k2 = next($old2) ? key($old2) : NULL;
		} else if ($k1 < $k2) {
			$l1 = $k1;
			$diff[] = current($old1);
			$k1 = next($old1) ? key($old1) : NULL;
		} else {
			$l2 = $k2;
			$diff[] = current($old2);
			$k2 = next($old2) ? key($old2) : NULL;
		}
	}
	while ($idx1 < $cnt1) {
		$diff[] = sprintf("%03d- ", $idx1+1).$w[$idx1++];
	}
	while ($idx2 < $cnt2) {
		$diff[] = sprintf("%03d+ ", $idx2+1).$ar2[$idx2++];
	}
	return $diff;
}

function generate_diff($wanted,$wanted_re,$output)
{
	$w = explode("\n", $wanted);
	$o = explode("\n", $output);
	$r = is_null($wanted_re) ? $w : explode("\n", $wanted_re);
	$diff = generate_array_diff($r,$o,!is_null($wanted_re),$w);
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
	$settings = '';
	foreach($ini_settings as $name => $value) {
		$value = addslashes($value);
		$settings .= " -d \"$name=$value\"";
	}
	$ini_settings = $settings;
}

function compute_summary()
{
	global $n_total, $test_results, $ignored_by_ext, $sum_results, $percent_results;

	$n_total = count($test_results);
	$n_total += $ignored_by_ext;
	
	$sum_results = array('PASSED'=>0, 'WARNED'=>0, 'SKIPPED'=>0, 'FAILED'=>0, 'BORKED'=>0);
	foreach ($test_results as $v) {
		$sum_results[$v]++;
	}
	$sum_results['SKIPPED'] += $ignored_by_ext;
	$percent_results = array();
	while (list($v,$n) = each($sum_results)) {
		$percent_results[$v] = (100.0 * $n) / $n_total;
	}
}

function get_summary($show_ext_summary)
{
	global $exts_skipped, $exts_tested, $n_total, $sum_results, $percent_results, $end_time, $start_time, $failed_test_summary, $PHP_FAILED_TESTS;

	$x_total = $n_total - $sum_results['SKIPPED'] - $sum_results['BORKED'];
	if ($x_total) {
		$x_warned = (100.0 * $sum_results['WARNED']) / $x_total;
		$x_failed = (100.0 * $sum_results['FAILED']) / $x_total;
		$x_passed = (100.0 * $sum_results['PASSED']) / $x_total;
	} else {
		$x_warned = $x_failed = $x_passed = 0;
	}

	$summary = "";
	if ($show_ext_summary) {
		$summary .= "
=====================================================================
TEST RESULT SUMMARY
---------------------------------------------------------------------
Exts skipped    : " . sprintf("%4d",$exts_skipped) . "
Exts tested     : " . sprintf("%4d",$exts_tested) . "
---------------------------------------------------------------------
";
	}
	$summary .= "
Number of tests : " . sprintf("%4d",$n_total). "          " . sprintf("%8d",$x_total);
	if ($sum_results['BORKED']) {
	$summary .= "
Tests borked    : " . sprintf("%4d (%5.1f%%)",$sum_results['BORKED'],$percent_results['BORKED']) . " --------";
	}
	$summary .= "
Tests skipped   : " . sprintf("%4d (%5.1f%%)",$sum_results['SKIPPED'],$percent_results['SKIPPED']) . " --------
Tests warned    : " . sprintf("%4d (%5.1f%%)",$sum_results['WARNED'],$percent_results['WARNED']) . " " . sprintf("(%5.1f%%)",$x_warned) . "
Tests failed    : " . sprintf("%4d (%5.1f%%)",$sum_results['FAILED'],$percent_results['FAILED']) . " " . sprintf("(%5.1f%%)",$x_failed) . "
Tests passed    : " . sprintf("%4d (%5.1f%%)",$sum_results['PASSED'],$percent_results['PASSED']) . " " . sprintf("(%5.1f%%)",$x_passed) . "
---------------------------------------------------------------------
Time taken      : " . sprintf("%4d seconds", $end_time - $start_time) . "
=====================================================================
";
	$failed_test_summary = '';
	if (count($PHP_FAILED_TESTS)) {
		$failed_test_summary .= "
=====================================================================
FAILED TEST SUMMARY
---------------------------------------------------------------------
";
	foreach ($PHP_FAILED_TESTS as $failed_test_data) {
		$failed_test_summary .=  $failed_test_data['test_name'] . $failed_test_data['info'] . "\n";
	}
	$failed_test_summary .=  "=====================================================================\n";
	}
	
	if ($failed_test_summary && !getenv('NO_PHPTEST_SUMMARY')) {
		$summary .= $failed_test_summary;
	}

	return $summary;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim: noet sw=4 ts=4
 */
?>
