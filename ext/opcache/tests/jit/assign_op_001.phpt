--TEST--
JIT ASSIGN_OP: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
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
} catch (DivisionByZeroError $e) {
    echo $e->getMessage(), "\n";
}
try {
    test2(1);
} catch (ArithmeticError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Modulo by zero
Bit shift by negative number
