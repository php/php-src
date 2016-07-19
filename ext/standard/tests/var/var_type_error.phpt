--TEST--
var_type() argument definition and parsing
--FILE--
<?php

var_dump(var_type());

var_dump(var_type('1', '2'));

?>
--EXPECTF--
Warning: var_type() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: var_type() expects exactly 1 parameter, 2 given in %s on line %d
NULL
