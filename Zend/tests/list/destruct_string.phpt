--TEST--
Destructuring with list() a value of type string
--FILE--
<?php

$v = 'hello';

list($a, $b) = $v;

var_dump($a, $b);

?>
--EXPECTF--
Warning: Cannot use string as array in %s on line %d

Warning: Cannot use string as array in %s on line %d
NULL
NULL
