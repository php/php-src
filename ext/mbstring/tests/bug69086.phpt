--TEST--
Request #69086 (enhancement for mb_convert_encoding)
--EXTENSIONS--
mbstring
--FILE--
<?php
mb_substitute_character(0xfffd);
var_dump("?" === mb_convert_encoding("\x80", "Shift_JIS", "EUC-JP"));
mb_internal_encoding("UCS-4BE");
var_dump("\x00\x00\xff\xfd" === mb_convert_encoding("\x80", "UCS-4BE", "UTF-8"));

mb_internal_encoding("UTF-8");
mb_substitute_character(0xfffd);
var_dump("\u{fffd}" === mb_convert_encoding("\x80", "UTF-8", "EUC-JP-2004"));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
