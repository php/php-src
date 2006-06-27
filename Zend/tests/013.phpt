--TEST--
interface_exists() tests
--FILE--
<?php

interface foo {
}

var_dump(interface_exists());
var_dump(interface_exists("qwerty"));
var_dump(interface_exists(""));
var_dump(interface_exists(array()));
var_dump(interface_exists("test", false));
var_dump(interface_exists("foo", false));
var_dump(interface_exists("foo"));
var_dump(interface_exists("stdClass", false));
var_dump(interface_exists("stdClass"));

echo "Done\n";
?>
--EXPECTF--	
Warning: interface_exists() expects at least 1 parameter, 0 given in %s on line %d
NULL
bool(false)
bool(false)

Warning: interface_exists() expects parameter 1 to be string, array given in %s on line %d
NULL
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
Done
