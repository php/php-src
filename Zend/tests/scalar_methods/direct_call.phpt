--TEST--
Scalar methods: the Str backing class can still be called directly
--FILE--
<?php
var_dump(Str::trim("  x  "));
var_dump(Str::upper("abc"));
var_dump(Str::lower("ABC"));
var_dump(Str::length("hello"));
?>
--EXPECT--
string(1) "x"
string(3) "ABC"
string(3) "abc"
int(5)
