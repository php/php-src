--TEST--
Cannot use static type outside a class/trait: global function
--FILE--
<?php

function foo($x): static {};

?>
--EXPECTF--
Fatal error: Cannot use "static" when no class scope is active in %s on line %d
