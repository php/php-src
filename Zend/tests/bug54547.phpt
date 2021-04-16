--TEST--
Bug #54547: wrong equality of string numbers near LONG_MAX with 64-bit longs
--PLATFORM--
bits: 64
--FILE--
<?php
var_dump("9223372036854775807" == "9223372036854775808");
var_dump("-9223372036854775808" == "-9223372036854775809");
var_dump("0x7fffffffffffffff" == "9223372036854775808");

/* not exactly what the bug is about, but closely related problem: */
var_dump("999223372036854775807"=="999223372036854775808");
var_dump("899223372036854775807">"00999223372036854775807");
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
