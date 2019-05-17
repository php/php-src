--TEST--
Return type of self is not allowed in function
--FILE--
<?php

function test(): self {}
--EXPECTF--
Fatal error: Cannot use "self" when no class scope is active in %s on line 3
