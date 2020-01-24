--TEST--
Offset errors for various strpos functions
--FILE--
<?php

var_dump(mb_strpos("f", "bar", 3));
var_dump(mb_strpos("f", "bar", -3));
var_dump(mb_strrpos("f", "bar", 3));
var_dump(mb_strrpos("f", "bar", -3));
var_dump(mb_stripos("f", "bar", 3));
var_dump(mb_stripos("f", "bar", -3));
var_dump(mb_strripos("f", "bar", 3));
var_dump(mb_strripos("f", "bar", -3));

?>
--EXPECTF--
Warning: mb_strpos(): Offset not contained in string in %s on line %d
bool(false)

Warning: mb_strpos(): Offset not contained in string in %s on line %d
bool(false)

Warning: mb_strrpos(): Offset not contained in string in %s on line %d
bool(false)

Warning: mb_strrpos(): Offset not contained in string in %s on line %d
bool(false)

Warning: mb_stripos(): Offset not contained in string in %s on line %d
bool(false)

Warning: mb_stripos(): Offset not contained in string in %s on line %d
bool(false)

Warning: mb_strripos(): Offset not contained in string in %s on line %d
bool(false)

Warning: mb_strripos(): Offset not contained in string in %s on line %d
bool(false)
