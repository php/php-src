--TEST--
Test get_declared_traits() function : error conditions 
--FILE--
<?php
/* Prototype  : proto array get_declared_traits()
 * Description: Returns an array of all declared traits. 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */

echo "*** Testing get_declared_traits() : error conditions ***\n";

// One argument
echo "\n-- Testing get_declared_traits() function with one argument --\n";
$extra_arg = 10;;
var_dump( get_declared_traits($extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing get_declared_traits() : error conditions ***

-- Testing get_declared_traits() function with one argument --

Warning: get_declared_traits() expects exactly 0 parameters, 1 given in %s on line 13
NULL
Done
