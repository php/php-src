--TEST--
Test uasort() function : usage variations - 'cmp_function' with reference argument 
--FILE--
<?php
/* Prototype  : bool uasort(array $array_arg, string $cmp_function)
 * Description: Sort an array with a user-defined comparison function and maintain index association 
 * Source code: ext/standard/array.c
*/

/* Testing uasort() functionality with comparison function having arguments as reference
 */

echo "*** Testing uasort() : 'cmp_function' with reference arguments ***\n";

// comparison function
/* Prototype : int cmp(mixed &$value1, mixed &$value2)
 * Parameters : $value1 and $value2 - values recieved by reference 
 * Return value : 0 - if both values are same
 *                1 - if value1 is greater than value2
 *               -1 - if value1 is less than value2
 * Description : compares value1 and value2
 */
function cmp(&$value1, &$value2)
{
  if($value1 == $value2) {
    return 0;
  }
  else if($value1 > $value2) {
    return 1;
  }
  else
    return -1;
}

// Int array with default keys
$int_values = array(1, 8, 9, 3, 2, 6, 7);
echo "-- Passing integer values to 'cmp_function' --\n";
var_dump( uasort($int_values, 'cmp') );
var_dump($int_values);

// String array with default keys
$string_values = array("Mango", "Apple", "Orange", "Banana");
echo "-- Passing string values to 'cmp_function' --\n";
var_dump( uasort($string_values, 'cmp') );
var_dump($string_values);

echo "Done"
?>
--EXPECTF--
*** Testing uasort() : 'cmp_function' with reference arguments ***
-- Passing integer values to 'cmp_function' --
bool(true)
array(7) {
  [0]=>
  int(1)
  [4]=>
  int(2)
  [3]=>
  int(3)
  [5]=>
  int(6)
  [6]=>
  int(7)
  [1]=>
  int(8)
  [2]=>
  int(9)
}
-- Passing string values to 'cmp_function' --
bool(true)
array(4) {
  [1]=>
  string(5) "Apple"
  [3]=>
  string(6) "Banana"
  [0]=>
  string(5) "Mango"
  [2]=>
  string(6) "Orange"
}
Done
