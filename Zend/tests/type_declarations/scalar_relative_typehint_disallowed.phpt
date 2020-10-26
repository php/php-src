--TEST--
Scalar type - disallow relative types
--FILE--
<?php

function foo(bar\int $a): int {
    return $a;
}

foo(10);

?>
--EXPECTF--
Fatal error: Class "bar\int" cannot be declared, bar\int is a reserved class name in %s on line %d
