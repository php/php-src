--TEST--
Test each() function : error conditions - pass incorrect number of args
--FILE--
<?php
/* Prototype  : array each(array $arr)
 * Description: Return the currently pointed key..value pair in the passed array,
 * and advance the pointer to the next element
 * Source code: Zend/zend_builtin_functions.c
 */

/*
 * Pass an incorrect number of arguments to each() to test behaviour
 */

echo "*** Testing each() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing each() function with Zero arguments --\n";
var_dump( each() );

//Test each with one more than the expected number of arguments
echo "\n-- Testing each() function with more than expected no. of arguments --\n";
$arr = array(1, 2);
$extra_arg = 10;
var_dump( each($arr, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing each() : error conditions ***

-- Testing each() function with Zero arguments --

Warning: each() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing each() function with more than expected no. of arguments --

Warning: each() expects exactly 1 parameter, 2 given in %s on line %d
NULL
Done
