--TEST--
Test that the mixed|void return type is not valid
--FILE--
<?php

function foo(): mixed|void
{
    return null;
}

?>
--EXPECTF--
Fatal error: Type mixed can only be used as a standalone type in %s on line %d
