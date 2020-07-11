--TEST--
Register Alloction 003: Reuse temporary register
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
function test($char_code) {
    if ($char_code == !($char_code & 0xffffff80)) {
        return "correct";
    } else {
        return "wrong";
    }
}
echo test(65), "\n";
?>
--EXPECT--
correct
