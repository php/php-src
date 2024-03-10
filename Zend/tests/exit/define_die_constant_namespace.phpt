--TEST--
Attempting to define die constant in a namespace
--FILE--
<?php

namespace Foo;

const die = 5;

var_dump(die);

?>
--EXPECTF--
Fatal error: Cannot define constant with name die in %s on line %d
