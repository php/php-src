--TEST--
namespace\array is not a valid type declaration
--FILE--
<?php

function test(namespace\array $i) {}
test(0);

?>
--EXPECTF--
Fatal error: Type declaration 'array' must be unqualified in %s on line %d
