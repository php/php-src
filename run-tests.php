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
   | Author: Stig Bakken <ssb@fast.no>                                    |
   | (based on the PHP 3 test framework by Rasmus Lerdorf)                |
   +----------------------------------------------------------------------+
 */

/*
 * TODO:
 * - look for test-specific php.ini files
 * - implement module skipping for PEAR
 * - do not test PEAR components if base class and/or component class cannot be instanciated
 */

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


set_time_limit(0);

ob_implicit_flush();

define('TEST_PASSED', 0);
define('TEST_FAILED', -1);
define('TEST_SKIPPED', -2);
define('TEST_INTERNAL_ERROR', -3);

define('EXT_DIR_NAME','/php4/ext/');
define('EXT_DIR_NAME_LEN',strlen(EXT_DIR_NAME));

initialize();
/*
$opts = parse_options(&$argc, &$argv);
if ($opts['help']) {
    usage();
    exit;
}
*/

if(!isset($_SERVER['argc'])) {
	echo "\nWARNING: register_argc_argv seems to be set 'off' in php.ini\n\n";
}	

@do_testing($_SERVER['argc'], $_SERVER['argv']);

exit;

/*****************************************************************************/

function usage()
{
    dowriteln("Usage: run-tests.php [-d] [-h] [dir|file...]");
}

/*
 * Please use dowrite() and dowriteln() for all screen output.
 * This makes it easier to convert to HTML output later.
 */

function dowrite($str)
{
    global $term_bold, $term_norm;
    $str = str_replace("%b", $term_bold, $str);
    $str = str_replace("%B", $term_norm, $str);
    print $str;
}

function dowriteln($str)
{
    dowrite("$str\n");
}

function create_compiled_in_modules_list()  {
    global $compiled_in_modules;
    $compiled_in_modules = @get_loaded_extensions();
}

function extract_module_name_from_path($path)   {
    if ($pos1=strpos($path,EXT_DIR_NAME)) {
		$pos3=strpos($path,'/',$pos1+EXT_DIR_NAME_LEN);
		return substr($path,$pos2=$pos1+EXT_DIR_NAME_LEN,$pos3-$pos2);
    }
}


function create_found_tests_4_modules_list() {
	global $modules_2_test,$testdirs;

	foreach ($testdirs AS $path) {
		if ($mod_name=extract_module_name_from_path($path))
			$modules_2_test[$mod_name]=TRUE;
	}
}

function create_modules_2_test_list() {
    global $compiled_in_modules,$modules_2_test,$modules_available;
    foreach ($compiled_in_modules AS $value)
	if (isset($modules_2_test[$value])) $modules_available[]=$value;
}



function in_path($program, $windows_p)
{
    global $HTTP_ENV_VARS;
    if ($windows_p) {
        $delim = ";";
        $ext = ".exe";
    } else {
        $delim = ":";
        $ext = "";
    }
    $slash = DIRECTORY_SEPARATOR;
    $path_components = explode($delim, $HTTP_ENV_VARS['PATH']);
    if ($windows_p) {
		$cwd = getcwd() . DIRECTORY_SEPARATOR;
		array_unshift($path_components, "{$cwd}Release_TS_Inline", "{$cwd}Release_TS");
    }
    foreach ($path_components as $path) {
        $test = "{$path}{$slash}php{$ext}";
        if (@is_executable($test)) {
            return $test;
        }
    }
    return false;
}


