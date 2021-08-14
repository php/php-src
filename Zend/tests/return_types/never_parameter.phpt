--TEST--
never return type: valid as a parameter type
--FILE--
<?php

function foobar(never $a) {}

echo "OK!";
?>
--EXPECTF--
OK!
