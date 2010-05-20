--TEST--
Parameter type hint - Testing in function inside function
--FILE--
<?php

function a() {
	function b(real $c) {
	}
	return b(1);
}

a();

?>
--EXPECTF--
Catchable fatal error: Argument 1 passed to b() must be of the type double, integer given, called in %s on line %d and defined in %s on line %d
