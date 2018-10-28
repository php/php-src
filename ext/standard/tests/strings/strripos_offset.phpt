--TEST--
strripos() offset integer overflow
--FILE--
<?php

var_dump(strripos("t", "t", PHP_INT_MAX+1));
var_dump(strripos("tttt", "tt", PHP_INT_MAX+1));
var_dump(strripos(100, 101, PHP_INT_MAX+1));
var_dump(strripos(1024, 1024, PHP_INT_MAX+1));
var_dump(strripos(array(), array(), PHP_INT_MAX+1));
var_dump(strripos(1024, 1024, -PHP_INT_MAX));
var_dump(strripos(1024, "te", -PHP_INT_MAX));
var_dump(strripos(1024, 1024, -PHP_INT_MAX-1));
var_dump(strripos(1024, "te", -PHP_INT_MAX-1));

echo "Done\n";
?>
--EXPECTF--
Warning: strripos() expects parameter 3 to be int, float given in %s on line %d
bool(false)

Warning: strripos() expects parameter 3 to be int, float given in %s on line %d
bool(false)

Warning: strripos() expects parameter 3 to be int, float given in %s on line %d
bool(false)

Warning: strripos() expects parameter 3 to be int, float given in %s on line %d
bool(false)

Warning: strripos() expects parameter 1 to be string, array given in %s on line %d
bool(false)

Deprecated: strripos(): Non-string needles will be interpreted as strings in the future. Use an explicit chr() call to preserve the current behavior in %s on line %d

Warning: strripos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)

Warning: strripos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)

Deprecated: strripos(): Non-string needles will be interpreted as strings in the future. Use an explicit chr() call to preserve the current behavior in %s on line %d

Warning: strripos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)

Warning: strripos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)
Done
