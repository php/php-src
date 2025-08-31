--TEST--
Destructuring with list() a value of type bool
--FILE--
<?php

$v = true;

list($a, $b) = $v;

var_dump($a, $b);

?>
--EXPECTF--
Warning: Cannot use bool as array in %s on line %d

Warning: Cannot use bool as array in %s on line %d
NULL
NULL
