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
   | Authors: Ilia Alshanetsky <ilia@php.net>                             |
   |          Preston L. Bannister <pbannister@php.net>                   |
   |          Marcus Boerger <helly@php.net>                              |
   |          Derick Rethans <derick@php.net>                             |
   |          Sander Roobol <sander@php.net>                              |
   |          John Coggeshall <john@php.net>                              |
   | (based on version by: Stig Bakken <ssb@fast.no>)                     |
   | (based on the PHP 3 test framework by Rasmus Lerdorf)                |
   +----------------------------------------------------------------------+
 */
set_time_limit(0);
ob_implicit_flush();
error_reporting(E_ALL);
define('PHP_QA_EMAIL', 'php-qa@lists.php.net');
define('QA_SUBMISSION_PAGE', 'http://qa.php.net/buildtest-process.php');


class testHarness {
	var $cwd;
	var $TEST_PHP_SRCDIR;
	var $TEST_PHP_EXECUTABLE;
	var $TEST_PHP_LOG_FORMAT;
	var $TEST_PHP_DETAILED;
	var $TEST_PHP_ERROR_STYLE;
	var $REPORT_EXIT_STATUS;
	var $NO_PHPTEST_SUMMARY;
	var $NO_INTERACTION;
	
	var $test_to_run = array();
	var $test_files = array();
	var $test_results = array();
	var $failed_tests = array();
	var $exts_to_test;
	var $exts_tested = 0;
	var $exts_skipped = 0;
	var $ignored_by_ext = 0;
	var $test_dirs = array('tests', 'pear', 'ext');
	var $start_time;
	var $end_time;
	
	var $ddash = "=====================================================================";
	var $sdash = "---------------------------------------------------------------------";

	// Default ini settings
	var $ini_overwrites = array(
			'output_handler'=>'',
			'zlib.output_compression'=>'Off',
			'open_basedir'=>'',
			'safe_mode'=>'0',
			'disable_functions'=>'',
			'output_buffering'=>'Off',
			'error_reporting'=>'2047',
			'display_errors'=>'1',
			'log_errors'=>'0',
			'html_errors'=>'0',
			'track_errors'=>'1',
			'report_memleaks'=>'1',
			'report_zend_debug'=>'0',
			'docref_root'=>'/phpmanual/',
			'docref_ext'=>'.html',
			'error_prepend_string'=>'',
			'error_append_string'=>'',
			'auto_prepend_file'=>'',
			'auto_append_file'=>'',
			'magic_quotes_runtime'=>'0',
		);	
	var $env = array(
		'REDIRECT_STATUS'=>'',
		'QUERY_STRING'=>'',
		'PATH_TRANSLATED'=>'',
		'SCRIPT_FILENAME'=>'',
		'REQUEST_METHOD'=>'',
		'CONTENT_TYPE'=>'',
		'CONTENT_LENGTH'=>'',
		);
	var $info_params = array();

	function testHarness() {
		$this->checkPCRE();
		$this->checkSafeMode();
		$this->removeSensitiveEnvVars();
		$this->initializeConfiguration();
		$this->parseArgs();
		# change to working directory
		if ($this->TEST_PHP_SRCDIR) {
			@chdir($this->TEST_PHP_SRCDIR);
		}
		$this->cwd = getcwd();
		$this->isExecutable();
		$this->getInstalledExtensions();
		$this->contextHeader();
		$this->loadFileList();
		$this->run();
		$this->summarizeResults();
	}

	// Use this function to do any displaying of text, so that
	// things can be over-written as necessary.
	
	function writemsg($msg) {
	    
	    echo $msg;
	    
	}
	
	// Another wrapper function, this one should be used any time
	// a particular test passes or fails
	
	function showstatus($item, $status, $reason = '') {
	    
	    switch($status) {
		
		case 'PASSED':
		
		    $this->writemsg("PASSED: $item ($reason)\n");
		    
		    break;
		    
		case 'FAILED':
		    
		    $this->writemsg("FAILED: $item ($reason)\n");
		    
		    break;
		    
		case 'SKIPPED':
		
		    $this->writemsg("SKIPPED: $item ($reason)\n");
		    break;
		    
	    }
	    
	}
	
