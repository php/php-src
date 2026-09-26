--TEST--
Bug #66797 (mb_substr and mb_strcut only take 32-bit signed integer)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die('skip this test is for 64bit integers only');
?>
--FILE--
<?php
var_dump(
    mb_substr('bar', 0, 0x7fffffff),
    mb_substr('bar', 0, 0x80000000),
    mb_substr('bar', 0, 0x100000000),
    mb_substr('bar', 0xffffffff, 1),
    mb_substr('bar', 0x100000000, 1),
    mb_strcut('bar', 0, 0x100000000),
    mb_strcut('bar', 0x100000000, 1)
);
?>
--EXPECT--
string(3) "bar"
string(3) "bar"
string(3) "bar"
string(0) ""
string(0) ""
string(3) "bar"
string(0) ""
