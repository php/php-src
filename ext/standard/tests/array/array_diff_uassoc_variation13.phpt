--TEST--
Test array_diff_uassoc() function : usage variations - arrays containing referenced variables
--FILE--
<?php
/* Prototype  : array array_diff_uassoc(array arr1, array arr2 [, array ...], callback key_comp_func)
 * Description: Computes the difference of arrays with additional index check which is performed by a 
 * 				user supplied callback function
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_diff_uassoc() : usage variation ***\n";

//Initialize variables
$ref_var = 'a';
$array1 = array('a', $ref_var);
$array2 = array('a' => 1, &$ref_var);

echo "\n-- Testing array_diff_uassoc() function with referenced variable \$ref_var has value '$ref_var' --\n";
var_dump( array_diff_uassoc($array1, $array2, "strcasecmp") );
var_dump( array_diff_uassoc($array2, $array1, "strcasecmp") );

// re-assign reference variable to different value
$ref_var = 10.00;
echo "\n-- Testing array_diff_uassoc() function with referenced variable \$ref_var value changed to $ref_var --\n";
var_dump( array_diff_uassoc($array1, $array2, "strcasecmp") );
var_dump( array_diff_uassoc($array2, $array1, "strcasecmp") );

//When array are refenced
$array2 = &$array1;
echo "\n-- Testing array_diff_uassoc() function when \$array2 is referenced to \$array1 --\n";
var_dump( array_diff_uassoc($array1, $array2, "strcasecmp") );
var_dump( array_diff_uassoc($array2, $array1, "strcasecmp") );

?>
===DONE===
--EXPECTF--
*** Testing array_diff_uassoc() : usage variation ***

-- Testing array_diff_uassoc() function with referenced variable $ref_var has value 'a' --
array(1) {
  [1]=>
  string(1) "a"
}
array(1) {
  ["a"]=>
  int(1)
}

-- Testing array_diff_uassoc() function with referenced variable $ref_var value changed to 10 --
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "a"
}
array(2) {
  ["a"]=>
  int(1)
  [0]=>
  &float(10)
}

-- Testing array_diff_uassoc() function when $array2 is referenced to $array1 --
array(0) {
}
array(0) {
}
===DONE===
