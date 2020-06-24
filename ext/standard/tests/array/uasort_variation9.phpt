--TEST--
Test uasort() function : usage variations - 'cmp_function' with reference argument
--FILE--
<?php
/* Testing uasort() functionality with comparison function having arguments as reference
 */

echo "*** Testing uasort() : 'cmp_function' with reference arguments ***\n";

// comparison function
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
--EXPECT--
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
