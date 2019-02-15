--TEST--
Test get_loaded_extensions() function : error conditions
--FILE--
<?php
/* Prototype  : array get_loaded_extensions  ([ bool $zend_extensions= false  ] )
 * Description:  Returns an array with the names of all modules compiled and loaded
 * Source code: Zend/zend_builtin_functions.c
 */

echo "*** Testing get_loaded_extensions() : error conditions ***\n";

echo "\n-- Testing get_loaded_extensions() function with more than expected no. of arguments --\n";
$res = fopen(__FILE__, "r");
$extra_arg = 10;
var_dump( get_resource_type(true, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing get_loaded_extensions() : error conditions ***

-- Testing get_loaded_extensions() function with more than expected no. of arguments --

Warning: get_resource_type() expects exactly 1 parameter, 2 given in %s on line %d
NULL
===DONE===
