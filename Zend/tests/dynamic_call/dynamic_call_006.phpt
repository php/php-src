--TEST--
Dynamic calls to scope introspection functions are forbidden (function variations)
--FILE--
<?php
function test() {

    try {
        $func = 'extract';
        $func(['a' => 'b']);
    } catch (\Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }

    try {
        $func = 'compact';
        $func(['a']);
    } catch (\Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }

    try {
        $func = 'get_defined_vars';
        $func();
    } catch (\Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }

    try {
        $func = 'func_get_args';
        $func();
    } catch (\Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }

    try {
        $func = 'func_get_arg';
        $func(1);
    } catch (\Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }

    try {
        $func = 'func_num_args';
        $func();
    } catch (\Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
test();

?>
--EXPECT--
Error: Cannot call extract() dynamically
Error: Cannot call compact() dynamically
Error: Cannot call get_defined_vars() dynamically
Error: Cannot call func_get_args() dynamically
Error: Cannot call func_get_arg() dynamically
Error: Cannot call func_num_args() dynamically
