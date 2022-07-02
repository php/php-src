--TEST--
Test array_intersect_ukey() function : usage variation - Intersection of strings containing integer and float
--FILE--
<?php
echo "*** Testing array_intersect_ukey() : usage variation ***\n";

//Initialize variables
$arr1_string_int = array('0' => '1', '1' => '2');
$arr2_string_int = array('0' => '1', '1' => '3');
$arr1_string_float = array('0.00' => '1.00', '1.00' => '2.00');
$arr2_string_float = array('0.00' => '1.00', '1.00' => '3.00');

//Call back function
function key_compare_func($key1, $key2)
{
    if ($key1 == $key2)
        return 0;
    else
        return ($key1 > $key2)? 1:-1;
}

echo "\n-- Result of strings containing integers intersection --\n";
var_dump( array_intersect_ukey($arr1_string_int, $arr2_string_int, 'key_compare_func') );

echo "\n-- Result of strings containing floating points intersection --\n";
var_dump( array_intersect_ukey($arr1_string_float, $arr2_string_float, 'key_compare_func') );

echo "\n-- Result of strings containing integers and strings containing floating points intersection --\n";
var_dump( array_intersect_ukey($arr1_string_int, $arr2_string_float, 'key_compare_func') );
?>
--EXPECT--
*** Testing array_intersect_ukey() : usage variation ***

-- Result of strings containing integers intersection --
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
}

-- Result of strings containing floating points intersection --
array(2) {
  ["0.00"]=>
  string(4) "1.00"
  ["1.00"]=>
  string(4) "2.00"
}

-- Result of strings containing integers and strings containing floating points intersection --
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
}
