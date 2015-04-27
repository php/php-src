--TEST--
Return type of self is not allowed in closure

--FILE--
<?php

$c = function(): self {};

--EXPECTF--
Fatal error: Cannot use "self" when no class scope is active in %s on line 3