function initialize()
{
    global $term, $windows_p, $php, $skip, $testdirs, $tmpfile,
	$skipped, $failed, $passed, $total, $term_bold, $term_norm,
	$tests_in_dir;

    // XXX Should support HTML output as well.
    $php = "";
    if((substr(PHP_OS, 0, 3) == "WIN")) {
        $ext = ".exe";
        $windows_p = true;
        $term = getenv("COMSPEC");
    } else {
        $ext = "";
        $term = getenv("TERM");
        if (ereg('^(xterm|vt220)', $term)) {
            $term_bold = sprintf("%c%c%c%c", 27, 91, 49, 109);
            $term_norm = sprintf("%c%c%c", 27, 91, 109);
        } elseif (ereg('^vt100', $term)) {
            $term_bold = sprintf("%c%c%c%c", 27, 91, 49, 109);
            $term_norm = sprintf("%c%c%c", 27, 91, 109);
        } else {
            $term_bold = $term_norm = "";
        }
    }

/*  We need to check CGI SAPI first since some tests must be skipped when
    CLI SAPI is used. CLI SAPI is built always while CGI SAPI is not.
    - yohgaki@php.net */

	if (isset($_ENV["TOP_BUILDDIR"]) && @is_executable($_ENV["TOP_BUILDDIR"]."/php{$ext}")) {
        $php = $_ENV["TOP_BUILDDIR"]."/php{$ext}";
    } elseif (@is_executable("./php{$ext}")) {
        $php = getcwd() . "/php{$ext}";
	} elseif (isset($_ENV["TOP_BUILDDIR"]) && @is_executable($_ENV["TOP_BUILDDIR"]."/sapi/cli/php{$ext}")) {
        $php = $_ENV["TOP_BUILDDIR"]."/sapi/cli/php{$ext}";
    } elseif (@is_executable("./sapi/cli/php{$ext}")) {
        $php = getcwd() . "/sapi/cli/php{$ext}";
    } elseif (@is_executable(PHP_BINDIR . "/php{$ext}")) {
        $php = PHP_BINDIR . "/php{$ext}";
    }
// Test result can be bogus, if we use php binary in path. - yohgaki@php.net
//     if (empty($php)) {
//         $php = in_path("php", $windows_p);
//     }
    if (empty($php)) {
        dowriteln("Unable to find PHP executable (php{$ext}).");
        exit;
    }
    if ($windows_p) {
		// modify path to help Windows find DLL files
		$path = dirname($php) . ";" . getenv("PATH");
		putenv("PATH={$path}");
    }

    create_compiled_in_modules_list();

    $skip = array(
			"CVS" => 1
			);
    $testdirs = array();
    $tmpfile = array();
    $tests_in_dir = array();

    register_shutdown_function("delete_tmpfiles");

    $skipped = $failed = $passed = $total = 0;
}

function &parse_options(&$argc, &$argv)
{
    $options = array();
    while ($argc > 0 && ($opt = substr($argv[0], 0, 2)) == "--") {
        $opt = array_shift($argv);
        $argc--;
        if ($arg == "--") {
            return $options;
        }
        if (ereg('^--([^=]+)=(.*)$', $opt, $matches)) {
            $opt = $matches[1];
            $arg = $matches[2];
        } else {
            $arg = true;
        }
        $options[$opt] = $arg;
    }
    return $options;
}

function do_testing($argc, &$argv)
{
    global $term, $windows_p, $php, $skip, $testdirs, $tmpfile, $opts,
	$skipped, $failed, $passed, $total, $term_bold, $term_norm, $skipped_extensions;

    if ($argc > 1) {
        if (is_dir($argv[1])) {
            $dir = $argv[1];
        } else {
            for ($i = 1; $i < $argc; $i++) {
                switch (run_test($argv[$i])) {
                    case TEST_SKIPPED:
                    case TEST_INTERNAL_ERROR:
                        $skipped++;
					break;
                    case TEST_FAILED:
                        $failed++;
					break;
                    case TEST_PASSED:
                        $passed++;
					break;
                }
                $total++;
            }
        }
    } else {
        // $dir = $_ENV["TOP_SRCDIR"]; // XXX ??? where should this variable be set?
        $dir=str_replace('\\','/',trim(($windows_p ? getenv("TEST_DIR"):`pwd`)));
    }
    if (isset($dir) && $dir) {
        find_testdirs($dir);

        create_found_tests_4_modules_list();
        create_modules_2_test_list();

        for ($i = 0; $i < sizeof($testdirs); $i++) {
            run_tests_in_dir($testdirs[$i]);
        }
    }

    $counting = $total - $skipped;

    if ($counting <= 0) {
        dowriteln("No tests were run.");
        return;
    }

    $total_d = (double)$total;
    $counting_d = (double)$counting;
    $passed_p  = 100 * ($passed / $counting_d);
    $failed_p  = 100 * ($failed / $counting_d);
    $skipped_p = 100 * ($skipped / $total_d);
    $passed_pstr = sprintf($passed_p < 10.0 ? "%1.1f" : "%3.0f", $passed_p);
    $failed_pstr = sprintf($failed_p < 10.0 ? "%1.1f" : "%3.0f", $failed_p);
    $skipped_pstr = sprintf($skipped_p < 10.0 ? "%1.1f" : "%3.0f", $skipped_p);

    dowriteln("TEST RESULT SUMMARY");
    dowriteln("=============================");
    dowriteln(sprintf("Number of tests:  %4d", $total));
    dowriteln(sprintf("Tests skipped:    %4d (%s%%)", $skipped, $skipped_pstr));
    dowriteln(sprintf("Tests failed:     %4d (%s%%)", $failed, $failed_pstr));
    dowriteln(sprintf("Tests passed:     %4d (%s%%)", $passed, $passed_pstr));
    dowriteln("=============================");
    dowriteln("Skipped ".sizeof($skipped_extensions)." extensions.");
 	$php_bin_info_cmd = "$php -q -f ".$_ENV["TOP_BUILDDIR"]."/tests/bin-info.inc";
 	system($php_bin_info_cmd);
}

