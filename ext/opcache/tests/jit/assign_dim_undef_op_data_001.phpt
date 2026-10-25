--TEST--
JIT ASSIGN_DIM: undefined OP_DATA is not read when the container is a scalar
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=function
--FILE--
<?php
function test() {
    $x = 1.5;
    $x[0] = $undef;
}
try {
    test();
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Cannot use a scalar value as an array
