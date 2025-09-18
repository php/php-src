--TEST--
GH-17422 (OPcache bypasses the user-defined error handler for deprecations) - Fatal Error
--FILE--
<?php

set_error_handler(static function (int $errno, string $errstr, string $errfile, int $errline) {
    function fatal_error() {}
    function fatal_error() {}
});

require __DIR__ . "/warning.inc";

warning();

?>
--EXPECTF--
Fatal error: Cannot redeclare function fatal_error() (previously declared in %s:%d) in %s on line %d
