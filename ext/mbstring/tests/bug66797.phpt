--TEST--
Bug #66797 (mb_substr only takes 32-bit signed integer)
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
if (PHP_INT_SIZE != 8) die('skip this test is for 64bit platforms only');
?>
--FILE--
<?php
var_dump(
    mb_substr('bar', 0, 0x7fffffff),
    mb_substr('bar', 0, 0x80000000),
    mb_substr('bar', 0xffffffff, 1),
    mb_substr('bar', 0x100000000, 1)
);
?>
==DONE==
--EXPECTF--
string(3) "bar"
string(3) "bar"
string(0) ""
string(0) ""
==DONE==
