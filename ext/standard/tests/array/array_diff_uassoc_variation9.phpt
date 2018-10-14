--TEST--
Test array_diff_uassoc() function : usage variation - Passing integer indexed array
--FILE--
<?php
/* Prototype  : array array_diff_uassoc(array arr1, array arr2 [, array ...], callback key_comp_func)
 * Description: Computes the difference of arrays with additional index check which is performed by a
 * 				user supplied callback function
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_diff_uassoc() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$input_array = array(10 => 10, 12 => 12);

$input_arrays = array(
      'decimal indexed' => array(10 => 10, -17 => -17),
      'octal indexed' => array( 012 => 10, -011 => -011,),
      'hexa  indexed' => array(0xA => 10, -0x7 => -0x7 ),
);

foreach($input_arrays as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( array_diff_uassoc($input_array, $value, "strcasecmp") );
      var_dump( array_diff_uassoc($value, $input_array, "strcasecmp") );
}

?>
===DONE===
--EXPECTF--
*** Testing array_diff_uassoc() : usage variation ***

--decimal indexed--
array(1) {
  [12]=>
  int(12)
}
array(1) {
  [-17]=>
  int(-17)
}

--octal indexed--
array(1) {
  [12]=>
  int(12)
}
array(1) {
  [-9]=>
  int(-9)
}

--hexa  indexed--
array(1) {
  [12]=>
  int(12)
}
array(1) {
  [-7]=>
  int(-7)
}
===DONE===
