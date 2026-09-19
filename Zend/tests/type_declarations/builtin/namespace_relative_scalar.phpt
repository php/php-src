--TEST--
namespace\int is not a valid type declaration
--FILE--
<?php

function test(namespace\int $i) {}
test(0);

?>
--EXPECTF--
Fatal error: Type declaration 'int' must be unqualified in %s on line %d
