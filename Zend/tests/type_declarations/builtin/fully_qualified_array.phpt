--TEST--
Fully qualified (leading backslash) array type name must fail
--FILE--
<?php

function foo(\array $foo) {
    var_dump($foo);
}
foo(1);

?>
--EXPECTF--
Fatal error: Cannot use "array" as a type name as it is reserved in %s on line %d
