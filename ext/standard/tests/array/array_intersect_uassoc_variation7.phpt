--TEST--
Test array_intersect_uassoc() function : usage variation - Intersection of strings containing integers, float
--FILE--
<?php
echo "*** Testing array_intersect_uassoc() : usage variation ***\n";

//Initialize variables
$arr1_string_int = array('1', '2');
$arr2_string_int = array('1', '3');
$arr1_string_float = array('1.00', '2.00');
$arr2_string_float = array('1.00', '3.00');

function key_compare_func($a, $b)
{
    if ($a === $b) {
        return 0;
    }
    return ($a > $b)? 1:-1;
}

echo "\n-- Result of strings containing integers intersection --\n";
var_dump( array_intersect_uassoc($arr1_string_int, $arr2_string_int, "key_compare_func") );

echo "\n-- Result of strings containing floating points intersection --\n";
var_dump( array_intersect_uassoc($arr1_string_float, $arr2_string_float, "key_compare_func") );

echo "\n-- Result of strings containing integers and strings containing floating points intersection --\n";
var_dump( array_intersect_uassoc($arr1_string_int, $arr2_string_float, "key_compare_func") );
?>
--EXPECT--
*** Testing array_intersect_uassoc() : usage variation ***

-- Result of strings containing integers intersection --
array(1) {
  [0]=>
  string(1) "1"
}

-- Result of strings containing floating points intersection --
array(1) {
  [0]=>
  string(4) "1.00"
}

-- Result of strings containing integers and strings containing floating points intersection --
array(0) {
}
