<?php
/**
 * PEAR_Command_Test (run-tests)
 *
 * PHP versions 4 and 5
 *
 * LICENSE: This source file is subject to version 3.0 of the PHP license
 * that is available through the world-wide-web at the following URI:
 * http://www.php.net/license/3_0.txt.  If you did not receive a copy of
 * the PHP License and are unable to obtain it through the web, please
 * send a note to license@php.net so we can mail you a copy immediately.
 *
 * @category   pear
 * @package    PEAR
 * @author     Stig Bakken <ssb@php.net>
 * @author     Martin Jansen <mj@php.net>
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    CVS: $Id$
 * @link       http://pear.php.net/package/PEAR
 * @since      File available since Release 0.1
 */

/**
 * base class
 */
require_once 'PEAR/Command/Common.php';

/**
 * PEAR commands for login/logout
 *
 * @category   pear
 * @package    PEAR
 * @author     Stig Bakken <ssb@php.net>
 * @author     Martin Jansen <mj@php.net>
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 0.1
 */

class PEAR_Command_Test extends PEAR_Command_Common
{
    // {{{ properties

    var $commands = array(
        'run-tests' => array(
            'summary' => 'Run Regression Tests',
            'function' => 'doRunTests',
            'shortcut' => 'rt',
            'options' => array(
                'recur' => array(
                    'shortopt' => 'r',
                    'doc' => 'Run tests in child directories, recursively.  4 dirs deep maximum',
                ),
                'ini' => array(
                    'shortopt' => 'i',
                    'doc' => 'actual string of settings to pass to php in format " -d setting=blah"',
                    'arg' => 'SETTINGS'
                ),
                'realtimelog' => array(
                    'shortopt' => 'l',
                    'doc' => 'Log test runs/results as they are run',
                ),
                'quiet' => array(
                    'shortopt' => 'q',
                    'doc' => 'Only display detail for failed tests',
                ),
                'simple' => array(
                    'shortopt' => 's',
                    'doc' => 'Display simple output for all tests',
                ),
                'package' => array(
                    'shortopt' => 'p',
                    'doc' => 'Treat parameters as installed packages from which to run tests',
                ),
            ),
            'doc' => '[testfile|dir ...]
Run regression tests with PHP\'s regression testing script (run-tests.php).',
            ),
        );

    var $output;

    // }}}
    // {{{ constructor

    /**
     * PEAR_Command_Test constructor.
     *
     * @access public
     */
    function PEAR_Command_Test(&$ui, &$config)
    {
        parent::PEAR_Command_Common($ui, $config);
    }

    // }}}
    // {{{ doRunTests()

