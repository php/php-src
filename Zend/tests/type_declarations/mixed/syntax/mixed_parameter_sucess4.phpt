--TEST--
Test that the mixed parameter type can have any default value
--FILE--
<?php

function foo(mixed $a = null, mixed $b = false, mixed $c = 1, mixed $d = 3.13, mixed $e = "", mixed $f = [])
{
}

?>
--EXPECT--
