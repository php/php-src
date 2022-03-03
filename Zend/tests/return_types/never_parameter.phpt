--TEST--
never return type: not valid as a parameter type
--FILE--
<?php

function foobar(never $a) {}
?>
--EXPECTF--
Fatal error: never cannot be used as a parameter type in %s on line %d
