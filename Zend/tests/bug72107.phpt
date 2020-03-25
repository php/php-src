--TEST--
Bug #72107: Segfault when using func_get_args as error handler
--FILE--
<?php
set_error_handler('func_get_args');
function test($a) {
    echo $undef;
}
try {
    test(1);
} catch (\Error $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
Cannot call func_get_args() dynamically
