--TEST--
In operator tests for strings
--FILE--
<?php

echo "\n-\n\n";

// compile-time warnings for constant expressions
var_dump(null in "php");
var_dump(true in "php");
var_dump(false in "php");

echo "\n";

// runtime warnings otherwise
$foo = null;
var_dump($foo in "php");

echo "\n";

var_dump("foo" in "foobar");
var_dump("oob" in "foobar");
var_dump("php" in "foobar");

echo "\n";

var_dump(0 in "0");

?>
--EXPECTF--
Warning: Invalid needle type null to haystack type string for in operator in %s on line %d

Warning: Invalid needle type boolean to haystack type string for in operator in %s on line %d

Warning: Invalid needle type boolean to haystack type string for in operator in %s on line %d

-

bool(false)
bool(false)
bool(false)


Warning: Invalid needle type null to haystack type string for in operator in %s on line %d
bool(false)

bool(true)
bool(true)
bool(false)

bool(true)
