--TEST--
str_repeat() may return a shared string
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=1205
opcache.jit_buffer_size=32M
--FILE--
<?php
function test(int $length): void {
    $input = str_repeat('a', $length);
    str_repeat($input, 1);
    $overwrite = str_repeat('b', $length);
    var_dump($input, $overwrite);
}
test(16);
?>
--EXPECT--
string(16) "aaaaaaaaaaaaaaaa"
string(16) "bbbbbbbbbbbbbbbb"
