--TEST--
Test array_filter() function : error conditions 
--FILE--
<?php
/* Prototype  : array array_filter(array $input [, callback $callback])
 * Description: Filters elements from the array via the callback. 
 * Source code: ext/standard/array.c
*/

echo "*** Testing array_filter() : error conditions ***\n";

// zero arguments
echo "-- Testing array_filter() function with Zero arguments --";
var_dump( array_filter() );

$input = array(0, 1, 2, 3, 5);
/*  callback function
 *  Prototype : bool odd(array $input)
 *  Parameters : $input - array for which each elements should be checked into the function
 *  Return Type : bool - true if element is odd and returns false otherwise
 *  Description : Function takes array as input and checks for its each elements.
*/
function odd($input)
{
  return ($input % 2 != 0);
}
$extra_arg = 10;

// with one more than the expected number of arguments
echo "-- Testing array_filter() function with more than expected no. of arguments --";
var_dump( array_filter($input, "odd", $extra_arg) );

// with incorrect callback function
echo "-- Testing array_filter() function with incorrect callback --";
var_dump( array_filter($input, "even") );

echo "Done"
?>
--EXPECTF--
*** Testing array_filter() : error conditions ***
-- Testing array_filter() function with Zero arguments --
Warning: Wrong parameter count for array_filter() in %s on line %d
NULL
-- Testing array_filter() function with more than expected no. of arguments --
Warning: Wrong parameter count for array_filter() in %s on line %d
NULL
-- Testing array_filter() function with incorrect callback --
Warning: array_filter(): The second argument, 'even', should be a valid callback in %s on line %d
NULL
Done
