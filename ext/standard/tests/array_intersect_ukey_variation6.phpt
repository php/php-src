--TEST--
Test array_intersect_ukey() function : usage variation - Intersection of floating points with strings.
--FILE--
<?php
/* Prototype  : array array_intersect_ukey(array arr1, array arr2 [, array ...], callback key_compare_func)
 * Description: Computes the intersection of arrays using a callback function on the keys for comparison. 
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_intersect_ukey() : usage variation ***\n";

//Initialize variables
$arr_float = array(0.00 => 1.00, 1.00 => 2.00);
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

echo "\n-- Result of floating points and strings containing integers intersection --\n";
var_dump( array_intersect_ukey($arr_float, $arr_string, 'key_compare_func') );

echo "\n-- Result of floating points and strings containing floating point intersection --\n";
var_dump( array_intersect_ukey($arr_float, $arr_string_float, 'key_compare_func') );
?>
===DONE===
--EXPECTF--
*** Testing array_intersect_ukey() : usage variation ***

-- Result of floating points and strings containing integers intersection --
array(2) {
  [0]=>
  float(1)
  [1]=>
  float(2)
}

-- Result of floating points and strings containing floating point intersection --
array(2) {
  [0]=>
  float(1)
  [1]=>
  float(2)
}
===DONE===