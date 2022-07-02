--TEST--
Bug #49536 (mb_detect_encoding() returns incorrect results when strict_mode is turned on)
--EXTENSIONS--
mbstring
--FILE--
<?php
// non-strict mode
var_dump(mb_detect_encoding("A\x81", "SJIS", false));
// strict mode
var_dump(mb_detect_encoding("A\x81", "SJIS", true));
// non-strict mode
var_dump(mb_detect_encoding("\xc0\x00", "UTF-8", false));
// strict mode
var_dump(mb_detect_encoding("\xc0\x00", "UTF-8", true));
?>
--EXPECT--
string(4) "SJIS"
bool(false)
bool(false)
bool(false)
