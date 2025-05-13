--TEST--
GH-17422 (OPcache bypasses the user-defined error handler for deprecations) - Throwing error handler
--FILE--
<?php

set_error_handler(static function (int $errno, string $errstr, string $errfile, int $errline) {
	throw new \ErrorException($errstr, 0, $errno, $errfile, $errline);
});

try {
	require __DIR__ . "/warning.inc";
} catch (\Exception $e) {
	echo "Caught: ", $e->getMessage(), PHP_EOL;
}

warning();

?>
--EXPECT--
Caught: "continue" targeting switch is equivalent to "break"
OK: warning
