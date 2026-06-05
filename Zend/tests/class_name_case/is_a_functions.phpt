--TEST--
Class name with incorrect case returns false in is_a and is_subclass_of
--FILE--
<?php
class Foo extends Exception {}

$o = new Foo();

// correct case
var_dump(is_a($o, 'Foo'));
var_dump(is_subclass_of($o, 'Exception'));

// wrong case - returns false
var_dump(is_a($o, 'FOO'));
var_dump(is_subclass_of($o, 'EXCEPTION'));
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
