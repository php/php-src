--TEST--
Bug #49536 (mb_detect_encoding() returns incorrect results when strict_mode is turned on)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
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
string(5) "UTF-8"
bool(false)
