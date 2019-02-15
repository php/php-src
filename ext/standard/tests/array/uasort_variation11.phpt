--TEST--
Test uasort() function : usage variations - different associative arrays
--FILE--
<?php
/* Prototype  : bool uasort(array $array_arg, string $cmp_function)
 * Description: Sort an array with a user-defined comparison function and maintain index association
 * Source code: ext/standard/array.c
*/

/* Testing uasort() with different associative arrays having keys as
 *   string, integer, default & duplicate keys
 */

echo "*** Testing uasort() : sorting different associative arrays ***\n";

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
  else
    return -1;
}

// Array with duplicate string and integer keys
$array_arg = array(0 => 2, "a" => 8, "d" => 9, 3 => 3, 5 => 2, "o" => 6, "z" => -99, 0 => 1, "z" => 3);
echo "-- Array with duplicate keys --\n";
var_dump( uasort($array_arg, 'cmp') );
var_dump($array_arg);

// Array with default and assigned keys
$array_arg = array(0 => "Banana", 1 => "Mango", "Orange", 2 => "Apple", "Pineapple");
echo "-- Array with default/assigned keys --\n";
var_dump( uasort($array_arg, 'cmp') );
var_dump($array_arg);

echo "Done"
?>
--EXPECT--
*** Testing uasort() : sorting different associative arrays ***
-- Array with duplicate keys --
bool(true)
array(7) {
  [0]=>
  int(1)
  [5]=>
  int(2)
  [3]=>
  int(3)
  ["z"]=>
  int(3)
  ["o"]=>
  int(6)
  ["a"]=>
  int(8)
  ["d"]=>
  int(9)
}
-- Array with default/assigned keys --
bool(true)
array(4) {
  [2]=>
  string(5) "Apple"
  [0]=>
  string(6) "Banana"
  [1]=>
  string(5) "Mango"
  [3]=>
  string(9) "Pineapple"
}
Done
