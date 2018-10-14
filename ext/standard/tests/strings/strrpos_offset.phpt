--TEST--
strrpos() offset integer overflow
--FILE--
<?php

var_dump(strrpos("t", "t", PHP_INT_MAX+1));
var_dump(strrpos("tttt", "tt", PHP_INT_MAX+1));
var_dump(strrpos(100, 101, PHP_INT_MAX+1));
var_dump(strrpos(1024, 1024, PHP_INT_MAX+1));
var_dump(strrpos(1024, 1024, -PHP_INT_MAX));
var_dump(strrpos(1024, "te", -PHP_INT_MAX));
var_dump(strrpos(1024, 1024, -PHP_INT_MAX-1));
var_dump(strrpos(1024, "te", -PHP_INT_MAX-1));

echo "Done\n";
?>
--EXPECTF--
Warning: strrpos() expects parameter 3 to be int, float given in %s on line %d
bool(false)

Warning: strrpos() expects parameter 3 to be int, float given in %s on line %d
bool(false)

Warning: strrpos() expects parameter 3 to be int, float given in %s on line %d
bool(false)

Warning: strrpos() expects parameter 3 to be int, float given in %s on line %d
bool(false)

Deprecated: strrpos(): Non-string needles will be interpreted as strings in the future. Use an explicit chr() call to preserve the current behavior in %s on line %d

Warning: strrpos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)

Warning: strrpos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)

Deprecated: strrpos(): Non-string needles will be interpreted as strings in the future. Use an explicit chr() call to preserve the current behavior in %s on line %d

Warning: strrpos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)

Warning: strrpos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)
Done
