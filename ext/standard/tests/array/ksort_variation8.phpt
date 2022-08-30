--TEST--
Test ksort() function : usage variations - sort mixed values, 'sort_flags' as default/SORT_REGULAR (OK to fail as result is unpredectable)
--FILE--
<?php
/*
 * testing ksort() by providing array of mixed values for $array argument with following flag values:
 *  1.flag value as default
 *  2.SORT_REGULAR - compare items normally
*/

echo "*** Testing ksort() : usage variations ***\n";

// mixed value array with different key values
$mixed_values = array (
  "array1" => array(),
  "array2" => array ( "sub_array[2,1]" => array(33,-5,6), "sub_array[2,2]" => array(11),
                      "sub_array[2,3]" => array(22,-55), "sub_array[2,4]" => array()
                    ),
  4 => 4, "4" => "4", 4 => 4.01, "b" => "b", "5" => "5", -2 => -2, -2 => -2.01,
  -2 => -2.98989, "-.9" => "-.9", "True" => "True", "" =>  "", NULL => NULL,
  "ab" => "ab", "abcd" => "abcd", 0 => 0.01, -0 => -0, '' => '' ,
  "abcd\x00abcd\x00abcd" => "abcd\x00abcd\x00abcd", 0 => 0.001
);

echo "\n-- Testing ksort() by supplying mixed value array, 'flag' value is default --\n";
$temp_array = $mixed_values;
var_dump( ksort($temp_array) );
var_dump($temp_array);

echo "\n-- Testing ksort() by supplying mixed value array, 'flag' value is SORT_REGULAR --\n";
$temp_array = $mixed_values;
var_dump( ksort($temp_array, SORT_REGULAR) );
var_dump($temp_array);

echo "Done\n";
?>
--EXPECTF--
*** Testing ksort() : usage variations ***

-- Testing ksort() by supplying mixed value array, 'flag' value is default --
bool(true)
array(13) {
  [-2]=>
  float(-2.98989)
  [""]=>
  string(0) ""
  ["-.9"]=>
  string(3) "-.9"
  ["True"]=>
  string(4) "True"
  ["ab"]=>
  string(2) "ab"
  ["abcd"]=>
  string(4) "abcd"
  ["array1"]=>
  array(0) {
  }
  ["array2"]=>
  array(4) {
    ["sub_array[2,1]"]=>
    array(3) {
      [0]=>
      int(33)
      [1]=>
      int(-5)
      [2]=>
      int(6)
    }
    ["sub_array[2,2]"]=>
    array(1) {
      [0]=>
      int(11)
    }
    ["sub_array[2,3]"]=>
    array(2) {
      [0]=>
      int(22)
      [1]=>
      int(-55)
    }
    ["sub_array[2,4]"]=>
    array(0) {
    }
  }
  ["b"]=>
  string(1) "b"
  [0]=>
  float(0.001)
  ["abcd%0abcd%0abcd"]=>
  string(14) "abcd%0abcd%0abcd"
  [4]=>
  float(4.01)
  [5]=>
  string(1) "5"
}

-- Testing ksort() by supplying mixed value array, 'flag' value is SORT_REGULAR --
bool(true)
array(13) {
  [-2]=>
  float(-2.98989)
  [""]=>
  string(0) ""
  ["-.9"]=>
  string(3) "-.9"
  ["True"]=>
  string(4) "True"
  ["ab"]=>
  string(2) "ab"
  ["abcd"]=>
  string(4) "abcd"
  ["array1"]=>
  array(0) {
  }
  ["array2"]=>
  array(4) {
    ["sub_array[2,1]"]=>
    array(3) {
      [0]=>
      int(33)
      [1]=>
      int(-5)
      [2]=>
      int(6)
    }
    ["sub_array[2,2]"]=>
    array(1) {
      [0]=>
      int(11)
    }
    ["sub_array[2,3]"]=>
    array(2) {
      [0]=>
      int(22)
      [1]=>
      int(-55)
    }
    ["sub_array[2,4]"]=>
    array(0) {
    }
  }
  ["b"]=>
  string(1) "b"
  [0]=>
  float(0.001)
  ["abcd%0abcd%0abcd"]=>
  string(14) "abcd%0abcd%0abcd"
  [4]=>
  float(4.01)
  [5]=>
  string(1) "5"
}
Done
