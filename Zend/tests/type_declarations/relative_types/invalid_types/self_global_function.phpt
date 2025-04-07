--TEST--
Cannot use self type outside a class/trait: global function
--FILE--
<?php

function foo($x): self {};

?>
--EXPECTF--
Fatal error: Cannot use "self" when no class scope is active in %s on line %d
