--TEST--
Parameter type hint - Wrong parameter for integer
--FILE--
<?php

function test_int(int $a) {
	echo $a, "\n";	
}

test_int('+1');

?>
--EXPECTF--
Catchable fatal error: Argument 1 passed to test_int() must be of the type integer, string given, called in %s on line %d and defined in %s on line %d
--UEXPECTF--
Catchable fatal error: Argument 1 passed to test_int() must be of the type integer, Unicode string given, called in %s on line %d and defined in %s on line %d
