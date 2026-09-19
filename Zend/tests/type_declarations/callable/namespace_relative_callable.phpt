--TEST--
namespace\callable is not a valid type declaration
--FILE--
<?php

function test($i): namespace\callable {}

?>
--EXPECTF--
Fatal error: Type declaration 'callable' must be unqualified in %s on line %d