function find_testdirs($dir = '.', $first_pass = true)
{
    global $testdirs, $skip;

    if ($first_pass && is_dir($dir)) {
        $testdirs[] = $dir;
    }
    $dp = @opendir($dir);
    if (!$dp) {
        print "Warning: could not open directory $dir\n";
        return false;
    }
    while ($ent = readdir($dp)) {
        $path = "$dir/$ent";

        if ((isset($skip[$ent]) && $skip[$ent])
            || substr($ent, 0, 1) == "."
            || !is_dir($path)

            ) {
            continue;
		}

        if (strstr("/$path/", "/tests/")) {
            $testdirs[] = $path;
        }
        find_testdirs($path, false);
    }
    closedir($dp);

}

function run_tests_in_dir($dir = '.')
{
    global $skip, $skipped, $failed, $passed, $total, $opts, $tests_in_dir,$modules_available,$skipped_extensions;
    $dp = opendir($dir);
    if (!$dp) {
        print "Warning: could not run tests in $dir\n";
        return false;
    }
    $testfiles = array();
    while ($ent = readdir($dp)) {
        if ((isset($skip[$ent]) && $skip[$ent]) || substr($ent, 0, 1) == "." || substr($ent, -5) != ".phpt") {
            continue;
        }
        $testfiles[] = "$dir/$ent";
        if(isset($tests_in_dir[$dir]))  $tests_in_dir[$dir]++; else $tests_in_dir[$dir]=1;
    }
    closedir($dp);
    if (isset($tests_in_dir[$dir]) && ($tests_in_dir[$dir] == 0)) {
        return true;
    }
    $oskipped = $skipped;
    if (sizeof($testfiles) == 0) {
        return;
    }

    if ($mod_name=extract_module_name_from_path($dir))   {
        if ($ext_found=in_array($mod_name,$modules_available))
			dowriteln("Testing extension: $mod_name");
        else $skipped_extensions[$mod_name]=TRUE;
    }

    if (!isset($ext_found) or $ext_found!==FALSE) {
        dowriteln("%bRunning tests in $dir%B");
        dowriteln("=================".str_repeat("=", strlen($dir)));
        sort($testfiles);
        for ($i = 0; $i < sizeof($testfiles); $i++) {
            switch (run_test($testfiles[$i])) {
                case TEST_SKIPPED:
                case TEST_INTERNAL_ERROR:
                    $skipped++;
				break;
                case TEST_FAILED:
                    $failed++;
				break;
                case TEST_PASSED:
                    $passed++;
				break;
            }
            $total++;
        }
        if ($oskipped + (isset($tests_in_dir[$dir])?$tests_in_dir[$dir]:0)  == $skipped) {
            $skippednow = $skipped - $oskipped;
            dowriteln("[all $skippednow test(s) skipped]");
        }
		dowriteln("");
    }

    return true;
}

