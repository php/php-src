--TEST--
Attempting to define exit constant in a namespace
--FILE--
<?php

namespace Foo;

const exit = 5;

var_dump(exit);

?>
--EXPECTF--
Fatal error: Cannot define constant with name exit in %s on line %d
