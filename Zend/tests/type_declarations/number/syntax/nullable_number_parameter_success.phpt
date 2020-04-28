--TEST--
Test that the nullable number parameter type is valid
--FILE--
<?php

function foo(?number $a)
{
}

?>
--EXPECTF--
