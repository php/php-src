--TEST--
Test that the mixed parameter type can't be used together with any other type
--FILE--
<?php

function foo(mixed|int|null $a)
{
}

?>
--EXPECTF--
Fatal error: Type mixed can only be used as a standalone type in %s on line %d
