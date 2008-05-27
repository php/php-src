--TEST--
Test join() function: error conditions
--FILE--
<?php
/* Prototype  : string join( string $glue, array $pieces )
 * Description: Join array elements with a string
 * Source code: ext/standard/string.c
 * Alias of function: implode()
*/

echo "*** Testing join() : error conditions ***\n";

// Zero argument
echo "\n-- Testing join() function with Zero arguments --\n";
var_dump( join() );

// More than expected number of arguments
echo "\n-- Testing join() function with more than expected no. of arguments --\n";
$glue = 'string_val';
$pieces = array(1, 2);
$extra_arg = 10;

var_dump( join($glue, $pieces, $extra_arg) );

// Less than expected number of arguments 
echo "\n-- Testing join() with less than expected no. of arguments --\n";
$glue = 'string_val';
 
var_dump( join($glue));

echo "Done\n";
?>
--EXPECTF--
*** Testing join() : error conditions ***

-- Testing join() function with Zero arguments --

Warning: Wrong parameter count for join() in %s on line %d
NULL

-- Testing join() function with more than expected no. of arguments --

Warning: Wrong parameter count for join() in %s on line %d
NULL

-- Testing join() with less than expected no. of arguments --

Warning: join(): Argument must be an array in %s on line %d
NULL
Done
