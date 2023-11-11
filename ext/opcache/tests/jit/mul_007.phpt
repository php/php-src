--TEST--
JIT MUL: 007 incorrect optimization
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
--FILE--
<?php
function test() {
    1.5%2%2%2/2%2;
}
test();
?>
DONE
--EXPECTF--
Deprecated: Implicit conversion from float 1.5 to int loses precision in %smul_007.php on line 3

Deprecated: Implicit conversion from float 0.5 to int loses precision in %smul_007.php on line 3
DONE