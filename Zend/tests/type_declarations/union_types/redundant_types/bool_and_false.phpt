--TEST--
Using both bool and false in a union
--FILE--
<?php

function test(): bool|false {
}

?>
--EXPECTF--
Fatal error: Duplicate type false is redundant in %s on line %d
