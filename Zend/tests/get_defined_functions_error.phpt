--TEST--
Test get_defined_functions() function : error conditions 
--FILE--
<?php

/* Prototype  : array get_defined_functions  ( void  )
 * Description: Gets an array of all defined functions.
 * Source code: Zend/zend_builtin_functions.c
*/


echo "*** Testing get_defined_functions() : error conditions ***\n";


echo "\n-- Testing get_defined_functions() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( get_defined_functions($extra_arg) );

?> 
===Done===
--EXPECTF--
*** Testing get_defined_functions() : error conditions ***

-- Testing get_defined_functions() function with more than expected no. of arguments --

Warning: get_defined_functions() expects exactly 0 parameters, 1 given in %s on line %d
NULL
 
===Done===
