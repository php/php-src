--TEST--
Test that the number|void return type is not valid
--FILE--
<?php

function foo(): number|void
{
}

?>
--EXPECTF--
Fatal error: Void can only be used as a standalone type in %s on line %d
