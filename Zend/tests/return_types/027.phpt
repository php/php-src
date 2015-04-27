--TEST--
Return type of parent is not allowed in closure
--FILE--
<?php

$c = function(): parent {};

--EXPECTF--
Fatal error: Cannot use "parent" when no class scope is active in %s on line 3
