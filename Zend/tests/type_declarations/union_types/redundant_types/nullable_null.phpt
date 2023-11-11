--TEST--
Combining nullability with null
--FILE--
<?php

function test(): ?null {
}

?>
--EXPECTF--
Fatal error: null cannot be marked as nullable in %s on line %d
