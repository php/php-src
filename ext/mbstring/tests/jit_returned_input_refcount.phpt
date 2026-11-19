--TEST--
mb_encode_mimeheader() and mb_output_handler() returning their input string under function JIT
--EXTENSIONS--
mbstring
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=function
opcache.jit_buffer_size=64M
opcache.file_update_protection=0
--FILE--
<?php
$s = str_repeat("a", mt_rand(3, 3));
mb_encode_mimeheader($s);
mb_http_output("UTF-8");
mb_output_handler($s, 0);
mb_http_output("pass");
mb_output_handler($s, 0);
echo "input after calls: ";
var_dump($s);
echo "mb_encode_mimeheader() returns input: ";
var_dump(mb_encode_mimeheader($s) === $s);
echo "mb_output_handler() returns input: ";
var_dump(mb_output_handler($s, 0) === $s);
?>
--EXPECT--
input after calls: string(3) "aaa"
mb_encode_mimeheader() returns input: bool(true)
mb_output_handler() returns input: bool(true)
