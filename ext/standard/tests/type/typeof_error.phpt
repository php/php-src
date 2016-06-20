--TEST--
typeof() argument definition and parsing
--FILE--
<?php

var_dump(typeof());

var_dump(typeof('1', '2', '3'));

var_dump(typeof('1', []));

?>
--EXPECTF--
Warning: typeof() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: typeof() expects at most 2 parameters, 3 given in %s on line %d
NULL

Warning: typeof() expects parameter 2 to be boolean, array given in %s on line %d
NULL
