--TEST--
Using both bool and true in a union
--FILE--
<?php

function test(): bool|true {
}

?>
--EXPECTF--
Fatal error: Duplicate type true is redundant in %s on line %d
