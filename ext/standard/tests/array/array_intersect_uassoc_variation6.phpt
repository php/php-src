--TEST--
Test array_intersect_uassoc() function : usage variation - Intersection of floating points with strings.
--FILE--
<?php
echo "*** Testing array_intersect_uassoc() : usage variation ***\n";

//Initialize variables
$arr_float = array(0 => 1.00, 1 => 2.00);
$arr_string = array('1', '2', '3');
$arr_string_float = array('1.00', '2.00');

function key_compare_func($a, $b)
{
    if ($a === $b) {
        return 0;
    }
    return ($a > $b)? 1:-1;
}

echo "\n-- Result of floating points and strings containing integers intersection --\n";
var_dump( array_intersect_uassoc($arr_float, $arr_string, "key_compare_func") );

echo "\n-- Result of floating points and strings containing floating point intersection --\n";
var_dump( array_intersect_uassoc($arr_float, $arr_string_float, "key_compare_func") );
?>
--EXPECT--
*** Testing array_intersect_uassoc() : usage variation ***

-- Result of floating points and strings containing integers intersection --
array(2) {
  [0]=>
  float(1)
  [1]=>
  float(2)
}

-- Result of floating points and strings containing floating point intersection --
array(0) {
}
