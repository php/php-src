--TEST--
Scalar type hint - default via constants - error condition
--FILE--
<?php

const STRING_VAL = "test";

function int_val(int $a = STRING_VAL): int {
	return $a;
}

var_dump(int_val());

?>
--EXPECTF--
Fatal error: Argument 1 passed to int_val() must be of the type integer, string given, called in %s on line %d and defined in %s on line %d