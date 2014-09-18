--TEST--
Test for bug #67719: is_numeric treats leading and trailing spaces differently
--CREDITS--
Boro Sitnikovski <buritomath@yahoo.com>
--FILE--
<?php
$str1 = " 1";
$str2 = "1 ";
var_dump(is_numeric($str1));
var_dump(is_numeric($str2));
var_dump($str1 == 1);
var_dump($str2 == 1);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
