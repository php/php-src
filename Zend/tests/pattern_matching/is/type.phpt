--TEST--
Type pattern matching
--FILE--
<?php

class Foo {}
$foo = new Foo();

var_dump('' is string);
var_dump('' is int);
var_dump('' is bool);
var_dump('' is object);
var_dump('' is ?string);
var_dump(null is ?string);
var_dump('' is ?object);

?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
