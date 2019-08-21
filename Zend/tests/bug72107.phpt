--TEST--
Bug #72107: Segfault when using func_get_args as error handler
--FILE--
<?php
try {
    set_error_handler('func_get_args');
} catch (\Error $e) {
    echo $e->getMessage();
}
function test($a) {
    echo $undef;
}
test(1);
?>
--EXPECTF--
Cannot call func_get_args() dynamically
Notice: Undefined variable: undef in %s on line %d
