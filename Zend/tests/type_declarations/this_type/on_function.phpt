--TEST--
$this on global function
--FILE--
<?php

function test(): $this {
}

?>
--EXPECTF--
Fatal error: Cannot use "$this" type when no class scope is active in %s on line %d
