--TEST--
Test uasort() function : basic functionality - duplicate values
--FILE--
<?php
echo "*** Testing uasort() : basic functionality with duplicate values ***\n";

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

// increasing values
$int_values1 = array(1, 1, 2, 2, 3, 3);
echo "-- Numeric array with increasing values --\n";
var_dump( uasort($int_values1, 'cmp') );
var_dump($int_values1);

// decreasing values
$int_values2 = array(3, 3, 2, 2, 1, 1);
echo "-- Numeric array with decreasing values --\n";
var_dump( uasort($int_values2, 'cmp') );
var_dump($int_values2);

// increasing and decreasing values
$int_values3 = array(1, 2, 3, 3, 2, 1);
echo "-- Numeric array with increasing and decreasing values --\n";
var_dump( uasort($int_values3, 'cmp') );
var_dump($int_values3);

echo "Done"
?>
--EXPECT--
*** Testing uasort() : basic functionality with duplicate values ***
-- Numeric array with increasing values --
bool(true)
array(6) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(2)
  [4]=>
  int(3)
  [5]=>
  int(3)
}
-- Numeric array with decreasing values --
bool(true)
array(6) {
  [4]=>
  int(1)
  [5]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(2)
  [0]=>
  int(3)
  [1]=>
  int(3)
}
-- Numeric array with increasing and decreasing values --
bool(true)
array(6) {
  [0]=>
  int(1)
  [5]=>
  int(1)
  [1]=>
  int(2)
  [4]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(3)
}
Done
