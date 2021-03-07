--TEST--
noreturn return type: not valid as a parameter type
--FILE--
<?php

function foobar(noreturn $a) {}
?>
--EXPECTF--
Fatal error: noreturn cannot be used as a parameter type in %s on line %d
