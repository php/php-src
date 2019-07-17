--TEST--
interface_exists() tests
--FILE--
<?php

interface foo {
}

var_dump(interface_exists("qwerty"));
var_dump(interface_exists(""));
var_dump(interface_exists("test", false));
var_dump(interface_exists("foo", false));
var_dump(interface_exists("foo"));
var_dump(interface_exists("stdClass", false));
var_dump(interface_exists("stdClass"));

echo "Done\n";
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
Done
