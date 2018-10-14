--TEST--
Test array_filter() function : basic functionality
--FILE--
<?php
/* Prototype  : array array_filter(array $input [, callback $callback])
 * Description: Filters elements from the array via the callback.
 * Source code: ext/standard/array.c
*/


echo "*** Testing array_filter() : basic functionality ***\n";


// Initialise all required variables
$input = array(1, 2, 3, 0, -1);  // 0 will be considered as FALSE and removed in default callback

/* Callback function
 * Prototype : bool even(array $input)
 * Parameters : $input - input array each element of which will be checked in function even()
 * Return type : boolean - true if element is even and false otherwise
 * Description : This function takes array as parameter and checks for each element of array.
 *              It returns true if the element is even number else returns false
 */
function even($input)
{
  return ($input % 2 == 0);
}

// with all possible arguments
var_dump( array_filter($input,"even") );

// with default arguments
var_dump( array_filter($input) );

echo "Done"
?>
--EXPECTF--
*** Testing array_filter() : basic functionality ***
array(2) {
  [1]=>
  int(2)
  [3]=>
  int(0)
}
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [4]=>
  int(-1)
}
Done
