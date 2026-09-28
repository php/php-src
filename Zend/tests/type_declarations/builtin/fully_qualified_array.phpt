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
Fatal error: Type declaration 'array' must be unqualified in %s on line %d
