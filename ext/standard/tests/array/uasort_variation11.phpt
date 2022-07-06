--TEST--
Test uasort() function : usage variations - different associative arrays
--FILE--
<?php
/* Testing uasort() with different associative arrays having keys as
 *   string, integer, default & duplicate keys
 */

echo "*** Testing uasort() : sorting different associative arrays ***\n";

// comparison function
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
