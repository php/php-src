--TEST--
Fully qualified (leading backslash) type names must fail
--FILE--
<?php

function foo(\array $foo) {
	var_dump($foo);
}
foo(1);

?>
--EXPECTF--
Fatal error: Cannot use the builtin type 'array' as fully qualified with a leading backslash in %s on line %d
