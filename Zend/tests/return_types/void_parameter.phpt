--TEST--
void return type: not valid as a parameter type
--FILE--
<?php

function foobar(void $a) {}
--EXPECTF--
Fatal error: void cannot be used as a parameter type in %s on line %d
