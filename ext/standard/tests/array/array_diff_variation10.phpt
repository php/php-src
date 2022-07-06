--TEST--
Test array_diff() function : usage variations - binary safe checking
--FILE--
<?php
/*
 * Test behaviour of array_diff() function with binary input
 */

echo "*** Testing array_diff() : usage variations ***\n";


$array1 = array( b"1",
                 b"hello",
                 "world",
                 "str1" => "hello",
                 "str2" => "world");

$array2 = array( b"1" => 'hello',
                 b"world",
                 "hello",
                 'test');

var_dump(array_diff($array1, $array2));
var_dump(array_diff($array2, $array1));

echo "Done";
?>
--EXPECT--
*** Testing array_diff() : usage variations ***
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [4]=>
  string(4) "test"
}
Done
