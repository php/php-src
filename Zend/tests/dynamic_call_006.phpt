--TEST--
Dynamic calls to scope introspection functions are forbidden (function variations)
--FILE--
<?php
function test() {
    $func = 'extract';
    $func(['a' => 'b']);

    $func = 'compact';
    $func(['a']);

    $func = 'get_defined_vars';
    $func();

    $func = 'func_get_args';
    $func();

    $func = 'func_get_arg';
    $func(1);

    $func = 'func_num_args';
    $func();
}
test();

?>
--EXPECTF--
Warning: Cannot call extract() dynamically in %s on line %d

Warning: Cannot call compact() dynamically in %s on line %d

Warning: Cannot call get_defined_vars() dynamically in %s on line %d

Warning: Cannot call func_get_args() dynamically in %s on line %d

Warning: Cannot call func_get_arg() dynamically in %s on line %d

Warning: Cannot call func_num_args() dynamically in %s on line %d
