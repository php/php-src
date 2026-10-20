--TEST--
JIT ASSIGN_OP: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
function test1($a) {
    $a %= 0;
}
function test2($a) {
    $a <<= -1;
}
try {
    test1(1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    test2(1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
DivisionByZeroError: Modulo by zero
ArithmeticError: Bit shift by negative number
