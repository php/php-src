--TEST--
Test array_udiff_assoc() function : variation
--FILE--
<?php
echo "*** Testing array_udiff_assoc() : variation - testing with multiple array arguments ***\n";

include('compare_function.inc');
$key_compare_function = 'compare_function';

// Initialise all required variables
$arr1 = array("one" => "one", "02" => "two", '3' => "three", "four", "0.5" => 5, 6 => 6, "seven" => "07");
$arr2 = array("one" => "one", "02" => "two", '3' => "three");
$arr3 = array("four", "0.5" => "five", 6 => 6, "seven" => 7);
$arr4 = array("four", "0.5" => "five", 6 => 6, "seven" => 7);


var_dump( array_udiff_assoc($arr1, $arr2, $arr3, $arr4, $key_compare_function) );


?>
--EXPECT--
*** Testing array_udiff_assoc() : variation - testing with multiple array arguments ***
array(2) {
  [4]=>
  string(4) "four"
  ["0.5"]=>
  int(5)
}
