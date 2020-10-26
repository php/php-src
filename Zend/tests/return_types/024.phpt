--TEST--
Return type of self is not allowed in function
--FILE--
<?php

function test(): self {}
?>
--EXPECTF--
Fatal error: "self" must be used only in a class scope in %s on line %d
