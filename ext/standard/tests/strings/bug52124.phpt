--TEST--
Test for bug #52124: [strpos] Limit search inside $haystack by new $length parameter
--CREDITS--
Boro Sitnikovski <buritomath@yahoo.com>
--FILE--
<?php
$string = "The big string [with some data here] and with MoRe DaTa there";

var_dump(strpos($string, "data", 16, 15));
var_dump(strpos($string, "more data", 16, 150));
var_dump(strpos($string, "more data", 16, -1));

var_dump(stripos($string, "more data", 46, 9));
var_dump(stripos($string, "more data", 16, 150));
var_dump(stripos($string, "more data", 16, -1));
?>
--EXPECTF--
int(26)

Warning: strpos(): Offset not contained in string in %s on line %d
bool(false)

Warning: strpos(): Length parameter may not be negative in %s on line %d
bool(false)
int(46)

Warning: stripos(): Offset not contained in string in %s on line %d
bool(false)

Warning: stripos(): Length parameter may not be negative in %s on line %d
bool(false)
