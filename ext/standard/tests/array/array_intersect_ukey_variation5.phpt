--TEST--
Test array_intersect_ukey() function : usage variation - Intersection of integers with floats and strings.
--FILE--
<?php
echo "*** Testing array_intersect_ukey() : usage variation ***\n";

//Initialize variables
$arr_default_int = array(1, 2 );
$arr_float = array(0 => 1.00, 1 => 2.00, 2 => 3.00);
$arr_string = array('0' => '1', '1' => '2', '2' => '3');
$arr_string_float = array('0.00' => '1.00', '1.00' => '2.00');

//Call back function
function key_compare_func($key1, $key2)
{
    if ($key1 == $key2)
        return 0;
    else
        return ($key1 > $key2)? 1:-1;
}

echo "\n-- Result of integers and floating point intersection --\n";
var_dump( array_intersect_ukey($arr_default_int, $arr_float, "key_compare_func") );

echo "\n-- Result of integers and strings containing integers intersection --\n";
var_dump( array_intersect_ukey($arr_default_int, $arr_string, "key_compare_func") );

echo "\n-- Result of integers and strings containing floating points intersection --\n";
var_dump( array_intersect_ukey($arr_default_int, $arr_string_float, "key_compare_func") );
?>
--EXPECT--
*** Testing array_intersect_ukey() : usage variation ***

-- Result of integers and floating point intersection --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}

-- Result of integers and strings containing integers intersection --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}

-- Result of integers and strings containing floating points intersection --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
