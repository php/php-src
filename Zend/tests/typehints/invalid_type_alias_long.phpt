--TEST--
'long' is not a valid type hint 
--FILE--
<?php

function foobar(long $a) {}
--EXPECTF--
Fatal error: 'long' is not a valid type hint, use 'int' instead in %s on line %d
