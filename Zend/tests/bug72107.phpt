--TEST--
Bug #72107: Segfault when using func_get_args as error handler
--FILE--
<?php
set_error_handler('func_get_args');
function test($a) {
    echo $undef;
}
test(1);
?>
--EXPECTF--
Warning: Cannot call func_get_args() dynamically in %s on line %d

Notice: Undefined variable: undef in %s on line %d
