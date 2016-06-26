--TEST--
var_info() argument definition and parsing
--FILE--
<?php

var_dump(var_info());

var_dump(var_info('1', '2'));

?>
--EXPECTF--
Warning: var_info() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: var_info() expects exactly 1 parameter, 2 given in %s on line %d
NULL
