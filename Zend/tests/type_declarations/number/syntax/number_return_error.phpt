--TEST--
Test that the number return type can't be used together with any other numeric type
--FILE--
<?php

function foo(): number|float|null
{
}

?>
--EXPECTF--
Fatal error: Duplicate type float is redundant in %s on line %d
