--TEST--
namespace\array is not a valid type declaration
--FILE--
<?php

function test(namespace\array $i) {}
test(0);

?>
--EXPECTF--
Fatal error: Cannot use "array" as a type name as it is reserved in %s on line %d
