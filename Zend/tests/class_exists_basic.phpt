--TEST--
class_exists() tests
--FILE--
<?php

class foo {
}

var_dump(class_exists("qwerty"));
var_dump(class_exists(""));
var_dump(class_exists("test", false));
var_dump(class_exists("foo", false));
var_dump(class_exists("foo"));
var_dump(class_exists("stdClass", false));
var_dump(class_exists("stdClass"));

echo "Done\n";
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
Done
