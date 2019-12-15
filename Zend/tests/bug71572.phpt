--TEST--
Bug #71572: String offset assignment from an empty string inserts null byte
--FILE--
<?php

$str = "abc";
var_dump($str[0] = "");
var_dump($str[1] = "");
var_dump($str[3] = "");
var_dump($str[10] = "");
var_dump($str);
?>
--EXPECT--
string(2) "bc"
string(1) "b"
string(3) "b  "
string(10) "b         "
string(10) "b         "
