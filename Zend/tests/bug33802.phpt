--TEST--
Bug #33802 (throw Exception in error handler causes crash)
--FILE--
<?php
set_error_handler('errorHandler', E_USER_WARNING);
try {
    test();
} catch(Exception $e){
}
restore_error_handler();

function test() {
    trigger_error("error", E_USER_WARNING);
}

function errorHandler($errno, $errstr, $errfile, $errline) {
    throw new Exception();
}
?>
ok
--EXPECT--
ok
