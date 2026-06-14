--TEST--
GH-17422 (OPcache bypasses the user-defined error handler for deprecations) - Fatal after warning
--FILE--
<?php

set_error_handler(static function (int $errno, string $errstr, string $errfile, int $errline) {
	echo "set_error_handler: {$errstr}", PHP_EOL;
});

require __DIR__ . "/warning-fatal.inc";

warning();

?>
--EXPECTF--
Warning: "continue" targeting switch is equivalent to "break" in %s on line %d

Fatal error: Cannot redeclare function warning() (previously declared in %s:%d) in %s on line %d
