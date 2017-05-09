--TEST--
'boolean' is not a valid type hint 
--FILE--
<?php

function foobar(boolean $a) {}
--EXPECTF--
Fatal error: 'boolean' is not a valid type hint, use 'bool' instead in %s on line %d
