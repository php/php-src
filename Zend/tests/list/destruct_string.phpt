--TEST--
Destructuring with list() a value of type string
--FILE--
<?php

$v = 'hi';

list($a, $b, $c) = $v;

var_dump($a, $b, $c);

?>
--EXPECTF--
Warning: Cannot use string as array in %s on line %d

Warning: Cannot use string as array in %s on line %d

Warning: Cannot use string as array in %s on line %d
NULL
NULL
NULL
