--TEST--
GH-24798 (Tracing JIT loses register-allocated CVs when an exception is caught in the same function)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.jit=tracing
opcache.jit_hot_loop=1
opcache.jit_hot_func=1
opcache.jit_hot_return=1
opcache.jit_hot_side_exit=1
--FILE--
<?php
function f($n) {
    $s = 0.0;
    for ($i = 0; $i < $n; $i++) {
        try {
            $s += 1.0 / ($i - 2);
        } catch (DivisionByZeroError $e) {
            echo $e::class, ': ', $e->getMessage(), ' at $i = ', $i, PHP_EOL;
            $s += 100;
        }
    }
    return $s;
}
var_dump(f(5));
?>
--EXPECT--
DivisionByZeroError: Division by zero at $i = 2
float(100)
