--TEST--
JIT ASSIGN_DIM: undefined OP_DATA is not read when appending to a string
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=function
--FILE--
<?php
function test() {
    $x = 'ab';
    $x[] = $undef;
}
try {
    test();
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: [] operator not supported for strings
