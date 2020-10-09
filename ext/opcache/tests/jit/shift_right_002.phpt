--TEST--
JIT Shift Right: 002
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function shr0(int $a) {
    return $a >> 0;
}
function shr1(int $a) {
    return $a >> 1;
}
function shr2(int $a) {
    return $a >> 2;
}
function shr64(int $a) {
    return $a >> 64;
}
function shrNEG(int $a) {
    return $a >> -1;
}
var_dump(shr0(256));
var_dump(shr1(256));
var_dump(shr2(256));
var_dump(shr2(-8));
try {
    var_dump(shr64(1));
} catch (Throwable $e) {
    echo "Exception " . $e->getMessage() . "\n";
}
try {
    var_dump(shr64(-1));
} catch (Throwable $e) {
    echo "Exception " . $e->getMessage() . "\n";
}
try {
    var_dump(shrNEG(1));
} catch (Throwable $e) {
    echo "Exception (" . get_class($e) . "): " . $e->getMessage() . "\n";
}
?>
--EXPECT--
int(256)
int(128)
int(64)
int(-2)
int(0)
int(-1)
Exception (ArithmeticError): Bit shift by negative number