    function doRunTests($command, $options, $params)
    {
        require_once 'PEAR/Common.php';
        require_once 'PEAR/RunTest.php';
        require_once 'System.php';
        $log = new PEAR_Common;
        $log->ui = &$this->ui; // slightly hacky, but it will work
        $run = new PEAR_RunTest($log, $options);
        $tests = array();
        if (isset($options['recur'])) {
            $depth = 4;
        } else {
            $depth = 1;
        }
        if (!count($params)) {
            $params[] = '.';
        }
        if (isset($options['package'])) {
            $oldparams = $params;
            $params = array();
            $reg = &$this->config->getRegistry();
            foreach ($oldparams as $param) {
                $pname = $reg->parsePackageName($param, $this->config->get('default_channel'));
                if (PEAR::isError($pname)) {
                    return $this->raiseError($pname);
                }
                $package = &$reg->getPackage($pname['package'], $pname['channel']);
                if (!$package) {
                    return PEAR::raiseError('Unknown package "' .
                        $reg->parsedPackageNameToString($pname) . '"');
                }
                $filelist = $package->getFilelist();
                foreach ($filelist as $name => $atts) {
                    if (isset($atts['role']) && $atts['role'] != 'test') {
                        continue;
                    }
                    if (!preg_match('/\.phpt$/', $name)) {
                        continue;
                    }
                    $params[] = $atts['installed_as'];
                }
            }
        }
        foreach ($params as $p) {
            if (is_dir($p)) {
                $dir = System::find(array($p, '-type', 'f',
                                            '-maxdepth', $depth,
                                            '-name', '*.phpt'));
                $tests = array_merge($tests, $dir);
            } else {
                if (!@file_exists($p)) {
                    if (!preg_match('/\.phpt$/', $p)) {
                        $p .= '.phpt';
                    }
                    $dir = System::find(array(dirname($p), '-type', 'f',
                                                '-maxdepth', $depth,
                                                '-name', $p));
                    $tests = array_merge($tests, $dir);
                } else {
                    $tests[] = $p;
                }
            }
        }
        $ini_settings = '';
        if (isset($options['ini'])) {
            $ini_settings .= $options['ini'];
        }
        if (isset($_ENV['TEST_PHP_INCLUDE_PATH'])) {
            $ini_settings .= " -d include_path={$_ENV['TEST_PHP_INCLUDE_PATH']}";
        }
        if ($ini_settings) {
            $this->ui->outputData('Using INI settings: "' . $ini_settings . '"');
        }
        $skipped = $passed = $failed = array();
        $this->ui->outputData('Running ' . count($tests) . ' tests', $command);
        $start = time();
        if (isset($options['realtimelog'])) {
            @unlink('run-tests.log');
        }
        foreach ($tests as $t) {
            if (isset($options['realtimelog'])) {
                $fp = @fopen('run-tests.log', 'a');
                if ($fp) {
                    fwrite($fp, "Running test $t...");
                    fclose($fp);
                }
            }
            PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
            $result = $run->run($t, $ini_settings);
            PEAR::staticPopErrorHandling();
            if (PEAR::isError($result)) {
                $this->ui->log(0, $result->getMessage());
                continue;
            }
            if (OS_WINDOWS) {
                for($i=0;$i<2000;$i++) {
                    $i = $i; // delay - race conditions on windows
                }
            }
            if (isset($options['realtimelog'])) {
                $fp = @fopen('run-tests.log', 'a');
                if ($fp) {
                    fwrite($fp, "$result\n");
                    fclose($fp);
                }
            }
            if ($result == 'FAILED') {
            	$failed[] = $t;
            }
            if ($result == 'PASSED') {
            	$passed[] = $t;
            }
            if ($result == 'SKIPPED') {
            	$skipped[] = $t;
            }
        }
        $total = date('i:s', time() - $start);
        if (count($failed)) {
            $output = "TOTAL TIME: $total\n";
            $output .= count($passed) . " PASSED TESTS\n";
            $output .= count($skipped) . " SKIPPED TESTS\n";
    		$output .= count($failed) . " FAILED TESTS:\n";
        	foreach ($failed as $failure) {
        		$output .= $failure . "\n";
        	}
            if (isset($options['realtimelog'])) {
                $fp = @fopen('run-tests.log', 'a');
            } else {
                $fp = @fopen('run-tests.log', 'w');
            }
            if ($fp) {
                fwrite($fp, $output, strlen($output));
                fclose($fp);
                $this->ui->outputData('wrote log to "' . realpath('run-tests.log') . '"', $command);
            }
        } elseif (@file_exists('run-tests.log') && !@is_dir('run-tests.log')) {
            @unlink('run-tests.log');
        }
        $this->ui->outputData('TOTAL TIME: ' . $total);
        $this->ui->outputData(count($passed) . ' PASSED TESTS', $command);
        $this->ui->outputData(count($skipped) . ' SKIPPED TESTS', $command);
        if (count($failed)) {
    		$this->ui->outputData(count($failed) . ' FAILED TESTS:', $command);
        	foreach ($failed as $failure) {
        		$this->ui->outputData($failure, $command);
        	}
        }

        return true;
    }
    // }}}
}

?>
