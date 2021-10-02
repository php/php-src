--TEST--
Combining nullability with null
--FILE--
<?php

function test(): ?null {
}

?>
--EXPECTF--
Fatal error: Null cannot be used as a standalone type in %s on line %d