function skip_headers($fp)
{
    // "cli" version of PHP does not output headers
    if (php_sapi_name() == "cli") {
		return;
    }
    while (!feof($fp)) {
        if (trim(fgets($fp, 1024)) == "") {
            break;
        }
    }
}

function delete_tmpfiles()
{
    global $tmpfile;
    reset($tmpfile);
    while (list($k, $v) = each($tmpfile)) {
        if (file_exists($v)) {
			//print "unlink($v): "; var_dump(unlink($v));
            unlink($v);
        }
    }
}

/**
 * Compares two files, ignoring blank lines.
 *
 * @param $file1 string name of first file to compare
 * @param $file2 string name of second file to compare
 *
 * @return bool whether the files were "equal"
 */
function compare_results($file1, $file2)
{
	$data1 = $data2 = "";
    if (!($fp1 = @fopen($file1, "r")) || !($fp2 = @fopen($file2, "r"))) {
        return false;
    }

    while (!(feof($fp1) || feof($fp2))) {
        if (!feof($fp1) && trim($line1 = fgets($fp1, 10240)) != "") {
            //print "adding line1 $line1\n";

            $data1 .= trim($line1);
        }
        if (!feof($fp2) && trim($line2 = fgets($fp2, 10240)) != "") {
            //print "adding line2 $line2\n";

            $data2 .= trim($line2);
        }
    }
    fclose($fp1);
    fclose($fp2);
    if ((trim($data1) != trim($data2))
        || ($data1=='' && $data2=='')) {
		//print "data1=";var_dump($data1);
		//print "data2=";var_dump($data2);
        return false;
    }
    return true;
}

