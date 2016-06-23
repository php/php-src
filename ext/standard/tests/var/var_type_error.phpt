--TEST--
var_type() argument definition and parsing
--FILE--
<?php

var_dump(var_type());

var_dump(var_type('1', '2'));

?>
--EXPECTF--
Warning: typeof() expects at exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: typeof() expects at exactly 1 parameters, 2 given in %s on line %d
NULL
