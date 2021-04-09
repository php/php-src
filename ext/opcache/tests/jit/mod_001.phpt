--TEST--
JIT MOD: 001
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
function mod(int $a, int $b) {
    return $a % $b;
}
var_dump(mod(125, 33));
var_dump(mod(125, 32));
var_dump(mod(-125, 33));
var_dump(mod(-125, 32));
var_dump(mod(125, -33));
var_dump(mod(-125, -33));
try {
    var_dump(mod(125, -1));
} catch (Throwable $e) {
    echo "Exception " . $e->getMessage() . "\n";
}
try {
    var_dump(mod(125, 0));
} catch (Throwable $e) {
    echo "Exception (" . get_class($e) . "): " . $e->getMessage() . "\n";
}
?>
--EXPECT--
int(26)
int(29)
int(-26)
int(-29)
int(26)
int(-26)
int(0)
Exception (DivisionByZeroError): Modulo by zero