function run_test($file)
{
    global $php, $tmpfile, $term_bold, $term_norm, $term, $windows_p;

    $variables = array("TEST", "POST", "GET", "FILE", "EXPECT", "SKIPIF",
                       "OUTPUT");
    $fp = @fopen($file, "r");
    if (!$fp) {
        return TEST_INTERNAL_ERROR;
    }
    $tmpdir = dirname($file);
    $tmpfix = "phpt.";
    $tmpfile["FILE"] = tempnam($tmpdir, $tmpfix);
    $tmpfile["SKIPIF"] = tempnam($tmpdir, $tmpfix);
    $tmpfile["POST"] = tempnam($tmpdir, $tmpfix);

    $tmpfile["EXPECT"] = tempnam($tmpdir, $tmpfix);
    $tmpfile["OUTPUT"] = tempnam($tmpdir, $tmpfix);


    while ($line = fgets($fp, 4096)) {
        if (ereg('^--([A-Z]+)--', $line, $matches)) {
            $var = $matches[1];
            if (isset($tmpfile[$var]) && $tmpfile[$var]) {
                $fps[$var] = @fopen($tmpfile[$var], "w");
            } else {
                $$var = '';
            }
        } else {
            if (isset($var) && $var) {
                if ($var == "POST") {
                    $line = trim($line);
                }
                if (isset($fps[$var]) && $fps[$var]) {
                    fwrite($fps[$var], $line);
                } else {
                    $$var .= $line;
                }
            }
        }
    }
    if(isset($fps) && is_array($fps)) {
        reset($fps);
        while (list($k, $v) = each($fps)) {
            if (is_resource($v)) {
                fclose($v);
            }
        }
    }
    putenv("PHP_TEST=1");
    putenv("REDIRECT_STATUS=1");
    putenv("CONTENT_LENGTH=");
    putenv("QUERY_STRING=".(isset($GET)?$GET:""));
    $include_path = ini_get("include_path");
    if (isset($fps["SKIPIF"])) {
        $tmpfile["SKIPIF_OUTPUT"] = tempnam($tmpdir, $tmpfix);
        putenv("REQUEST_METHOD=GET");
        putenv("CONTENT_TYPE=");
        putenv("PATH_TRANSLATED=$tmpfile[SKIPIF]");
        putenv("SCRIPT_FILENAME=$tmpfile[SKIPIF]");
        $skipifcmd = "$php -q -f $tmpfile[SKIPIF] > $tmpfile[SKIPIF_OUTPUT]";
        system($skipifcmd, $ret);
        $sp = @fopen($tmpfile["SKIPIF_OUTPUT"], "r");
        if ($sp) {
            skip_headers($sp);
            $skip = trim(fgets($sp, 1024));
            fclose($sp);
            if ($skip == "skip") {
                delete_tmpfiles();
				return TEST_SKIPPED;
			}
		}
	}
	putenv("PATH_TRANSLATED=$tmpfile[FILE]");
	putenv("SCRIPT_FILENAME=$tmpfile[FILE]");
	if (isset($fps["POST"])) {
		putenv("REQUEST_METHOD=POST");
		putenv("CONTENT_TYPE=application/x-www-form-urlencoded");
		putenv("CONTENT_LENGTH=".filesize($tmpfile["POST"]));
	} else {
		putenv("REQUEST_METHOD=GET");
		putenv("CONTENT_TYPE=");
		putenv("CONTENT_LENGTH=");
	}
	if (isset($fps["POST"])) {
		if(!$windows_p) {
			$cmd = "2>&1 $php -q $tmpfile[FILE] < $tmpfile[POST]";
		}
		else {
			$cmd = "$term /c " . realpath($php) ." -q $tmpfile[FILE] < $tmpfile[POST]";
		}
	} else {
		if(!$windows_p) {
			$cmd = "2>&1 $php -q $tmpfile[FILE]";
		}
		else {
			$cmd = "$term /c " . realpath($php) ." -q $tmpfile[FILE]";;
		}
	}
	$ofp = @fopen($tmpfile["OUTPUT"], "w");
	if (!$ofp) {
		dowriteln("Error: could not write to output file");
		delete_tmpfiles();
		return TEST_INTERNAL_ERROR;
	}
	$cp = popen($cmd, "r");
	if (!$cp) {
		dowriteln("Error: could not execute: $cmd");
		delete_tmpfiles();
		return TEST_INTERNAL_ERROR;
	}
	skip_headers($cp);
	while ($data = fread($cp, 2048)) {
		fwrite($ofp, $data);
	}
	fclose($ofp);
	pclose($cp);
	$desc = isset($TEST)?trim($TEST):"";
	$outfile = ereg_replace('\.phpt$', '.out', $file);
	$expectfile = ereg_replace('\.phpt$', '.exp', $file);
	$phpfile = ereg_replace('\.phpt$', '.php', $file);
	if (compare_results($tmpfile["OUTPUT"], $tmpfile["EXPECT"])) {
		$status = TEST_PASSED;
		$text = "passed";
		$pre = $post = "";
		if (file_exists($outfile)) {
			unlink($outfile);
		}
		if (file_exists($expectfile)) {
			unlink($expectfile);
		}
		if (file_exists($phpfile)) {
			unlink($phpfile);
		}
	} else {
		//system("env");
		$status = TEST_FAILED;
		$text = "failed";
		$pre = $term_bold;
		$post = $term_norm;
		$desc .= " (".basename($file).")";
		if (file_exists($outfile)) {
			unlink($outfile);
		}
		copy($tmpfile["OUTPUT"], $outfile);
		copy($tmpfile["EXPECT"], $expectfile);
		copy($tmpfile["FILE"], $phpfile);
	}
	dowriteln(sprintf("%s%-68s ... %s%s", $pre, substr($desc, 0, 68),
					  $text, $post));
//	  if ($status == TEST_FAILED) {
//		  for ($i = 0; $i < sizeof($variables); $i++) {
//			  $var = $variables[$i];
//			  print "$var:\n";
//			  if ($tmpfile[$var]) {
//				  if (file_exists($tmpfile[$var])) {
//					  system("cat ".$tmpfile[$var]);
//				  }
//			  } else {
//				  print $$var;
//			  }
//		  }
//		  print "--\n\n";
//	  }
	delete_tmpfiles();
	return $status;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
?>
