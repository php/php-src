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
--EXPECTF--
Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_split() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(false)
