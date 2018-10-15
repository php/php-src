--TEST--
Test file_exists() function : error conditions
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype  : proto bool file_exists(string filename)
 * Description: Returns true if filename exists
 * Source code: ext/standard/filestat.c
 * Alias to functions:
 */

echo "*** Testing file_exists() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing file_exists() function with Zero arguments --\n";
var_dump( file_exists() );

//Test file_exists with one more than the expected number of arguments
echo "\n-- Testing file_exists() function with more than expected no. of arguments --\n";
$filename = 'string_val';
$extra_arg = 10;
var_dump( file_exists($filename, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing file_exists() : error conditions ***

-- Testing file_exists() function with Zero arguments --

Warning: file_exists() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing file_exists() function with more than expected no. of arguments --

Warning: file_exists() expects exactly 1 parameter, 2 given in %s on line %d
NULL
Done
