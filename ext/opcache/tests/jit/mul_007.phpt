--TEST--
JIT MUL: 007 incorrect optimization
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--FILE--
<?php
function test() {
    1.5%2%2%2/2%2;
}
test();
?>
DONE
--EXPECT--
DONE