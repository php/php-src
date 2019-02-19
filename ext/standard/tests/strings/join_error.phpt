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

// Less than expected number of arguments
echo "\n-- Testing join() with less than expected no. of arguments --\n";
$glue = 'string_val';

var_dump( join($glue));

echo "Done\n";
?>
--EXPECTF--
*** Testing join() : error conditions ***

-- Testing join() with less than expected no. of arguments --

Warning: join(): Argument must be an array in %s on line %d
NULL
Done
