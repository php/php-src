<?php

/**
 * HTML output for PHPUnit suite tests.
 *
 * Copied for PEAR_PackageFileManager from HTML_CSS
 * @version    $Id$
 * @author     Laurent Laville <pear@laurent-laville.org>
 * @package    HTML_CSS
 */

require_once 'TestUnit.php';
require_once 'HTML_TestListener.php';
require_once 'PEAR/ErrorStack.php';

$title = 'PhpUnit test run, PEAR_ErrorStack package';
?>
<html>
<head>
<title><?php echo $title; ?></title>
<link rel="stylesheet" href="stylesheet.css" type="text/css" />
</head>
<body>
<h1><?php echo $title; ?></h1>
      <p>
	This page runs all the phpUnit self-tests, and produces nice HTML output.
      </p>
      <p>
	Unlike typical test run, <strong>expect many test cases to
	  fail</strong>.  Exactly those with <code>pass</code> in their name
	should succeed.
      </p>
      <p>
      For each test we display both the test result -- <span
      class="Pass">ok</span>, <span class="Failure">FAIL</span>, or
      <span class="Error">ERROR</span> -- and also a meta-result --
      <span class="Expected">as expected</span>, <span
      class="Unexpected">UNEXPECTED</span>, or <span
      class="Unknown">unknown</span> -- that indicates whether the
      expected test result occurred.  Although many test results will
      be 'FAIL' here, all meta-results should be 'as expected', except
      for a few 'unknown' meta-results (because of errors) when running
      in PHP3.
      </p>
      
<h2>Tests</h2>
	<?php
	$testcases = array(
    	    'PEAR_ErrorStack_TestCase_singleton',
    	    'PEAR_ErrorStack_TestCase_pushpop',
    	    'PEAR_ErrorStack_TestCase_pushpopstatic',
    	    'PEAR_ErrorStack_TestCase_pushpopcallback',
    	    'PEAR_ErrorStack_TestCase_getErrorMessage',
    	    'PEAR_ErrorStack_TestCase_getErrorMessageTemplate',
    	    'PEAR_ErrorStack_TestCase_getErrors',
    	    'PEAR_ErrorStack_TestCase_staticGetErrors',
	);
define('PEAR_LOG_EMERG',    0);     /** System is unusable */
define('PEAR_LOG_ALERT',    1);     /** Immediate action required */
define('PEAR_LOG_CRIT',     2);     /** Critical conditions */
define('PEAR_LOG_ERR',      3);     /** Error conditions */
define('PEAR_LOG_WARNING',  4);     /** Warning conditions */
define('PEAR_LOG_NOTICE',   5);     /** Normal but significant */
define('PEAR_LOG_INFO',     6);     /** Informational */
define('PEAR_LOG_DEBUG',    7);     /** Debug-level messages */
/**
* Mock Log object
*/
class BurfLog {
    var $testcase;
    var $method;
    var $expect = array();
    function setTestCase(&$testcase)
    {
        $this->testcase = &$testcase;
    }
    
    function curMethod($method)
    {
        $this->method = $method;
    }
    
    function pushExpect($message, $priority, $errarray)
    {
        unset($errarray['time']);
        unset($errarray['context']);
        array_push($this->expect, array($message, $priority, $errarray));
    }
    
    function clearExpect()
    {
        $this->expect = array();
    }

    function log($message, $priority, $errarray)
    {
        $this->testcase->wasLogged = true;
        if (!is_a($this->testcase, 'PHPUnit_TestCase')) {
            trigger_error('ERROR: burflog never set up', E_USER_ERROR);
            return;
        }
        if (!isset($this->method)) {
            $this->testcase->assertFalse(true, 'ERROR: burflog never set up');
            return;
        }
        if (!count($this->expect)) {
            $this->testcase->assertFalse(true, "method $this->method: logged, but no log expected");
            $this->testcase->assertFalse(true, "method $this->method: log message = $message");
            $this->testcase->assertFalse(true, "method $this->method: log priority = $priority");
            return;
        }
        unset($errarray['time']);
        unset($errarray['context']);
        $expect = array_pop($this->expect);
        $this->testcase->assertEquals($expect[0], $message, "method $this->method: wrong message");
        $this->testcase->assertEquals($expect[1], $priority, "method $this->method: wrong priority");
        $this->testcase->assertEquals($expect[2], $errarray, "method $this->method: wrong errarray");
    }
}
	
	$suite = new PHPUnit_TestSuite();

	foreach ($testcases as $testcase) {
    	    include_once $testcase . '.php';
            $suite->addTestSuite($testcase);
	}

	$listener = new HTML_TestListener();
    $finalresult = TestUnit::run($suite, $listener);
    $results = include_once dirname(__FILE__) . '/base_regression.php';
    $num = $results['number'];
    $failed = $results['failed'];
    $passed = $results['passed'];
    for ($i = 1; $i <= $num; $i++) {
        $bla = new Mock_PHPUnit;
        $bla->name = $i;
        $listener->startTest($bla);
    	if (isset($failed[$i])) {
    	    $listener->addFailure($bla, $failed[$i]);
            $finalresult->addFailure($bla, $a = 'context had additional ' . serialize($failed[$i]));
        }
        $listener->endTest($bla);
    }

	$finalresult->removeListener($listener);
    // hack in the base regression test count
    $finalresult->_runTests += count($results['failed']) + count($results['passed']);
	$finalresult->report();

	?>
</body>
</html>
