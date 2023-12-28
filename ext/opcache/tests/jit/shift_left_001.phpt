--TEST--
JIT Shift Left: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
--EXTENSIONS--
opcache
--FILE--
<?php
function shl(int $a, int $b) {
    return $a << $b;
}
var_dump(shl(1, 0));
var_dump(shl(1, 1));
var_dump(shl(1, 2));
var_dump(shl(-1, 2));
try {
    var_dump(shl(1, 64));
} catch (Throwable $e) {
    echo "Exception " . $e->getMessage() . "\n";
}
try {
    var_dump(shl(1, -1));
} catch (Throwable $e) {
    echo "Exception (" . get_class($e) . "): " . $e->getMessage() . "\n";
}
?>
--EXPECT--
int(1)
int(2)
int(4)
int(-4)
int(0)
Exception (ArithmeticError): Bit shift by negative number
