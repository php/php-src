--TEST--
Attempting to define die() function
--FILE--
<?php

function die() { }

?>
--EXPECTF--
Fatal error: Defining a custom die() function is not allowed, as the function has special semantics in %s on line %d
