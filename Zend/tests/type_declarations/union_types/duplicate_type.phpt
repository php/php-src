--TEST--
Using a type twice in a union
--FILE--
<?php

function test(): int|INT {
}

?>
--EXPECTF--
Fatal error: Type int is redundant in %s on line %d
