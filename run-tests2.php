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
	var $ini_settings = array (
			'open_basedir'=>'',
			'disable_functions'=>'',
			'error_reporting'=>'2047',
			'display_errors'=>'0',
			'log_errors'=>'0',
			'html_errors'=>'0',
			'track_errors'=>'1',
			'report_memleaks'=>'1',
			'docref_root'=>'/phpmanual/',
			'docref_ext'=>'.html',
			'error_prepend_string'=>'',
			'error_append_string'=>'',
			'auto_append_file'=>'',
			'auto_prepend_file'=>'',
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
	
	function testHarness() {
		$this->parseArgs();
		
		# change to working directory
		if ($this->TEST_PHP_SRCDIR) {
			@chdir($this->TEST_PHP_SRCDIR);
		}
		$this->cwd = getcwd();
		
		$this->isExecutable();
		
		$this->checkSafeMode();
		
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
		$this->initializeConfiguration();
		
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
	
	function initializeConfiguration()
	{
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
	
	//
	// Write test context information.
	//
	function contextHeader()
	{
		$ini='';
		if (function_exists('php_ini_scanned_files')) {
			$ini=php_ini_scanned_files();
		}
		
		$this->writemsg("\n$this->ddash\n".
			"CWD         : {$this->cwd}\n".
			"PHP         : {$this->TEST_PHP_EXECUTABLE}\n".
			"PHP_SAPI    : " . PHP_SAPI . "\n".
			"PHP_VERSION : " . PHP_VERSION . "\n".
			"PHP_OS      : " . PHP_OS . "\n".
			"INI actual  : " . realpath(get_cfg_var('cfg_file_path')) . "\n".
			"More .INIs  : " . str_replace("\n","", $ini) . "\n".
			"Test Dirs   : ");
		foreach ($this->test_dirs as $test_dir) {
			$this->writemsg("{$test_dir}\n              ");
		}
		$this->writemsg("\n$this->ddash\n");
	}
	
	function loadFileList()
	{
		foreach ($this->test_dirs as $dir) {
			#$this->findFilesInDir("{$this->cwd}/{$dir}", ($dir == 'ext'));
			$this->findFilesInDir($dir, ($dir == 'ext'));
		}
		sort($this->test_files);
	}
	
	function findFilesInDir($dir,$is_ext_dir=FALSE,$ignore=FALSE)
	{
		$o = opendir($dir) or $this->error("cannot open directory: $dir");
		while (($name = readdir($o)) !== FALSE) {
			if (is_dir("{$dir}/{$name}") && !in_array($name, array('.', '..', 'CVS'))) {
				$skip_ext = ($is_ext_dir && !in_array($name, $this->exts_to_test));
				if ($skip_ext) {
					$this->exts_skipped++;
				}
				$this->findFilesInDir("{$dir}/{$name}", FALSE, $ignore || $skip_ext);
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
					$testfile = realpath("{$dir}/{$name}");
					$this->test_files[] = $testfile;
				}
			}
		}
		closedir($o);
	}
	
	function runHeader()
	{
		$this->writemsg("TIME START " . date('Y-m-d H:i:s', $this->start_time) . "\n".$this->ddash."\n");
		$this->writemsg("Running selected tests.\n");
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
			$this->writemsg("Some tests have failed, would you like to send the\nreport to PHP's QA team? [Yn]: ");
			$user_input = fgets($fp, 10);
			
			if (strlen(trim($user_input)) == 0 || strtolower($user_input[0]) == 'y') {
				/*  
				 * Collect information about the host system for our report
				 * Fetch phpinfo() output so that we can see the PHP enviroment
				 * Make an archive of all the failed tests
				 * Send an email
				 */
				
				$failed_tests_data = '';
				$sep = "\n" . str_repeat('=', 80) . "\n";
				
				$failed_tests_data .= "OS:\n". PHP_OS. "\n";
				$automake = $autoconf = $libtool = $compiler = 'N/A';
				if (substr(PHP_OS, 0, 3) != "WIN") {
					$automake = shell_exec('automake --version');
					$autoconf = shell_exec('autoconf --version');
					$libtool = shell_exec('libtool --version');
					$compiler = shell_exec(getenv('CC').' -v 2>&1');
				}
				$failed_tests_data .= "Automake:\n$automake\n";
				$failed_tests_data .= "Autoconf:\n$autoconf\n";
				$failed_tests_data .= "Libtool:\n$libtool\n";
				$failed_tests_data .= "Compiler:\n$compiler\n";
				$failed_tests_data .= "Bison:\n". @shell_exec('bison --version'). "\n";
				$failed_tests_data .= "\n\n";
				
				$failed_tests_data .= $failed_test_summary . "\n";
				
				foreach ($this->failed_tests as $test_info) {
					$failed_tests_data .= $sep . $test_info['name'];
					$failed_tests_data .= $sep . $this->getFileContents($test_info['output']);
					$failed_tests_data .= $sep . $this->getFileContents($test_info['diff']);
					$failed_tests_data .= $sep . "\n\n";
				}
				
				$failed_tests_data .= $sep . "PHPINFO" . $sep;
				$failed_tests_data .= shell_exec($this->TEST_PHP_EXECUTABLE.' -dhtml_errors=0 -i');
				
				$compression = 0;
				
				fwrite($fp, "\nThank you for helping to make PHP better.\n");
				fclose($fp);

				if (0 && !$this->mail_qa_team($failed_tests_data, $compression)) {
					$output_file = 'php_test_results_' . date('Ymd') . ( $compression ? '.txt.gz' : '.txt' );
					$fp = fopen($output_file, "w");
					fwrite($fp, $failed_tests_data);
					fclose($fp);
				
					$this->writemsg("\nThe test script was unable to automatically send the report to PHP's QA Team\nPlease send ".$output_file." to ".PHP_QA_EMAIL." manually, thank you.\n");
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

	function getINISettings(&$section_text)
	{
		$ini_settings = $this->ini_settings;
		// Any special ini settings 
		// these may overwrite the test defaults...
		if (array_key_exists('INI', $section_text)) {
			foreach(preg_split( "/[\n\r]+/", $section_text['INI']) as $setting) {
				if (strpos($setting, '=')!==false) {
					$setting = explode("=", $setting,2);
					$name = trim(strtolower($setting[0]));
					$value = trim($setting[1]);
					$ini_settings[$name] = addslashes($value);
				}
			}
		}
		$settings = '';
		foreach($ini_settings as $name => $value) {
			$settings .= " -d \"$name=$value\"";
		}
		return $settings;
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
				$output = `{$this->TEST_PHP_EXECUTABLE} $tmp_skipif`;
				@unlink($tmp_skipif);
				if (ereg("^skip", trim($output))){
				
					$reason = (ereg("^skip[[:space:]]*(.+)\$", trim($output))) ? ereg_replace("^skip[[:space:]]*(.+)\$", "\\1", trim($output)) : FALSE;
					$this->showstatus($section_text['TEST'], 'SKIPPED', $reason);
	
					return 'SKIPPED';
				}
			}
		}
		return NULL;
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
		
		$ini_settings = $this->getINISettings($section_text);
	
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
			save_text($tmp_post,$post);
			$content_length = strlen($post);
	
			$env['REQUEST_METHOD']='POST';
			$env['CONTENT_TYPE']='application/x-www-form-urlencoded';
			$env['CONTENT_LENGTH']=$content_length;
	
			$cmd = "{$this->TEST_PHP_EXECUTABLE}$ini_settings -f $tmp_file 2>&1 < $tmp_post";
	
		} else {
	
			$env['REQUEST_METHOD']='GET';
			$env['CONTENT_TYPE']='';
			$env['CONTENT_LENGTH']='';
	
			$cmd = "{$this->TEST_PHP_EXECUTABLE}$ini_settings -f $tmp_file$args 2>&1";
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
		$this->writemsg("ERROR: {$message}\n");
		exit(1);
	}
}

$test = new webHarness();
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
?>
