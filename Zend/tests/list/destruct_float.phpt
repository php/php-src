--TEST--
Destructuring with list() a value of type float
--FILE--
<?php

$v = 25.64;

list($a, $b) = $v;

var_dump($a, $b);

?>
--EXPECTF--
Warning: Cannot use float as array in %s on line %d

Warning: Cannot use float as array in %s on line %d
NULL
NULL
