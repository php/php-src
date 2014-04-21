--TEST--
Basic return hints yield non-object
--FILE--
<?php
function foo() : array {
	yield;
}
?>
--EXPECTF--
Fatal error: Generators may only yield objects, array is not a valid type in %s on line 3



