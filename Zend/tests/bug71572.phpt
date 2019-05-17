--TEST--
Bug #71572: String offset assignment from an empty string inserts null byte
--FILE--
<?php

$str = "abc";
var_dump($str{0} = "");
var_dump($str{1} = "");
var_dump($str{3} = "");
var_dump($str{10} = "");
var_dump($str);
?>
==DONE==
--EXPECTF--
Warning: Cannot assign an empty string to a string offset in %s on line %d
NULL

Warning: Cannot assign an empty string to a string offset in %s on line %d
NULL

Warning: Cannot assign an empty string to a string offset in %s on line %d
NULL

Warning: Cannot assign an empty string to a string offset in %s on line %d
NULL
string(3) "abc"
==DONE==
