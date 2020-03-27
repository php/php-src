--TEST--
Test that the nullable mixed return type is not valid
--FILE--
<?php

function foo(): ?mixed
{
}

?>
--EXPECTF--
Fatal error: Type mixed cannot be marked as nullable since mixed already includes null in %s on line %d
