--TEST--
Attempting to define exit() function
--FILE--
<?php

function exit() { }

?>
--EXPECTF--
Fatal error: Defining a custom exit() function is not allowed, as the function has special semantics in %s on line %d
