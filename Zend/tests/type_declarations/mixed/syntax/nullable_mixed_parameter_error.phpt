--TEST--
Test that the nullable mixed parameter type is not valid
--FILE--
<?php

function foo(?mixed $a)
{
}

?>
--EXPECTF--
Fatal error: Type mixed cannot be marked as nullable since mixed already includes null in %s on line %d
