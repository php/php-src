--TEST--
Scalar methods: single method calls on a string-literal receiver
--FILE--
<?php
var_dump("  hi  "->trim());
var_dump("AB"->lower());
var_dump("hello"->length());
?>
--EXPECT--
string(2) "hi"
string(2) "ab"
int(5)