	function help()
	{
		return "usage: php run-tests.php [options]\n".
				"  -d testpaths            colon seperate path list\n".
				"  -s path                 to php sources\n".
				"  -p path                 to php executable to be tested\n".
				"  -l logformat            LEOD\n".
				"  -t 0|1                  detailed reporting\n".
				"  -e string               error style\n".
				"  -r 0|1                  report exit status\n".
				"  -n 0|1                  print test summary\n".
				"  -i 0|1                  console interaction\n".
				"  -h                      this help\n";
	}
	
	function parseArgs() {
		global $argc;
		global $argv;
		
		if (!isset($argv)) {
			$argv = $_SERVER['argv'];
			$argc = $_SERVER['argc'];
		}
		if (!isset($argc) || $argc < 2) return;
	
		for ($i=1; $i<$argc;) {
			if ($argv[$i][0] != '-') continue;
			$opt = $argv[$i++][1];
			$value = 0;
			if (@$argv[$i][0] != '-') {
				@$value = $argv[$i++];
			}
			switch($opt) {
				case 'd':
					$this->test_dirs = array();
					$paths = split(':|;',$value);
					foreach($paths as $path) {
						$this->test_dirs[] = realpath($path);
					}
					break;
				case 's':
					$this->TEST_PHP_SRCDIR = $value;
					break;
				case 'p':
					$this->TEST_PHP_EXECUTABLE = $value;
					break;
				case 'l':
					$this->TEST_PHP_LOG_FORMAT = $value;
					break;
				case 't':
					$this->TEST_PHP_DETAILED = $value;
					break;
				case 'e':
					$this->TEST_PHP_ERROR_STYLE = strtoupper($value);
					break;
				case 'r':
					$this->REPORT_EXIT_STATUS = $value;
					break;
				case 'n':
					$this->NO_PHPTEST_SUMMARY = $value;
					break;
				case 'i':
					$this->NO_INTERACTION = $value;
					break;
				case 'h':
					print $this->help();
					exit(0);
			}
		}
	}
	
	function removeSensitiveEnvVars()
	{
		# delete sensitive env vars
		putenv('SSH_CLIENT=deleted');
		putenv('SSH_AUTH_SOCK=deleted');
		putenv('SSH_TTY=deleted');
	}
	
	function initializeConfiguration()
	{
		# get config from environment
		$this->TEST_PHP_SRCDIR = getenv('TEST_PHP_SRCDIR');
		$this->TEST_PHP_EXECUTABLE = getenv('TEST_PHP_EXECUTABLE');
		$this->TEST_PHP_LOG_FORMAT = getenv('TEST_PHP_LOG_FORMAT');
		if (!$this->TEST_PHP_LOG_FORMAT) $this->TEST_PHP_LOG_FORMAT = 'LEOD';
		$this->TEST_PHP_DETAILED = getenv('TEST_PHP_DETAILED');
		$this->TEST_PHP_ERROR_STYLE = strtoupper(getenv('TEST_PHP_ERROR_STYLE'));
		$this->REPORT_EXIT_STATUS = getenv('REPORT_EXIT_STATUS');
		$this->NO_PHPTEST_SUMMARY = getenv('NO_PHPTEST_SUMMARY');
		$this->NO_INTERACTION = getenv('NO_INTERACTION');
	}

