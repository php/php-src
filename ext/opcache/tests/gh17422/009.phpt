--TEST--
GH-17422 (OPcache bypasses the user-defined error handler for deprecations) - Early binding error after warning
--FILE--
<?php

set_error_handler(static function (int $errno, string $errstr, string $errfile, int $errline) {
	echo "set_error_handler: {$errstr}", PHP_EOL;
});

require __DIR__ . "/early-bind-warning-error.inc";

?>
--EXPECTF--
Deprecated: Return type of C::getTimezone() should either be compatible with DateTime::getTimezone(): DateTimeZone|false, or the #[\ReturnTypeWillChange] attribute should be used to temporarily suppress the notice in %s on line %d

Fatal error: Declaration of C::getTimestamp(C $arg): int must be compatible with DateTime::getTimestamp(): int in %s on line %d
