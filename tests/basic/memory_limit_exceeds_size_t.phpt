--TEST--
memory_limit beyond size_t is caught by max_memory_limit
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8 || PHP_SYS_SIZE != 4) die("skip only for 64-bit integers with 32-bit size_t");
?>
--INI--
memory_limit=8G
--FILE--
<?php
var_dump(ini_get('max_memory_limit'));
var_dump(ini_get('memory_limit'));
ini_set('memory_limit', '16G');
var_dump(ini_get('memory_limit'));
?>
--EXPECTF--
Warning: Failed to set memory_limit to 8589934592 bytes. Setting to max_memory_limit instead (currently: 4294967295 bytes) in Unknown on line 0
string(2) "-1"
string(2) "-1"

Warning: Failed to set memory_limit to 17179869184 bytes. Setting to max_memory_limit instead (currently: 4294967295 bytes) in %s on line %d
string(2) "-1"
