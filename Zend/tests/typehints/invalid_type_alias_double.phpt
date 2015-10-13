--TEST--
'double' is not a valid type hint 
--FILE--
<?php

function foobar(double $a) {}
--EXPECTF--
Fatal error: 'double' is not a valid type hint, use 'float' instead in %s on line %d
