--TEST--
max_memory_limit beyond size_t is rejected
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8 || PHP_SYS_SIZE != 4) die("skip only for 64-bit integers with 32-bit size_t");
?>
--INI--
max_memory_limit=8G
memory_limit=1G
--FILE--
<?php
var_dump(ini_get('max_memory_limit'));
var_dump(ini_get('memory_limit'));
?>
--EXPECTF--
Warning: Failed to set max_memory_limit to 8589934592 bytes, exceeds the maximum of 4294967295 bytes in Unknown on line 0
string(2) "-1"
string(2) "1G"
