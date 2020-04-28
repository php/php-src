--TEST--
Test that the nullable number return type is valid
--FILE--
<?php

function foo(): ?number
{
}

?>
--EXPECT--
