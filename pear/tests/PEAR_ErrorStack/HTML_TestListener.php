<?php

/**
 * Provides a nice HTML output for PHPUnit suite tests.
 * 
 * @version    $Id$
 * @author     Laurent Laville <pear@laurent-laville.org>
 * @package    HTML_CSS
 */

class HTML_TestListener extends PHPUnit_TestListener {

    function HTML_TestListener() {

$report = <<<EOS
<table cellspacing="1" cellpadding="1" border="0" width="90%" align="center" class="details">
<tr><th>Class</th><th>Function</th><th>Success</th><th>Meta-result</th></tr>
EOS;
        echo $report;
    }

    function addError(&$test, &$t) {
        $this->_errors += 1;
    }

    function addFailure(&$test, &$t) {
        $this->_fails += 1;
    }

    function endTest(&$test) {
	/* Report both the test result and, for this special situation
	   where some tests are expected to fail, a "meta" test result
	   which indicates whether the test result matches the
	   expected result. 
	 */
	$expect_failure = preg_match('/fail/i', $test->getName());
	$test_passed = ($this->_fails == 0 && $this->_errors == 0);

	if ($this->_errors > 0) {
	    $outcome = "<span class=\"Error\">ERROR</span>";
	} else if ($this->_fails > 0) {
	    $outcome = "<span class=\"Failure\">FAIL</span>";
	} else {
	    $outcome = "<span class=\"Pass\">OK</span>";
        }
	if ($this->_errors > 0) {
	    $meta_outcome = '<span class="Unknown">unknown</span>';
	} else {
	    $meta_outcome = ($expect_failure xor $test_passed)
		? '<span class="Expected">as expected</span>'
		: '<span class="Unexpected">UNEXPECTED</span>';
        }
	printf("<td>$outcome</td><td>$meta_outcome</td></tr>");
    }

    function startTest(&$test) {
        $this->_fails = 0;
        $this->_errors = 0;
        printf("<tr><td>%s </td><td>%s </td>", get_class($test), $test->getName());
    }


}
?>