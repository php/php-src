--TEST--
namespace\int is not a valid type hint
--FILE--
<?php

function test(namespace\int $i) {}
test(0);

?>
--EXPECTF--
Fatal error: Scalar type declaration 'int' must be unqualified in %s on line %d
