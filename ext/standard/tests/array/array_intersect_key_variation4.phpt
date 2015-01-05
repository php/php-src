--TEST--
Test array_intersect_key() function : usage variation - Passing integer indexed array
--FILE--
<?php
/* Prototype  : array array_intersect_key(array arr1, array arr2 [, array ...])
 * Description: Returns the entries of arr1 that have keys which are present in all the other arguments. 
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_intersect_key() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$input_array = array(0 => '0', -1 => '-1' , 02 => 'two', -07 => '-07', 0xA => '0xA', -0xC => '-0xc'); 

$input_arrays = array(
      'decimal indexed' => array(10 => '10', '-17' => '-17'),
      'octal indexed' => array(-011 => '-011', 012 => '012'),
      'hexa  indexed' => array(0x12 => '0x12', -0x7 => '-0x7', ),
);

foreach($input_arrays as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( array_intersect_key($input_array, $value) );
      var_dump( array_intersect_key($value,$input_array ) );
}
?>
===DONE===
--EXPECTF--
*** Testing array_intersect_key() : usage variation ***

--decimal indexed--
array(1) {
  [10]=>
  string(3) "0xA"
}
array(1) {
  [10]=>
  string(2) "10"
}

--octal indexed--
array(1) {
  [10]=>
  string(3) "0xA"
}
array(1) {
  [10]=>
  string(3) "012"
}

--hexa  indexed--
array(1) {
  [-7]=>
  string(3) "-07"
}
array(1) {
  [-7]=>
  string(4) "-0x7"
}
===DONE===
