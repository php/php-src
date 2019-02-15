--TEST--
Test array_diff_uassoc() function : usage variation - Comparing floating points with strings having integers and float
--FILE--
<?php
/* Prototype  : array array_diff_uassoc(array arr1, array arr2 [, array ...], callback key_comp_func)
 * Description: Computes the difference of arrays with additional index check which is performed by a
 * 				user supplied callback function
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_diff_uassoc() : usage variation ***\n";

//Initialize variables
$arr_float = array(0 => 1.00, 1.00 => 2.00);
$arr_string_int = array('1', '2');
$arr_string_float = array('0' => '1.00', '1.00' => '2.00');

function key_compare_func($key1, $key2)
{
    if ($key1 === $key2) {
        return 0;
    }
    return ($key1 > $key2)? 1:-1;
}

echo "\n-- Result of comparing floating points and strings containing integers --\n";
var_dump( array_diff_uassoc($arr_float, $arr_string_int, "key_compare_func") );
var_dump( array_diff_uassoc($arr_string_int, $arr_float, "key_compare_func") );

echo "\n-- Result of comparing floating points and strings containing floating point --\n";
var_dump( array_diff_uassoc($arr_float, $arr_string_float, "key_compare_func") );
var_dump( array_diff_uassoc($arr_string_float, $arr_float, "key_compare_func") );

?>
===DONE===
--EXPECT--
*** Testing array_diff_uassoc() : usage variation ***

-- Result of comparing floating points and strings containing integers --
array(0) {
}
array(0) {
}

-- Result of comparing floating points and strings containing floating point --
array(2) {
  [0]=>
  float(1)
  [1]=>
  float(2)
}
array(2) {
  [0]=>
  string(4) "1.00"
  ["1.00"]=>
  string(4) "2.00"
}
===DONE===
