--TEST--
Test that the number parameter type can't be used together with any other numeric type
--FILE--
<?php

function foo(number|int $a)
{
}

?>
--EXPECTF--
Fatal error: Duplicate type int is redundant in %s on line %d
