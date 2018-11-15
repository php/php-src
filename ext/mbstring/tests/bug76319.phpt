--TEST--
Bug #76319: mb_strtolower with invalid UTF-8 causes segmentation fault
--SKIPIF--
<?php if(!extension_loaded('mbstring')) die('skip mbstring not loaded'); ?>
--FILE--
<?php
mb_substitute_character(0xFFFD);
var_dump(mb_strtolower("a\xA1", 'UTF-8'));
?>
--EXPECT--
string(4) "a�"
