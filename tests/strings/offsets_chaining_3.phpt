--TEST--
testing the behavior of string offset chaining
--FILE--
<?php
$string = "foobar";
var_dump(isset($string[0][0][0][0]));
?>
--EXPECT--
bool(true)
