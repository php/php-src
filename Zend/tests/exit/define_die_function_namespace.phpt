--TEST--
Attempting to define die() function in a namespace
--FILE--
<?php

namespace Foo;

function die() { }

var_dump(die());

?>
--EXPECTF--
Fatal error: Defining a custom die() function is not allowed, as the function has special semantics in %s on line %d
