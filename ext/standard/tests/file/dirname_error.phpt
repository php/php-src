--TEST--
Test dirname() function : error conditions 
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype  : string dirname(string path)
 * Description: Returns the directory name component of the path 
 * Source code: ext/standard/string.c
 * Alias to functions: 
 */

echo "*** Testing dirname() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing dirname() function with Zero arguments --\n";
var_dump( dirname() );

//Test dirname with one more than the expected number of arguments
echo "\n-- Testing dirname() function with more than expected no. of arguments --\n";
$path = 'string_val';
$extra_arg = 10;
var_dump( dirname($path, 1, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing dirname() : error conditions ***

-- Testing dirname() function with Zero arguments --

Warning: dirname() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing dirname() function with more than expected no. of arguments --

Warning: dirname() expects at most 2 parameters, 3 given in %s on line %d
NULL
===DONE===

