--TEST--
Request #69086 (enhancement for mb_convert_encoding)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
mb_substitute_character(0xfffd);
var_dump("?" === mb_convert_encoding("\x80", "Shift_JIS", "EUC-JP"));
mb_internal_encoding("UCS-4BE");
var_dump("\x00\x00\xff\xfd" === mb_convert_encoding("\x80", "UCS-4BE", "UTF-8"));
mb_substitute_character(0xd800);
var_dump("\x00\x00\x00\x3f" === mb_convert_encoding("\x80", "UCS-4BE", "UTF-8"));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)