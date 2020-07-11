--TEST--
Test array_merge_recursive() function : basic functionality - array with default keys
--FILE--
<?php
echo "*** Testing array_merge_recursive() : array with default keys ***\n";

// Initialise the arrays
$arr1 = array(1, array(1, 2));
$arr2 = array(3, array("hello", 'world'));
$arr3 = array(array(6, 7), array("str1", 'str2'));

// Calling array_merge_recursive() without arguments
echo "-- Without arguments --\n";
var_dump( array_merge_recursive() );

// Calling array_merge_recursive() with default arguments
echo "-- With default argument --\n";
var_dump( array_merge_recursive($arr1) );

// Calling array_merge_recursive() with more arguments
echo "-- With more arguments --\n";
var_dump( array_merge_recursive($arr1,$arr2) );
var_dump( array_merge_recursive($arr1,$arr2,$arr3) );

echo "Done";
?>
--EXPECT--
*** Testing array_merge_recursive() : array with default keys ***
-- Without arguments --
array(0) {
}
-- With default argument --
array(2) {
  [0]=>
  int(1)
  [1]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}
-- With more arguments --
array(4) {
  [0]=>
  int(1)
  [1]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [2]=>
  int(3)
  [3]=>
  array(2) {
    [0]=>
    string(5) "hello"
    [1]=>
    string(5) "world"
  }
}
array(6) {
  [0]=>
  int(1)
  [1]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [2]=>
  int(3)
  [3]=>
  array(2) {
    [0]=>
    string(5) "hello"
    [1]=>
    string(5) "world"
  }
  [4]=>
  array(2) {
    [0]=>
    int(6)
    [1]=>
    int(7)
  }
  [5]=>
  array(2) {
    [0]=>
    string(4) "str1"
    [1]=>
    string(4) "str2"
  }
}
Done
