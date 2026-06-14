--TEST--
First class callable error: more than one argument
--FILE--
<?php

foo(1, ...);

?>
--EXPECTF--
Fatal error: Cannot create a Closure for call expression with more than one argument, or non-variadic placeholders in %s on line %d
