--TEST--
Test get_resource_type() function : error conditions
--FILE--
<?php
/* Prototype  : string get_resource_type  ( resource $handle  )
 * Description:  Returns the resource type
 * Source code: Zend/zend_builtin_functions.c
 */

echo "*** Testing get_resource_type() : error conditions ***\n";

echo "\n-- Testing get_resource_type() function with Zero arguments --\n";
var_dump( get_resource_type() );

echo "\n-- Testing get_resource_type() function with more than expected no. of arguments --\n";
$res = fopen(__FILE__, "r");
$extra_arg = 10;
var_dump( get_resource_type($res, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing get_resource_type() : error conditions ***

-- Testing get_resource_type() function with Zero arguments --

Warning: get_resource_type() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing get_resource_type() function with more than expected no. of arguments --

Warning: get_resource_type() expects exactly 1 parameter, 2 given in %s on line %d
NULL
===DONE===
