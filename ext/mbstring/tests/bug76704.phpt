--TEST--
Bug #76704 (mb_detect_order return value varies based on argument type)
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
?>
--FILE--
<?php
var_dump(mb_detect_order('Foo, UTF-8'));
var_dump(mb_detect_order(['Foo', 'UTF-8']))
?>
===DONE===
--EXPECT--
bool(false)
bool(false)
===DONE===
