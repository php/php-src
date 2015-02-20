--TEST--
Request #69086 (enhancement for mb_convert_encoding)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
mb_substitute_character(0xFFFD);
var_dump(mb_convert_encoding("\x80", "Shift_JIS", "EUC-JP"));
?>
--EXPECT--
string(1) "?"