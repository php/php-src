--TEST--
Attempting to define exit constant in a namespace
--FILE--
<?php

namespace Foo;

const exit = 5;

var_dump(exit);

?>
--EXPECTF--
Parse error: syntax error, unexpected token "exit", expecting identifier in %s on line %d
