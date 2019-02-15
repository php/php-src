--TEST--
Test get_declared_classes() function : error conditions
--FILE--
<?php
/* Prototype  : proto array get_declared_classes()
 * Description: Returns an array of all declared classes.
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions:
 */

echo "*** Testing get_declared_classes() : error conditions ***\n";

// One argument
echo "\n-- Testing get_declared_classes() function with one argument --\n";
$extra_arg = 10;
var_dump( get_declared_classes($extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing get_declared_classes() : error conditions ***

-- Testing get_declared_classes() function with one argument --

Warning: get_declared_classes() expects exactly 0 parameters, 1 given in %s on line 13
NULL
Done
