--TEST--
Test natcasesort() function : usage variations - octal values
--FILE--
<?php
/*
 * Pass an array of octal values to test how natcasesort() re-orders it
 */

echo "*** Testing natcasesort() : usage variation ***\n";

$unsorted_oct_array = array(01235, 0321, 0345, 066, 0772, 077, -066, -0345, 0);

var_dump( natcasesort($unsorted_oct_array) );
var_dump($unsorted_oct_array);

echo "Done";
?>
--EXPECT--
*** Testing natcasesort() : usage variation ***
bool(true)
array(9) {
  [6]=>
  int(-54)
  [7]=>
  int(-229)
  [8]=>
  int(0)
  [3]=>
  int(54)
  [5]=>
  int(63)
  [1]=>
  int(209)
  [2]=>
  int(229)
  [4]=>
  int(506)
  [0]=>
  int(669)
}
Done
