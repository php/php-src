--TEST--
Bug #80185 (jdtounix() fails after 2037)
--SKIPIF--
<?php
if (!extension_loaded('calendar')) die('skip ext/calendar required');
if (PHP_INT_SIZE != 4) die("skip for 32bit platforms only");
?>
--FILE--
<?php
var_dump(jdtounix(2465712));
var_dump(jdtounix(PHP_INT_MAX / 86400 + 2440588));
var_dump(jdtounix(PHP_INT_MAX / 86400 + 2440589));
?>
--EXPECT--
bool(false)
int(2147472000)
bool(false)
