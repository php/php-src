--TEST--
Error recording is disabled after a fatal error during compilation
--FILE--
<?php
register_shutdown_function(function () {
    trigger_error('From shutdown', E_USER_WARNING);
});
require __DIR__ . '/record_errors_compile_bailout.inc';
?>
--EXPECTF--
Fatal error: Cannot redeclare A::f() in %srecord_errors_compile_bailout.inc on line %d

Warning: From shutdown in %s on line %d
