<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 5                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2004 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 3.0 of the PHP license,       |
// | that is bundled with this package in the file LICENSE, and is        |
// | available through the world-wide-web at the following url:           |
// | http://www.php.net/license/3_0.txt.                                  |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Tomas V.V.Cox <cox@idecnet.com>                             |
// |          Greg Beaver <cellog@php.net>                                |
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$
//

/**
 * Simplified version of PHP's test suite
 * -- EXPERIMENTAL --

 Try it with:

 $ php -r 'include "../PEAR/RunTest.php"; $t=new PEAR_RunTest; $o=$t->run("./pear_system.phpt");print_r($o);'


TODO:

Actually finish the development and testing

 */

require_once 'PEAR.php';
require_once 'PEAR/Config.php';

define('DETAILED', 1);
putenv("PHP_PEAR_RUNTESTS=1");

class PEAR_RunTest
{
    var $_logger;

    /**
     * An object that supports the PEAR_Common->log() signature, or null
     * @param PEAR_Common|null
     */
    function PEAR_RunTest($logger = null)
    {
        $this->_logger = $logger;
    }

    //
    //  Run an individual test case.
    //

    function run($file, $ini_settings = '')
    {
        $cwd = getcwd();
        $conf = &PEAR_Config::singleton();
        $php = $conf->get('php_bin');
        //var_dump($php);exit;
        global $log_format, $info_params, $ini_overwrites;

        $info_params = '';
        $log_format = 'LEOD';

        // Load the sections of the test file.
        $section_text = array(
            'TEST'    => '(unnamed test)',
            'SKIPIF'  => '',
            'GET'     => '',
            'ARGS'    => '',
        );

        if (!is_file($file) || !$fp = fopen($file, "r")) {
            return PEAR::raiseError("Cannot open test file: $file");
        }

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

        $shortname = str_replace($cwd.'/', '', $file);
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

        // Check if test should be skipped.
        $info = '';
        $warn = false;
        if (array_key_exists('SKIPIF', $section_text)) {
            if (trim($section_text['SKIPIF'])) {
                $this->save_text($tmp_skipif, $section_text['SKIPIF']);
                //$extra = substr(PHP_OS, 0, 3) !== "WIN" ?
                //    "unset REQUEST_METHOD;": "";

                //$output = `$extra $php $info_params -f $tmp_skipif`;
                $output = `$php $info_params -f $tmp_skipif`;
                unlink($tmp_skipif);
                if (eregi("^skip", trim($output))) {
                    $skipreason = "SKIP $tested";
                    $reason = (eregi("^skip[[:space:]]*(.+)\$", trim($output))) ? eregi_replace("^skip[[:space:]]*(.+)\$", "\\1", trim($output)) : FALSE;
                    if ($reason) {
                        $skipreason .= " (reason: $reason)";
                    }
                    $this->_logger->log(0, $skipreason);
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

        // We've satisfied the preconditions - run the test!
        $this->save_text($tmp_file,$section_text['FILE']);

        $args = $section_text['ARGS'] ? ' -- '.$section_text['ARGS'] : '';

        $cmd = "$php$ini_settings -f $tmp_file$args 2>&1";
        if (isset($this->_logger)) {
            $this->_logger->log(2, 'Running command "' . $cmd . '"');
        }

        $savedir = getcwd(); // in case the test moves us around
        if (isset($section_text['RETURNS'])) {
            ob_start();
            system($cmd, $return_value);
            $out = ob_get_contents();
            ob_end_clean();
            @unlink($tmp_post);
            $section_text['RETURNS'] = (int) trim($section_text['RETURNS']);
            $returnfail = ($return_value != $section_text['RETURNS']);
        } else {
            $out = `$cmd`;
            $returnfail = false;
        }
        chdir($savedir);
        // Does the output match what is expected?
        $output = trim($out);
        $output = preg_replace('/\r\n/', "\n", $output);

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
                $wanted_re = str_replace("%f", "[+\-]?\.?[0-9]+\.?[0-9]*(E-?[0-9]+)?", $wanted_re);
                $wanted_re = str_replace("%c", ".", $wanted_re);
                // %f allows two points "-.0.0" but that is the best *simple* expression
            }
    /* DEBUG YOUR REGEX HERE
            var_dump($wanted_re);
            print(str_repeat('=', 80) . "\n");
            var_dump($output);
    */
            if (!$returnfail && preg_match("/^$wanted_re\$/s", $output)) {
                @unlink($tmp_file);
                $this->_logger->log(0, "PASS $tested$info");
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
            if (!$returnfail && $ok) {
                @unlink($tmp_file);
                $this->_logger->log(0, "PASS $tested$info");
                if (isset($old_php)) {
                    $php = $old_php;
                }
                return 'PASSED';
            }
        }

        // Test failed so we need to report details.
        if ($warn) {
            $this->_logger->log(0, "WARN $tested$info");
        } else {
            $this->_logger->log(0, "FAIL $tested$info");
        }

        if (isset($section_text['RETURNS'])) {
            $GLOBALS['__PHP_FAILED_TESTS__'][] = array(
                            'name' => $file,
                            'test_name' => $tested,
                            'output' => ereg_replace('\.phpt$','.log', $file),
                            'diff'   => ereg_replace('\.phpt$','.diff', $file),
                            'info'   => $info,
                            'return' => $return_value
                            );
        } else {
            $GLOBALS['__PHP_FAILED_TESTS__'][] = array(
                            'name' => $file,
                            'test_name' => $tested,
                            'output' => ereg_replace('\.phpt$','.log', $file),
                            'diff'   => ereg_replace('\.phpt$','.diff', $file),
                            'info'   => $info,
                            );
        }

        // write .exp
        if (strpos($log_format,'E') !== FALSE) {
            $logname = ereg_replace('\.phpt$','.exp',$file);
            if (!$log = fopen($logname,'w')) {
                return PEAR::raiseError("Cannot create test log - $logname");
            }
            fwrite($log,$wanted);
            fclose($log);
        }

        // write .out
        if (strpos($log_format,'O') !== FALSE) {
            $logname = ereg_replace('\.phpt$','.out',$file);
            if (!$log = fopen($logname,'w')) {
                return PEAR::raiseError("Cannot create test log - $logname");
            }
            fwrite($log,$output);
            fclose($log);
        }

        // write .diff
        if (strpos($log_format,'D') !== FALSE) {
            $logname = ereg_replace('\.phpt$','.diff',$file);
            if (!$log = fopen($logname,'w')) {
                return PEAR::raiseError("Cannot create test log - $logname");
            }
            fwrite($log, $this->generate_diff($wanted, $output,
                isset($section_text['RETURNS']) ? array(trim($section_text['RETURNS']),
                    $return_value) : null));
            fclose($log);
        }

        // write .log
        if (strpos($log_format,'L') !== FALSE) {
            $logname = ereg_replace('\.phpt$','.log',$file);
            if (!$log = fopen($logname,'w')) {
                return PEAR::raiseError("Cannot create test log - $logname");
            }
            fwrite($log,"
---- EXPECTED OUTPUT
$wanted
---- ACTUAL OUTPUT
$output
---- FAILED
");
            if ($returnfail) {
                fwrite($log,"
---- EXPECTED RETURN
$section_text[RETURNS]
---- ACTUAL RETURN
$return_value
");
            }
            fclose($log);
            //error_report($file,$logname,$tested);
        }

        if (isset($old_php)) {
            $php = $old_php;
        }

        return $warn ? 'WARNED' : 'FAILED';
    }

    function generate_diff($wanted, $output, $return_value)
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
        if ($return_value) {
            $extra = "##EXPECTED: $return_value[0]\r\n##RETURNED: $return_value[1]";
        } else {
            $extra = '';
        }
        return implode("\r\n", $diff) . $extra;
    }

    //
    //  Write the given text to a temporary file, and return the filename.
    //

    function save_text($filename, $text)
    {
        if (!$fp = fopen($filename, 'w')) {
            return PEAR::raiseError("Cannot open file '" . $filename . "' (save_text)");
        }
        fwrite($fp,$text);
        fclose($fp);
    if (1 < DETAILED) echo "
FILE $filename {{{
$text
}}}
";
    }

}
?>