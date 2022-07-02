--TEST--
Return type of parent is not allowed in function
--FILE--
<?php

function test(): parent {}
?>
--EXPECTF--
Fatal error: Cannot use "parent" when no class scope is active in %s on line %d
