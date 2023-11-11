--TEST--
Register Alloction 003: Reuse temporary register
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
--EXTENSIONS--
opcache
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
--EXPECTF--
Deprecated: Implicit conversion from float 4294967168 to int loses precision in %s on line %d
correct
