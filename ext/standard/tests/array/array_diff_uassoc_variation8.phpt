--TEST--
Test array_diff_uassoc() function : usage variation - Comparing integers with strings containing integers and float
--FILE--
<?php
/* Prototype  : array array_diff_uassoc(array arr1, array arr2 [, array ...], callback key_comp_func)
 * Description: Computes the difference of arrays with additional index check which is performed by a 
 * 				user supplied callback function
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_diff_uassoc() : usage variation ***\n";

//Initialize variables
$arr_default_int = array(1, 2, 3);
$arr_string_int = array('1', '2');
$arr_string_float = array('0' => '1.00', '1.00' => '2.00');

function key_compare_func($key1, $key2)
{
    if ($key1 === $key2) {
        return 0;
    }
    return ($key1 > $key2)? 1:-1;
}

echo "\n-- Result of comparing integers and strings containing an integers --\n";
var_dump( array_diff_uassoc($arr_default_int, $arr_string_int, "key_compare_func") );
var_dump( array_diff_uassoc($arr_string_int, $arr_default_int, "key_compare_func") );

echo "\n-- Result of comparing integers and strings containing floating points --\n";
var_dump( array_diff_uassoc($arr_default_int, $arr_string_float, "key_compare_func") );
var_dump( array_diff_uassoc($arr_string_float, $arr_default_int, "key_compare_func") );

?>
===DONE===
--EXPECTF--
*** Testing array_diff_uassoc() : usage variation ***

-- Result of comparing integers and strings containing an integers --
array(1) {
  [2]=>
  int(3)
}
array(0) {
}

-- Result of comparing integers and strings containing floating points --
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(2) {
  [0]=>
  string(4) "1.00"
  ["1.00"]=>
  string(4) "2.00"
}
===DONE===
