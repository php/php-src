--TEST--
JIT Shift Right: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--EXTENSIONS--
opcache
--FILE--
<?php
function shr(int $a, int $b) {
    return $a >> $b;
}
var_dump(shr(256, 0));
var_dump(shr(256, 1));
var_dump(shr(256, 2));
var_dump(shr(-8, 2));
try {
    var_dump(shr(1, 64));
} catch (Throwable $e) {
    echo "Exception " . $e->getMessage() . "\n";
}
try {
    var_dump(shr(-1, 64));
} catch (Throwable $e) {
    echo "Exception " . $e->getMessage() . "\n";
}
try {
    var_dump(shr(1, -1));
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
