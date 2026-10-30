--TEST--
Test sprintf() function : Rope Optimization with a throwing error handler.
--FILE--
<?php

function exception_error_handler(int $errno, string $errstr, ?string $errfile, int $errline) {
	if (!(error_reporting() & $errno)) {
		// This error code is not included in error_reporting
		return;
	}
	throw new \ErrorException($errstr, 0, $errno, $errfile, $errline);
}
set_error_handler(exception_error_handler(...));

try {
	var_dump(sprintf("%s-%s", new stdClass(), []));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

echo "Done";
?>
--EXPECTF--
Error: Object of class stdClass could not be converted to string in %s:13
Stack trace:
#0 {main}

Done
