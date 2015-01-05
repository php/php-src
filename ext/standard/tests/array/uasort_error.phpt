--TEST--
Test uasort() function : error conditions 
--FILE--
<?php
/* Prototype  : bool uasort(array $array_arg, string $cmp_function)
 * Description: Sort an array with a user-defined comparison function and maintain index association 
 * Source code: ext/standard/array.c
*/

echo "*** Testing uasort() : error conditions ***\n";

// comparison function 
/* Prototype : int cmp(mixed $value1, mixed $value2)
 * Parameters : $value1 and $value2 - values to be compared
 * Return value : 0 - if both values are same
 *                1 - if value1 is greater than value2
 *               -1 - if value1 is less than value2
 * Description : compares value1 and value2
 */
function cmp($value1, $value2)
{
  if($value1 == $value2) {
    return 0;
  }
  else if($value1 > $value2) {
    return 1;
  }
  else {
    return -1;
  }
}

// Initialize 'array_arg' 
$array_arg = array(0 => 1, 1 => 10, 2 => 'string', 3 => 3, 4 => 2, 5 => 100, 6 => 25);

// With zero arguments
echo "-- Testing uasort() function with Zero argument --\n";
var_dump( uasort() );

// With one more than the expected number of arguments
echo "-- Testing uasort() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( uasort($array_arg, 'cmp', $extra_arg) );

// With one less than the expected number of arguments
echo "-- Testing uasort() function with less than expected no. of arguments --\n";
var_dump( uasort($array_arg) );

// With non existent comparison function
echo "-- Testing uasort() function with non-existent compare function --\n";
var_dump( uasort($array_arg, 'non_existent') );

// With non existent comparison function and extra argument
echo "-- Testing uasort() function with non-existent compare function and extra argument --\n";
var_dump( uasort($array_arg, 'non_existent', $extra_arg) );

echo "Done"
?>
--EXPECTF--
*** Testing uasort() : error conditions ***
-- Testing uasort() function with Zero argument --

Warning: uasort() expects exactly 2 parameters, 0 given in %s on line %d
NULL
-- Testing uasort() function with more than expected no. of arguments --

Warning: uasort() expects exactly 2 parameters, 3 given in %s on line %d
NULL
-- Testing uasort() function with less than expected no. of arguments --

Warning: uasort() expects exactly 2 parameters, 1 given in %s on line %d
NULL
-- Testing uasort() function with non-existent compare function --

Warning: uasort() expects parameter 2 to be a valid callback, function 'non_existent' not found or invalid function name in %s on line %d
NULL
-- Testing uasort() function with non-existent compare function and extra argument --

Warning: uasort() expects exactly 2 parameters, 3 given in %s on line %d
NULL
Done
