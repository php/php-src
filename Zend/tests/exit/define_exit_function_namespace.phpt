--TEST--
Attempting to define exit() function in a namespace
--FILE--
<?php

namespace Foo;

function exit() { }

?>
--EXPECTF--
Parse error: syntax error, unexpected token "exit", expecting "(" in %s on line %d
