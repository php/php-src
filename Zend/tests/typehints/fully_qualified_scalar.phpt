--TEST--
Fully qualified (leading backslash) type names must fail
--FILE--
<?php

function foo(\int $foo) {
	var_dump($foo);
}
foo(1);

?>
--EXPECTF--
Fatal error: Type declaration 'int' must be unqualified in %s on line %d
