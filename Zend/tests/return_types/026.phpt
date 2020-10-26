--TEST--
Return type of parent is not allowed in function
--FILE--
<?php

function test(): parent {}
?>
--EXPECTF--
Fatal error: "parent" must be used only in a class scope in %s on line %d
