--TEST--
First class callable error: non-variadic placeholder
--FILE--
<?php

foo(?);

?>
--EXPECTF--
Fatal error: Cannot create a Closure for call expression with more than one argument, or non-variadic placeholders in %s on line %d
