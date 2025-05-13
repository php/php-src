--TEST--
GH-17422 (OPcache bypasses the user-defined error handler for deprecations) - require
--FILE--
<?php

set_error_handler(static function (int $errno, string $errstr, string $errfile, int $errline) {
	require_once __DIR__ . "/dummy.inc";
});

require __DIR__ . "/warning.inc";

dummy();

?>
--EXPECT--
OK: dummy
