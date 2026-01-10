--TEST--
Destructuring with list() a value of type resource
--FILE--
<?php

$v = STDERR;

list($a, $b) = $v;

var_dump($a, $b);

?>
--EXPECTF--
Warning: Cannot use resource as array in %s on line %d

Warning: Cannot use resource as array in %s on line %d
NULL
NULL
