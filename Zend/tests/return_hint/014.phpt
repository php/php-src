--TEST--
Basic return hints no yield
--FILE--
<?php
function foo() : array {
	yield;
}
?>
--EXPECTF--
Fatal error: The "yield" expression can not be used inside a function with a return type hint in %s on line %d


