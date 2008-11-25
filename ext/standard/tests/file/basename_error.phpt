--TEST--
Test basename() function : error conditions 
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype  : string basename(string path [, string suffix])
 * Description: Returns the filename component of the path 
 * Source code: ext/standard/string.c
 * Alias to functions: 
 */

echo "*** Testing basename() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing basename() function with Zero arguments --\n";
var_dump( basename() );

//Test basename with one more than the expected number of arguments
echo "\n-- Testing basename() function with more than expected no. of arguments --\n";
$path = 'string_val';
$suffix = 'string_val';
$extra_arg = 10;
var_dump( basename($path, $suffix, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing basename() : error conditions ***

-- Testing basename() function with Zero arguments --

Warning: basename() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing basename() function with more than expected no. of arguments --

Warning: basename() expects at most 2 parameters, 3 given in %s on line %d
NULL
===DONE===
