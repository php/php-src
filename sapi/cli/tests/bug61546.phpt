--TEST--
Bug #61546 (functions related to current script failed when chdir() in cli sapi)
--FILE--
<?php

function test_func($func) {
	$php = getenv("TEST_PHP_EXECUTABLE");
	$test_file = pathinfo(__FILE__, PATHINFO_FILENAME) . '.inc';

	$desc = array(
		0 => STDIN,
		1 => STDOUT,
		2 => STDERR,
	);

	/*
     * The auto test tool will pass absolute path of the current script to php,
     * so use proc_open to execute script as relative filename.
     */
	proc_open("$php -n " . escapeshellarg($test_file) . " $func", $desc, $pipes, dirname(__FILE__));
}

test_func('get_current_user');
test_func('getmyinode');
test_func('getlastmod');

/*
 * getmyuid and getmygid will not be affected because those two functions have fallback
 * when current script can't be found such as run as php -r. listed here just for testing
 */
test_func('getmyuid');
test_func('getmygid');
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
