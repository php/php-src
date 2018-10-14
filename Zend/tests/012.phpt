--TEST--
class_exists() tests
--FILE--
<?php

class foo {
}

var_dump(class_exists());
var_dump(class_exists("qwerty"));
var_dump(class_exists(""));
var_dump(class_exists(array()));
var_dump(class_exists("test", false));
var_dump(class_exists("foo", false));
var_dump(class_exists("foo"));
var_dump(class_exists("stdClass", false));
var_dump(class_exists("stdClass"));

echo "Done\n";
?>
--EXPECTF--
Warning: class_exists() expects at least 1 parameter, 0 given in %s on line %d
NULL
bool(false)
bool(false)

Warning: class_exists() expects parameter 1 to be string, array given in %s on line %d
NULL
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
Done
