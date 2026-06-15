--TEST--
Scalar methods: extra arguments are forwarded after the receiver
--FILE--
<?php
// "xxhixx"->trim("x") desugars to Str::trim("xxhixx", "x").
var_dump("xxhixx"->trim("x"));
// Argument plus chaining.
var_dump("--Hi--"->trim("-")->upper());
?>
--EXPECT--
string(2) "hi"
string(2) "HI"