	function getInstalledExtensions()
	{
		// get the list of installed extensions
		$this->exts_to_test = get_loaded_extensions();
		$this->exts_tested = count($this->exts_to_test);
		sort($this->exts_to_test);
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

	function isExecutable() {
		if (!$this->TEST_PHP_EXECUTABLE ||
			(function_exists('is_executable') &&
			!@is_executable($this->TEST_PHP_EXECUTABLE))) {
			$this->error("invalid PHP executable specified by TEST_PHP_EXECUTABLE  = " .
					$this->TEST_PHP_EXECUTABLE);
			return false;
		}
		return true;
	}
	
	function checkPCRE() {
		if (!extension_loaded("pcre")) {
			echo <<<NO_PCRE_ERROR

+-----------------------------------------------------------+
|                       ! ERROR !                           |
| The test-suite requires that you have pcre extension      |
| enabled. To enable this extension either compile your PHP |
| with --with-pcre-regex or if you have compiled pcre as a  |
| shared module load it via php.ini.                        |
+-----------------------------------------------------------+

NO_PCRE_ERROR;
		exit;
		}
	}
	
	function checkSafeMode() {
		if (ini_get('safe_mode')) {
			$safewarn =  <<< SAFE_MODE_WARNING

+-----------------------------------------------------------+
|                       ! WARNING !                         |
| You are running the test-suite with "safe_mode" ENABLED ! |
|                                                           |
| Chances are high that no test will work at all,           |
| depending on how you configured "safe_mode" !             |
+-----------------------------------------------------------+


SAFE_MODE_WARNING;
			writemsg($safewarn);
			return true;
		}
		return false;
	}
	
	function getExecutableInfo()
	{
		$info_file = realpath(dirname(__FILE__)) . '/run-test-info.php';
		@unlink($info_file);
		$php_info = '<?php echo "
PHP_SAPI    : " . PHP_SAPI . "
PHP_VERSION : " . phpversion() . "
ZEND_VERSION: " . zend_version() . "
PHP_OS      : " . PHP_OS . "
INI actual  : " . realpath(get_cfg_var("cfg_file_path")) . "
More .INIs  : " . (function_exists(\'php_ini_scanned_files\') ? str_replace("\n","", php_ini_scanned_files()) : "** not determined **"); ?>';
		$this->save_text($info_file, $php_info);
		$this->settings2array($this->ini_overwrites,$this->info_params);
		$this->settings2params($this->info_params);
		$php_info = `{$this->TEST_PHP_EXECUTABLE} {$this->info_params} -f $info_file`;
		@unlink($info_file);
		return $php_info;
	}
	
	//
	// Write test context information.
	//
	function contextHeader()
	{
		$ini='';
		if (function_exists('php_ini_scanned_files')) {
			$ini=php_ini_scanned_files();
		}

		$info = $this->getExecutableInfo();
		
		$this->writemsg("\n$this->ddash\n".
			"CWD         : {$this->cwd}\n".
			"PHP         : {$this->TEST_PHP_EXECUTABLE} $info\n".
			"Test Dirs   : ");
		foreach ($this->test_dirs as $test_dir) {
			$this->writemsg("$test_dir\n              ");
		}
		$this->writemsg("\n$this->ddash\n");
	}
	
	function loadFileList()
	{
		foreach ($this->test_dirs as $dir) {
			print "searching {$this->cwd}/{$dir}\n";
			$this->findFilesInDir("{$this->cwd}/$dir", ($dir == 'ext'));
			#$this->findFilesInDir($dir, ($dir == 'ext'));
		}
		usort($this->test_files,array($this,"test_sort"));
		$this->writemsg("found ".count($this->test_files)." files\n");
	}
	
	function findFilesInDir($dir,$is_ext_dir=FALSE,$ignore=FALSE)
	{
		$skip = array('.', '..', 'CVS');
		$o = opendir($dir) or $this->error("cannot open directory: $dir");
		while (($name = readdir($o)) !== FALSE) {
			if (in_array($name, $skip)) continue;
			if (is_dir("$dir/$name")) {
				$skip_ext = ($is_ext_dir && !in_array($name, $this->exts_to_test));
				if ($skip_ext) {
					$this->exts_skipped++;
				}
				$this->findFilesInDir("$dir/$name", FALSE, $ignore || $skip_ext);
			}
	
			// Cleanup any left-over tmp files from last run.
			if (substr($name, -4) == '.tmp') {
				@unlink("$dir/$name");
				continue;
			}
	
			// Otherwise we're only interested in *.phpt files.
			if (substr($name, -5) == '.phpt') {
				if ($ignore) {
					$this->ignored_by_ext++;
				} else {
					$testfile = realpath("$dir/$name");
					$this->test_files[] = $testfile;
				}
			}
		}
		closedir($o);
	}
	
	function runHeader()
	{
		$this->writemsg("TIME START " . date('Y-m-d H:i:s', $this->start_time) . "\n".$this->ddash."\n");
		if (count($this->test_to_run)) {
			$this->writemsg("Running selected tests.\n");
		} else {
			$this->writemsg("Running all test files.\n");
		}
	}
	
	// Probably unnecessary for CLI, but used when overloading a
	// web-based test class
	
	function runFooter()
	{
	    
	}
	
	function run()
	{
		$this->start_time = time();
		$this->runHeader();
		// Run selected tests.
		if (count($this->test_to_run)) {
			
			foreach($this->test_to_run as $name=>$runnable) {
				if(!preg_match("/\.phpt$/", $name))
					continue;
				if ($runnable) {
					$this->test_results[$name] = $this->run_test($name);
				}
			}
		} else {
			foreach ($this->test_files as $name) {
				$this->test_results[$name] = $this->run_test($name);
			}
		}
		$this->end_time = time();
		$this->runFooter();
	}

	function summarizeResults()
	{
		if (count($this->test_results) == 0) {
			$this->writemsg("No tests were run.\n");
			return;
		}
		
		$n_total = count($this->test_results);
		$n_total += $this->ignored_by_ext;
		
		$sum_results = array('PASSED'=>0, 'SKIPPED'=>0, 'FAILED'=>0);
		foreach ($this->test_results as $v) {
			$sum_results[$v]++;
		}
		$sum_results['SKIPPED'] += $this->ignored_by_ext;
		$percent_results = array();
		while (list($v,$n) = each($sum_results)) {
			$percent_results[$v] = (100.0 * $n) / $n_total;
		}
		
		$this->writemsg("\n".$this->ddash."\n".
			"TIME END " . date('Y-m-d H:i:s', $this->end_time) . "\n".
			$this->ddash."\n".
			"TEST RESULT SUMMARY\n".
			$this->sdash."\n".
			"Exts skipped    : " . sprintf("%4d",$this->exts_skipped) . "\n".
			"Exts tested     : " . sprintf("%4d",$this->exts_tested) . "\n".
			$this->sdash."\n".
			"Number of tests : " . sprintf("%4d",$n_total) . "\n".
			"Tests skipped   : " . sprintf("%4d (%2.1f%%)",$sum_results['SKIPPED'],$percent_results['SKIPPED']) . "\n".
			"Tests failed    : " . sprintf("%4d (%2.1f%%)",$sum_results['FAILED'],$percent_results['FAILED']) . "\n".
			"Tests passed    : " . sprintf("%4d (%2.1f%%)",$sum_results['PASSED'],$percent_results['PASSED']) . "\n".
			$this->sdash."\n".
			"Time taken      : " . sprintf("%4d seconds", $this->end_time - $this->start_time) . "\n".
			$this->ddash."\n");
		
		$failed_test_summary = '';
		if ($this->failed_tests) {
			$failed_test_summary .= "\n".$this->ddash."\n".
				"FAILED TEST SUMMARY\n".$this->sdash."\n";
			foreach ($this->failed_tests as $failed_test_data) {
				$failed_test_summary .=  $failed_test_data['test_name'] . "\n";
			}
			$failed_test_summary .=  $this->ddash."\n";
		}
		
		if ($failed_test_summary && !$this->NO_PHPTEST_SUMMARY) {
			$this->writemsg($failed_test_summary);
		}

		/* We got failed Tests, offer the user to send and e-mail to QA team, unless NO_INTERACTION is set */
		if ($sum_results['FAILED'] && !$this->NO_INTERACTION) {
			$fp = fopen("php://stdin", "r+");
			$this->writemsg("\nPlease allow this report to be send to the PHP QA\nteam. This will give us a better understanding in how\n");
			$this->writemsg("PHP's test cases are doing.\n");
			$this->writemsg("(choose \"s\" to just save the results to a file)? [Yns]: ");
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
					$fp = fopen("php://stdin", "r+");
					$user_email = trim(fgets($fp, 1024));
					$user_email = str_replace("@", " at ", str_replace(".", " dot ", $user_email));
				}
		
				$failed_tests_data = '';
				$sep = "\n" . str_repeat('=', 80) . "\n";
				
				$failed_tests_data .= $failed_test_summary . "\n";
				$failed_tests_data .= $summary . "\n";
				
				if (sum($this->failed_tests)) {
					foreach ($this->failed_tests as $test_info) {
						$failed_tests_data .= $sep . $test_info['name'];
						$failed_tests_data .= $sep . $this->getFileContents($test_info['output']);
						$failed_tests_data .= $sep . $this->getFileContents($test_info['diff']);
						$failed_tests_data .= $sep . "\n\n";
					}
					$status = "failed";
				} else {
					$status = "success";
				}
				
				$failed_tests_data .= "\n" . $sep . 'BUILD ENVIRONMENT' . $sep;
				$failed_tests_data .= "OS:\n". PHP_OS. "\n\n";
				$automake = $autoconf = $libtool = $compiler = 'N/A';

				if (substr(PHP_OS, 0, 3) != "WIN") {
					$automake = shell_exec('automake --version');
					$autoconf = shell_exec('autoconf --version');
					/* Always use the generated libtool - Mac OSX uses 'glibtool' */
					$libtool = shell_exec('./libtool --version');
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

				if (isset($user_email)) {
					$failed_tests_data .= "User's E-mail: ".$user_email."\n\n";
				}

				$failed_tests_data .= $sep . "PHPINFO" . $sep;
				$failed_tests_data .= shell_exec($this->TEST_PHP_EXECUTABLE.' -dhtml_errors=0 -i');
				
				$compression = 0;

				if ($just_save_results || !$this->mail_qa_team($failed_tests_data, $compression, $status)) {
					$output_file = 'php_test_results_' . date('Ymd_Hi') . ( $compression ? '.txt.gz' : '.txt' );
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
		 
		if($this->REPORT_EXIT_STATUS == 1 and $sum_results['FAILED']) {
			exit(1);
		}
	}

	function getFileContents($filename)
	{
		$real_filename = realpath($filename);
		if (function_exists('file_get_contents')) {
			return file_get_contents($real_filename);
		}
		
		$fd = fopen($real_filename, "rb");
		if ($fd) {
			$contents = fread($fd, filesize($real_filename));
			fclose($fd);
			return $contents;
		}
		return NULL;
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

	function getINISettings(&$section_text)
	{
		$ini_settings = $this->ini_overwrites;
		// Any special ini settings 
		// these may overwrite the test defaults...
		if (array_key_exists('INI', $section_text)) {
			$this->settings2array(preg_split( "/[\n\r]+/", $section_text['INI']), $ini_settings);
		}
		return $this->settings2params($ini_settings);
	}
	
	//
	// Load the sections of the test file.
	//
	function getSectionText($file)
	{
		// Load the sections of the test file.
		$section_text = array(
			'TEST'   => '(unnamed test)',
			'SKIPIF' => '',
			'GET'    => '',
			'ARGS'   => '',
			'_FILE'   => $file,
			'_DIR'    => realpath(dirname($file)),
		);
	
		$fp = @fopen($file, "r")
				or $this->error("Cannot open test file: $file");
	
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
		return $section_text;
	}
	
	function setEnvironment($env)
	{
		foreach ($env as $name=>$value) {
			putenv("$name=$value");
		}
	}
	
	//
	// Check if test should be skipped.
	//
	function getSkipReason(&$section_text)
	{
		if (array_key_exists('SKIPIF', $section_text)) {
			$tmp_skipif = $section_text['_DIR'] . uniqid('/phpt.');
			@unlink($tmp_skipif);
			if (trim($section_text['SKIPIF'])) {
				$this->save_text($tmp_skipif, $section_text['SKIPIF']);
				$output = `{$this->TEST_PHP_EXECUTABLE} {$this->info_params} $tmp_skipif`;
				@unlink($tmp_skipif);
				if (eregi("^skip", trim($output))){
				
					$reason = (ereg("^skip[[:space:]]*(.+)\$", trim($output))) ? ereg_replace("^skip[[:space:]]*(.+)\$", "\\1", trim($output)) : FALSE;
					$this->showstatus($section_text['TEST'], 'SKIPPED', $reason);
	
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
		return NULL;
	}

	function execute($commandline)
	{
		$data = "";
		
		$proc = proc_open($commandline, array(
                  0 => array('pipe', 'r'),
                  1 => array('pipe', 'w'),
                  2 => array('pipe', 'w')
                  ), $pipes);
		
		if (!$proc)
                return false;
          fclose($pipes[0]);
	
		while (true) {
			/* hide errors from interrupted syscalls */
			$r = $pipes;
			$w = null;
			$e = null;
			$n = @stream_select($r, $w, $e, 60);
	
			if ($n == 0) {
				/* timed out */
				$data .= "\n ** ERROR: process timed out **\n";
				proc_terminate($proc);
				return $data;
			} else if ($n) {
				$line = fread($pipes[1], 8192);
				if (strlen($line) == 0) {
					/* EOF */
					break;
				}
				$data .= $line;
			}
		}
		$code = proc_close($proc);
		return $data;
	}

	//
	//  Run an individual test case.
	//
	function run_test($file)
	{
		if ($this->TEST_PHP_DETAILED)
			$this->writemsg("\n=================\nTEST $file\n");
	
		$section_text = $this->getSectionText($file);
	
		$shortname = str_replace($this->cwd.'/', '', $file);
		$tested = trim($section_text['TEST'])." [$shortname]";
	
		$tmp_file   = ereg_replace('\.phpt$','.php',$file);
		$tmp_post   = $section_text['_DIR'] . uniqid('/phpt.');
	
		@unlink($tmp_file);
		@unlink($tmp_post);
	
		// unlink old test results	
		@unlink(ereg_replace('\.phpt$','.diff',$file));
		@unlink(ereg_replace('\.phpt$','.log',$file));
		@unlink(ereg_replace('\.phpt$','.exp',$file));
		@unlink(ereg_replace('\.phpt$','.out',$file));
	
		// Reset environment from any previous test.
		$env = $this->env;
		$this->setEnvironment($env);

		$skipreason = $this->getSkipReason($section_text);
		if ($skipreason == 'SKIPPED') return $skipreason;
		
		$ini_overwrites = $this->getINISettings($section_text);
	
		// We've satisfied the preconditions - run the test!
		$this->save_text($tmp_file,$section_text['FILE']);
		if (array_key_exists('GET', $section_text)) {
			$query_string = trim($section_text['GET']);
		} else {
			$query_string = '';
		}
	
		$env['REDIRECT_STATUS']='1';
		$env['QUERY_STRING']=$query_string;
		$env['PATH_TRANSLATED']=$tmp_file;
		$env['SCRIPT_FILENAME']=$tmp_file;
	
		$args = $section_text['ARGS'] ? ' -- '.$section_text['ARGS'] : '';
	
		if (array_key_exists('POST', $section_text) && !empty($section_text['POST'])) {
	
			$post = trim($section_text['POST']);
			$this->save_text($tmp_post,$post);
			$content_length = strlen($post);
	
			$env['REQUEST_METHOD']='POST';
			$env['CONTENT_TYPE']='application/x-www-form-urlencoded';
			$env['CONTENT_LENGTH']=$content_length;
	
			$cmd = "{$this->TEST_PHP_EXECUTABLE} $ini_overwrites -f $tmp_file 2>&1 < $tmp_post";
	
		} else {
	
			$env['REQUEST_METHOD']='GET';
			$env['CONTENT_TYPE']='';
			$env['CONTENT_LENGTH']='';
	
			$cmd = "{$this->TEST_PHP_EXECUTABLE} $ini_overwrites -f $tmp_file$args 2>&1";
		}
	
		if ($this->TEST_PHP_DETAILED)
			$this->writemsg("\nCONTENT_LENGTH  = " . $env['CONTENT_LENGTH'] . 
					"\nCONTENT_TYPE    = " . $env['CONTENT_TYPE'] . 
					"\nPATH_TRANSLATED = " . $env['PATH_TRANSLATED'] . 
					"\nQUERY_STRING    = " . $env['QUERY_STRING'] . 
					"\nREDIRECT_STATUS = " . $env['REDIRECT_STATUS'] . 
					"\nREQUEST_METHOD  = " . $env['REQUEST_METHOD'] . 
					"\nSCRIPT_FILENAME = " . $env['SCRIPT_FILENAME'] . 
					"\nCOMMAND $cmd\n");
	
		$this->setEnvironment($env);
		
		$out = $this->execute($cmd);
	
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
			var_dump($wanted);
			print(str_repeat('=', 80) . "\n");
			var_dump($output);
	*/
			if (preg_match("/^$wanted_re\$/s", $output)) {
				@unlink($tmp_file);
				$this->showstatus($tested, 'PASSED');
				return 'PASSED';
			}
	
		} else {
			$wanted = trim($section_text['EXPECT']);
			$wanted = preg_replace('/\r\n/',"\n",$wanted);
		// compare and leave on success
			$ok = (0 == strcmp($output,$wanted));
			if ($ok) {
				@unlink($tmp_file);
				$this->showstatus($tested, 'PASSED');
				return 'PASSED';
			}
		}
	
		// Test failed so we need to report details.
		$this->showstatus($tested, 'FAILED');
	
		$this->failed_tests[] = array(
							'name' => $file,
							'test_name' => $tested,
							'output' => ereg_replace('\.phpt$','.log', $file),
							'diff'   => ereg_replace('\.phpt$','.diff', $file)
							);
	
		// write .exp
		if (strpos($this->TEST_PHP_LOG_FORMAT,'E') !== FALSE) {
			$logname = ereg_replace('\.phpt$','.exp',$file);
			$this->save_text($logname,$wanted);
		}
	
		// write .out
		if (strpos($this->TEST_PHP_LOG_FORMAT,'O') !== FALSE) {
			$logname = ereg_replace('\.phpt$','.out',$file);
			$this->save_text($logname,$output);
		}
	
		// write .diff
		if (strpos($this->TEST_PHP_LOG_FORMAT,'D') !== FALSE) {
			$logname = ereg_replace('\.phpt$','.diff',$file);
			$this->save_text($logname,$this->generate_diff($wanted,$output));
		}
	
		// write .log
		if (strpos($this->TEST_PHP_LOG_FORMAT,'L') !== FALSE) {
			$logname = ereg_replace('\.phpt$','.log',$file);
			$this->save_text($logname,
						"\n---- EXPECTED OUTPUT\n$wanted\n".
						"---- ACTUAL OUTPUT\n$output\n".
						"---- FAILED\n");
			$this->error_report($file,$logname,$tested);
		}
	
		return 'FAILED';
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
	
		$this->writemsg("Posting to {$url_bits['host']} {$url_bits['path']}\n");
		fwrite($fs, "POST ".$url_bits['path']." HTTP/1.1\r\n");
		fwrite($fs, "Host: ".$url_bits['host']."\r\n");
		fwrite($fs, "User-Agent: QA Browser 0.1\r\n");
		fwrite($fs, "Content-Type: application/x-www-form-urlencoded\r\n");
		fwrite($fs, "Content-Length: ".$data_length."\r\n\r\n");
		fwrite($fs, $data);
		fwrite($fs, "\r\n\r\n");
		fclose($fs);
	
		return TRUE;
	} 

	//
	//  Write the given text to a temporary file, and return the filename.
	//
	function save_text($filename,$text)
	{
		$fp = @fopen($filename,'w')
			or $this->error("Cannot open file '" . $filename . "' (save_text)");
		fwrite($fp,$text);
		fclose($fp);
		if (1 < $this->TEST_PHP_DETAILED) {
			$this->writemsg("\nFILE $filename {{{\n$text\n}}}\n");
		}
	}

	//
	//  Write an error in a format recognizable to Emacs or MSVC.
	//
	function error_report($testname,$logname,$tested) 
	{
		$testname = realpath($testname);
		$logname  = realpath($logname);
		switch ($this->TEST_PHP_ERROR_STYLE) {
		default:
		case 'MSVC':
			$this->writemsg($testname . "(1) : $tested\n");
			$this->writemsg($logname . "(1) :  $tested\n");
			break;
		case 'EMACS':
			$this->writemsg($testname . ":1: $tested\n");
			$this->writemsg($logname . ":1:  $tested\n");
			break;
		}
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
		$this->writemsg("ERROR: {$message}\n");
		exit(1);
	}
}

class webHarness extends testHarness {
	
	var $textdata;
	
	function checkSafeMode() {
		if (ini_get('safe_mode')) {
		
?>
<CENTER>
<TABLE CELLPADDING=5 CELLSPACING=0 BORDER=1>
<TR>
<TD BGCOLOR="#FE8C97" ALIGN=CENTER><B>WARNING</B>
<HR NOSHADE COLOR=#000000>
You are running this test-suite with "safe_mode" <B>ENABLED</B>!<BR><BR>
Chances are high that none of the tests will work at all, depending on
how you configured "safe_mode".
</TD>
</TR>
</TABLE>
</CENTER>
<?php
		return true;
	}
	return false;
	}
	
	function runHeader() {
?>
<TABLE CELLPADDING=3 CELLSPACING=0 BORDER=0 STYLE="border: thin solid black;">
<TR>
	<TD>TESTED FUNCTIONALITY</TD>
	<TD>RESULT</TD>
</TR>
<?php

	}

	function runFooter() {


?>
<TR>
<TD COLSPAN=2 ALIGN=CENTER><FONT SIZE=3>Additional Notes</FONT><HR><?php $this->displaymsg(); ?></TD>
</TR>
</TABLE><BR><BR>
<?php 
	}
	
	function error($message)
	{
		$this->writemsg("ERROR: {$message}\n");
		exit(1);
	}
	
	// Use this function to do any displaying of text, so that
	// things can be over-written as necessary.
	
	function writemsg($msg) {
		
		$this->textdata = $this->textdata . $msg;
		
	}
	
	function displaymsg() {
	
?>
<TEXTAREA ROWS=10 COLS=80><?=$this->textdata?></TEXTAREA>
<?php
	}
	
	// Another wrapper function, this one should be used any time
	// a particular test passes or fails
	
	function showstatus($item, $status, $reason = '') 
	{
		static $color = "#FAE998";
		
		$color = ($color == "#FAE998") ? "#FFFFFF" : "#FAE998";
		
		switch($status) {
		
			case 'PASSED':
			
?>
<TR>
<TD BGCOLOR=<?=$color?>><?=$item?></TD>
<TD VALIGN=CENTER ALIGN=CENTER BGCOLOR=<?=$color?> ROWSPAN=2><FONT COLOR=#00FF00>PASSED</FONT></TD>
</TR>
<TR>
<TD BGCOLOR=<?=$color?>>Notes: <?=htmlentities($reason)?></TD>
</TR>
<?php
				
				break;
				
			case 'FAILED':
				
?>
<TR>
<TD BGCOLOR=<?=$color?>><?=$item?></TD>
<TD VALIGN=CENTER ALIGN=CENTER BGCOLOR=<?=$color?> ROWSPAN=2><FONT COLOR=#FF0000>FAILED</FONT></TD>
</TR>
<TR>
<TD BGCOLOR=<?=$color?>>Notes: <?=htmlentities($reason)?></TD>
</TR>
<?php
				
				break;
				
			case 'SKIPPED':
			
?>
<TR>
<TD BGCOLOR=<?=$color?>><?=$item?></TD>
<TD VALIGN=CENTER ALIGN=CENTER BGCOLOR=<?=$color?> ROWSPAN=2><FONT COLOR=#000000>SKIPPED</FONT></TD>
</TR>
<TR>
<TD BGCOLOR=<?=$color?>>Notes: <?=htmlentities($reason)?></TD>
</TR>
<?php
				break;
			
		}
	
	}
}

$test = new testHarness();
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
?>
