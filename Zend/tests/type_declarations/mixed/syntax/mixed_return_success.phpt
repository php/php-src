--TEST--
Test that mixed is a valid return type
--FILE--
<?php

function foo(): mixed
{
    return null;
}

?>
--EXPECT--
