--TEST--
testing the behavior of string offset chaining
--FILE--
<?php
$string = "foobar";
var_dump($string[0][0][0][0]);
?>
--EXPECT--
string(1) "f"
