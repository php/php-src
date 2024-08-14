--TEST--
Attempting to define die() function in a namespace
--FILE--
<?php

namespace Foo;

function die() { }

var_dump(die());

?>
--EXPECTF--
Parse error: syntax error, unexpected token "exit", expecting "(" in %s on line %d
