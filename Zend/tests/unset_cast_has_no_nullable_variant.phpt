--TEST--
There is no (?unset) nullable cast
--FILE--
<?php

$x = 1;
var_dump((?unset) $x);
var_dump($x);

?>
--EXPECTF--
Parse error: syntax error, unexpected '?' in %s on line %d
