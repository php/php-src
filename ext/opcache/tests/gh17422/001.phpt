--TEST--
GH-17422 (OPcache bypasses the user-defined error handler for deprecations)
--FILE--
<?php

set_error_handler(static function (int $errno, string $errstr, string $errfile, int $errline) {
	echo "set_error_handler: {$errstr}", PHP_EOL;
});

require __DIR__ . "/warning.inc";

warning();

?>
--EXPECT--
set_error_handler: "continue" targeting switch is equivalent to "break"
OK: warning
