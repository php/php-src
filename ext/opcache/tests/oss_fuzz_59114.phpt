--TEST--
Incorrect range inference for null|undefined operands
--EXTENSIONS--
opcache
--INI--
opcache.enable_cli=1
opcache.jit=1205
opcache.jit_buffer_size=16M
--FILE--
<?php
function test($a){
    for (; $a < -1;) {
        42 % ($b = $a + $a);
    }
}
try {
    test(NULL);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Modulo by zero
