--TEST--
Bug #75944 (wrong detection cp1251 encoding because of missing last cyrillic letter)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
var_dump(mb_detect_encoding(chr(0xfe), array('CP-1251'))); // letter '?'
var_dump(mb_detect_encoding(chr(0xff), array('CP-1251'))); // letter '?'
?>
--EXPECT--
string(12) "Windows-1251"
string(12) "Windows-1251"
