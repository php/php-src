--TEST--
'integer' is not a valid type hint 
--FILE--
<?php

function foobar(integer $a) {}
--EXPECTF--
Fatal error: 'integer' is not a valid type hint, use 'int' instead in %s on line %d
