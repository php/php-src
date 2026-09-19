--TEST--
namespace\callable is not a valid type declaration
--FILE--
<?php

function test($i): namespace\callable {}

?>
--EXPECTF--
Fatal error: Cannot use "callable" as a type name as it is reserved in %s on line %d
