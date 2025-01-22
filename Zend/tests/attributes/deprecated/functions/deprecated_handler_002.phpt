--TEST--
#[\Deprecated]: Exception Handler is deprecated for throwing error handler.
--FILE--
<?php

function my_error_handler(int $errno, string $errstr, ?string $errfile = null, ?int $errline = null) {
	throw new \ErrorException($errstr, 0, $errno, $errfile, $errline);
}

set_error_handler('my_error_handler');

#[\Deprecated]
function my_exception_handler($e) {
	echo "Handled: ", $e->getMessage(), PHP_EOL;
};

set_exception_handler('my_exception_handler');

#[\Deprecated]
function test() {
}

test();

?>
--EXPECTF--
Fatal error: Uncaught ErrorException: Function my_exception_handler() is deprecated in Unknown:0
Stack trace:
#0 [internal function]: my_error_handler(%d, '%s', '%s', %d)
#1 {main}
  thrown in Unknown on line 0
