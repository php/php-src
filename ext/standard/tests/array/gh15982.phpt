--TEST--
GH-15982 (Assertion failure with array_find when references are involved)
--FILE--
<?php
$var = "hello";
$arrayWithRef = [];
$arrayWithRef[0] =& $var;
var_dump(array_find($arrayWithRef, fn () => true));
?>
--EXPECT--
string(5) "hello"
