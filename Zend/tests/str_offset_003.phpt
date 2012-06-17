--TEST--
string offset 003
--FILE--
<?php
$a = "aaa";
var_dump($a);
$a[-1] = "b";
var_dump($a);
$a[-2] = "b";
var_dump($a);
$a[-3] = "b";
var_dump($a);
?>
--EXPECTF--
string(3) "aaa"
string(3) "aab"
string(3) "abb"
string(3) "bbb"