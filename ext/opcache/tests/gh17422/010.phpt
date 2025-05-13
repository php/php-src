--TEST--
GH-17422 (OPcache bypasses the user-defined error handler for deprecations) - Inheritance warning
--FILE--
<?php

set_error_handler(static function (int $errno, string $errstr, string $errfile, int $errline) {
	echo "set_error_handler: {$errstr}", PHP_EOL;
});

require __DIR__ . "/link-warning.inc";

?>
--EXPECTF--
set_error_handler: Return type of C::getTimezone() should either be compatible with DateTime::getTimezone(): DateTimeZone|false, or the #[\ReturnTypeWillChange] attribute should be used to temporarily suppress the notice
