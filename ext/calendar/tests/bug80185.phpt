--TEST--
Bug #80185 (jdtounix() fails after 2037)
--SKIPIF--
<?php
if (!extension_loaded('calendar')) die('skip ext/calendar required');
if (PHP_INT_SIZE != 8) die("skip for 64bit platforms only");
?>
--FILE--
<?php
var_dump(jdtounix(2465712));
var_dump(jdtounix(PHP_INT_MAX / 86400 + 2440588));
var_dump(jdtounix(PHP_INT_MAX / 86400 + 2440589));
?>
--EXPECT--
int(2170713600)
int(9223372036854720000)
bool(false)
