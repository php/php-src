--TEST--
Test that the nullable mixed parameter type is not valid even though a null default value
--FILE--
<?php

function foo(?mixed $a = null)
{
}

?>
--EXPECTF--
Fatal error: Type mixed cannot be marked as nullable since mixed already includes null in %s on line %d
