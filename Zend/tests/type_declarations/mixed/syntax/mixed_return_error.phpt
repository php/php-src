--TEST--
Test that the mixed return type can't be used together with any other type
--FILE--
<?php

function foo(): mixed|string|null
{
    return null;
}

?>
--EXPECTF--
Fatal error: Type mixed can only be used as a standalone type in %s on line %d
