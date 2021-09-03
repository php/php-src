--TEST--
Bug #77418 (Heap overflow in utf32be_mbc_to_code)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_split')) die('skip mb_split() not available');
?>
--FILE--
<?php
mb_regex_encoding("UTF-32");

var_dump(mb_split("\x00\x00\x00\x5c\x00\x00\x00B","000000000000000000000000000000"));
?>
--EXPECT--
bool(false)
