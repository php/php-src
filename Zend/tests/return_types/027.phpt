--TEST--
Return type of parent is not allowed in closure
--FILE--
<?php

$c = function(): parent {};

--EXPECTF--
Fatal error: Cannot declare a return type of parent outside of a class scope in %s on line 3
