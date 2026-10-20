--TEST--
Destructuring with list() a value of type int
--FILE--
<?php

$v = 5;

list($a, $b) = $v;

var_dump($a, $b);

?>
--EXPECTF--
Warning: Cannot use int as array in %s on line %d

Warning: Cannot use int as array in %s on line %d
NULL
NULL
