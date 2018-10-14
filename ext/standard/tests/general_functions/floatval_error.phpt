--TEST--
Testing floatval() and its alias doubleval() : error conditions -  wrong numbers of parametersns
--FILE--
<?php
/* Prototype: float floatval( mixed $var );
 * Description: Returns the float value of var.
 */

echo "*** Testing floatval() and doubleval() : error conditions ***\n";


echo "\n-- Testing floatval() and doubleval() function with no arguments --\n";
var_dump( floatval() );
var_dump( doubleval() );

echo "\n-- Testing floatval() and doubleval() function with more than expected no. of arguments --\n";
var_dump( floatval(10.5, FALSE) );
var_dump( doubleval(10.5, FALSE) );

?>
===DONE===
--EXPECTF--
*** Testing floatval() and doubleval() : error conditions ***

-- Testing floatval() and doubleval() function with no arguments --

Warning: floatval() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: doubleval() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing floatval() and doubleval() function with more than expected no. of arguments --

Warning: floatval() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: doubleval() expects exactly 1 parameter, 2 given in %s on line %d
NULL
===DONE===
