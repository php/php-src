--TEST--
Test chunk_split() function : error conditions
--FILE--
<?php
/* Prototype  : string chunk_split(string $str [, int $chunklen [, string $ending]])
 * Description: Returns split line
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

/*
* Testing error conditions of chunk_split() with zero arguments
* and for more than expected number of arguments
*/

echo "*** Testing chunk_split() : error conditions ***\n";

// Zero arguments
echo "-- Testing chunk_split() function with Zero arguments --";
var_dump( chunk_split() );

// With one more than the expected number of arguments
$str = 'Testing chunk_split';
$chunklen = 5;
$ending = '***';
$extra_arg = 10;
echo "-- Testing chunk_split() function with more than expected no. of arguments --";
var_dump( chunk_split($str, $chunklen, $ending, $extra_arg) );

echo "Done"
?>
--EXPECTF--
*** Testing chunk_split() : error conditions ***
-- Testing chunk_split() function with Zero arguments --
Warning: chunk_split() expects at least 1 parameter, 0 given in %s on line %d
NULL
-- Testing chunk_split() function with more than expected no. of arguments --
Warning: chunk_split() expects at most 3 parameters, 4 given in %s on line %d
NULL
Done
