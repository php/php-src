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
   | (based on version by: Stig Bakken <ssb@fast.no>)                     |
   | (based on the PHP 3 test framework by Rasmus Lerdorf)                |
   +----------------------------------------------------------------------+
 */

/*
    History
    -------

    2002-05-07  Preston L. Bannister <preston.bannister@cox.net>

    Rewrote and updated to run on Win32.  

    Require exact specification of PHP executable to test (no guessing!).

    Require a specific tests/php.ini (rather than whatever is lying around),
    and test that this is indeed what we are using.

    Die if any internal errors encountered in test script.

    Regularized output for simpler post-processing of output.

    Optionally output error lines indicating the failing test source and log
    for direct jump with MSVC or Emacs.

    Run basic (non-extension) tests first.  Treat PEAR as extension.

    Tested on Windows 2000 with the Cygnus Win32 toolkit installed.
*/

/*
 * TODO:
 * - do not test PEAR components if base class and/or component class cannot be instanciated
 */

set_time_limit(0);
ob_implicit_flush();

if (ini_get('safe_mode')) {
    echo <<<SAFE_MODE_WARNING

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

$php = $_ENV['TEST_PHP_EXECUTABLE'];

isset($php)
    or die("FAIL environment variable TEST_PHP_EXECUTABLE must be set to specify PHP executable!\n");

@is_executable($php)
    or die("FAIL invalid PHP executable specified by TEST_PHP_EXECUTABLE  = " . $php . "\n");

// Check whether a detailed log is wanted.

define('DETAILED',0 + $_ENV['TEST_PHP_DETAILED']);

// Write INI file to be used in tests.

$include_path = getcwd();

$test_ini = realpath(dirname($php) . "/php.ini");
save_text($test_ini,"[PHP]\ninclude_path = \"$include_path\"\n");

// Write test context information.

echo "
=====================================================================
TIME " . date('Y-m-d H:i:s') . " - start of test run
CWD         : " . getcwd() . "
PHP         : $php
PHP_SAPI    : " . PHP_SAPI . "
PHP_VERSION : " . PHP_VERSION . "
PHP_OS      : " . PHP_OS . "
INI actual  : " . get_cfg_var('cfg_file_path') . "
INI wanted  : " . realpath('tests/php.ini') . "
INI tests   : $test_ini
=====================================================================

";

// Make sure we are using the proper php.ini.

$php_ini = realpath("tests/php.ini");
realpath(get_cfg_var('cfg_file_path')) == $php_ini
    or die("FAIL test/php.ini was not used!\n");

// Determine the tests to be run.

$test_to_run = array();
$test_files = array();
$test_results = array();

if (isset($_SERVER['argc'])) {

    // If parameters given assume they represent selected tests to run.

    $argc = $_SERVER['argc'];
    $argv = $_SERVER['argv'];
    for ($i = 1; $i < $argc; $i++) {
        $testfile = realpath($argv[$i]);
        $test_to_run[$testfile] = 1;
    }
    
}

// Compile a list of all test files (*.phpt).

{
    $directories = array();
    $directories[] = getcwd();
    for ($n = 0; $n < count($directories); $n++) {
        $path = $directories[$n];
        $module = '';
        if (ereg('/ext/([^/]+)/',"$path/",$r)) {
            $module = $r[1];
        } else if (ereg('/pear/',"$path/")) {
            $module = 'PEAR';
        }
        $o = opendir($path) or die("Cannot open directory - $path\n");
        while ($name = readdir($o)) {
            if (is_dir("$path/$name")) {
                if ('.' == $name) continue;
                if ('..' == $name) continue;
                if ('CVS' == $name) continue;
                $directories[] = "$path/$name";
                continue;
            }
            
            // Cleanup any left-over tmp files from last run.
            if (ereg('[.]tmp$',$name)) {
                @unlink("$path/$name");
                continue;
            }
            
            // Otherwise we're only interested in *.phpt files.
            if (!ereg('[.]phpt$',$name)) continue;
            //echo "Runnable '" . $name . "' in '" . $path . "'\n";
            $testfile = realpath("$path/$name");
            $test_files[] = $testfile;
            $module_of_test[$testfile] = $module;
        }
        closedir($o);    
    }
}

// Run only selected tests, if specified.

if (count($test_to_run)) {
    echo "Running selected tests.\n";
    while (list($name,$runnable) = each($test_to_run)) {
        echo "test: $name runnable: $runnable\n";
        if (!$runnable) continue;
        $test_results[$name] = run_test($php,$name);
    }
    exit;
}

// We need to know the compiled in modules so we know what to test.

$modules_compiled = @get_loaded_extensions();
$modules_to_test = array(
    ''      => 1,
    'PEAR'  => 1,
);
foreach ($modules_compiled as $module) {
    echo "Will test compiled extension: $module\n";
    $modules_to_test[$module] = 1;
}

echo '
=====================================================================
';

sort($test_files);
$modules_skipped = array();

// Run non-module tests.

$module_current = '';
$path_current = '';
foreach ($test_files as $name) {

    // Only non-module tests wanted for this pass.
    if ($module_of_test[$name]) continue;   
    
    $path = dirname($name);
    if ($path_current != $path) {
        $path_current = $path;
        echo ".... directory $path\n";
    }

    $test_results[$name] = run_test($php,$name);
}

// Run module tests (or at least those applicable to this PHP build).

$module_current = '';
$path_current = '';
foreach ($test_files as $name) {
    $module = $module_of_test[$name];
    
    // Already ran non-module tests.
    if (!$module) continue;     

    if ($module_current != $module) {
        $module_current = $module;
        echo "
---------------------------------------------------------------------
.... " . ($modules_to_test[$module] ? "testing " : "skipped ") . ($module ? "extension: $module" : "generic PHP") . "
";
    }
    
    // Can we run the test for the given module?
    if (!$modules_to_test[$module]) {
        $modules_skipped[$module] += 1;
        $test_results[$name] = 'SKIPPED';
        continue;
    }
    
    $path = dirname($name);
    if ($path_current != $path) {
        $path_current = $path;
        echo ".... directory $path\n";
    }

    // We've gotten this far - run the test!
    
    $test_results[$name] = run_test($php,$name);
}

// Summarize results

if (0 == count($test_results)) {
    echo "No tests were run.\n";
    return;
}

$n_total = count($test_results);
$sum_results = array();
foreach ($test_results as $v) {
    $sum_results[$v]++;
}
$percent_results = array();
while (list($v,$n) = each($sum_results)) {
    $percent_results[$v] = (100.0 * $n) / $n_total;
}

echo "
=====================================================================
TIME " . date('Y-m-d H:i:s') . " - end of test run

TEST RESULT SUMMARY
=====================================================================
Number of tests : " . sprintf("%4d",$n_total) . "
Tests skipped   : " . sprintf("%4d (%2.1f%%)",$sum_results['SKIPPED'],$percent_results['SKIPPED']) . "
Tests failed    : " . sprintf("%4d (%2.1f%%)",$sum_results['FAILED'],$percent_results['FAILED']) . "
Tests passed    : " . sprintf("%4d (%2.1f%%)",$sum_results['PASSED'],$percent_results['PASSED']) . "
=====================================================================
Skipped " . count($modules_skipped) . " extensions
";

//
//  Write the given text to a temporary file, and return the filename.
//

function save_text($filename,$text)
{
    $fp = @fopen($filename,'w')
        or die("Cannot open file '" . $filename . "'!\n");
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
    if (DETAILED) echo "
=================
TEST $file
";

    // Load the sections of the test file.
    
    $section_text = array(
        'TEST'      => '(unnamed test)',
        'SKIPIF'    => '',
        'GET'       => '',
    );

    $fp = @fopen($file, "r")
        or die("Cannot open test file: $file\n");

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

    $tmp = dirname($file);
    $tmp_skipif = realpath("$tmp/_SKIPIF");  
    $tmp_file   = realpath("$tmp/_FILE");  
    $tmp_post   = realpath("$tmp/_POST");  
    
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
    
    // Check if test should be skipped.
    
    if (trim($section_text['SKIPIF'])) {
        save_text($tmp_skipif,$section_text['SKIPIF']);
        $output = `$php -f $tmp_skipif`;
        @unlink($tmp_skipif);
        $output = trim($output);
        if (0 == strcmp('skip',$output)) {
            return 'SKIPPED';
        }
    }
    
    // We've satisfied the preconditions - run the test!
    
    save_text($tmp_file,$section_text['FILE']);
    $query_string = trim($section_text['GET']);

    putenv("REDIRECT_STATUS=1");
    putenv("QUERY_STRING=$query_string");
    putenv("PATH_TRANSLATED=$tmp_file");
    putenv("SCRIPT_FILENAME=$tmp_file");

    if (isset($section_text['POST'])) {
    
        $post = trim($section_text['POST']);
        save_text($tmp_post,$post);
        $content_length = strlen($post);
        
        putenv("REQUEST_METHOD=POST");
        putenv("CONTENT_TYPE=application/x-www-form-urlencoded");
        putenv("CONTENT_LENGTH=$content_length");
        
        $cmd = "$php 2>&1 < $tmp_post";
        
    } else {
    
        putenv("REQUEST_METHOD=GET");
        putenv("CONTENT_TYPE=");
        putenv("CONTENT_LENGTH=");

        $cmd = "$php 2>&1";
        
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

    $tested = trim($section_text['TEST']);
    
    $output = trim(preg_replace('/^(..+\n)+\n/','',$out));
    $wanted = trim($section_text['EXPECT']);
    
    $output = preg_replace('/\r\n/',"\n",$output);
    $wanted = preg_replace('/\r\n/',"\n",$wanted);

    $ok = (0 == strcmp($output,$wanted));
    if ($ok) {
        echo "PASS $tested\n";
        return 'PASSED';
    }
    
    // Test failed so we need to report details.

    echo "FAIL $tested (" . basename($file) . ").\n";

    $logname = ereg_replace('\.phpt$','.log',$file);
    $log = fopen($logname,'w')
        or die("Cannot create test log - $logname\n");
    
    fwrite($log,"
---- EXPECTED OUTPUT
$wanted
---- ACTUAL OUTPUT
$output
---- FAILED
");
    fclose($log);

    error_report($file,$logname,$tested);
    
    return 'FAILED';
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
?>
