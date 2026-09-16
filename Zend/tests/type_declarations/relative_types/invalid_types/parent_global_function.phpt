--TEST--
Cannot use parent type outside a class/trait: global function
--FILE--
<?php

function foo($x): parent {};

?>
--EXPECTF--
Fatal error: Cannot use "parent" when no class scope is active in %s on line %d
