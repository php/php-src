--TEST--
Parameter type hint - Wrong parameter for double
--FILE--
<?php

function test_double(real $a) {
	echo $a, "\n";	
}

test_double('+1.1');

?>
--EXPECTF--
Catchable fatal error: Argument 1 passed to test_double() must be of the type double, string given, called in %s on line %d and defined in %s on line %d
--UEXPECTF--
Catchable fatal error: Argument 1 passed to test_double() must be of the type double, Unicode string given, called in %s on line %d and defined in %s on line %d
