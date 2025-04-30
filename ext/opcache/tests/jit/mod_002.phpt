--TEST--
JIT MOD: 002
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
--EXTENSIONS--
opcache
--FILE--
<?php
function mod33(int $a) {
    return $a % 33;
}
function mod32(int $a) {
    return $a % 32;
}
function modNeg33(int $a) {
    return $a % -33;
}
function modNeg1(int $a) {
    return $a % -1;
}
function mod0(int $a) {
    return $a % 0;
}
var_dump(mod33(125));
var_dump(mod32(125));
var_dump(mod33(-125));
var_dump(mod32(-125));
var_dump(modNeg33(125));
var_dump(modNeg33(-125));
try {
    var_dump(modNeg1(125));
} catch (Throwable $e) {
    echo "Exception " . $e->getMessage() . "\n";
}
try {
    var_dump(mod0(125));
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
