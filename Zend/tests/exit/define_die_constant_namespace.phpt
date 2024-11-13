--TEST--
Attempting to define die constant in a namespace
--FILE--
<?php

namespace Foo;

const die = 5;

var_dump(die);

?>
--EXPECTF--
Parse error: syntax error, unexpected token "exit", expecting identifier in %s on line %d
