--TEST--
testing the behavior of string offset chaining
--INI--
error_reporting=E_ALL | E_DEPRECATED
--FILE--
<?php
$string = "foobar";
var_dump($string{0}{0}[0][0]);
?>
--EXPECTF--
string(1) "f"

