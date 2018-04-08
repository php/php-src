--TEST--
"iterable" must not be fully qualified
--FILE--
<?php

function foo(): \iterable {
	return [];
}
var_dump(foo());

?>
--EXPECTF--
Fatal error: Type declaration 'iterable' must be unqualified in %s on line %d
