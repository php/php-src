--TEST--
Test array_intersect_key() function : usage variation - Passing null,unset and undefeined variable indexed array
--FILE--
<?php
/* Prototype  : array array_intersect_key(array arr1, array arr2 [, array ...])
 * Description: Returns the entries of arr1 that have keys which are present in all the other arguments.
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_intersect_key() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$input_array = array(0 => '0', 1 => '1' , -10 => '-10' , null => 'null');
//get an unset variable
$unset_var = 10;
unset ($unset_var);

$input_arrays = array(
      'null indexed' => array(NULL => 'null 1', null => 'null 2'),
      'undefined indexed' => array(@$undefined_var => 'undefined'),
      'unset  indexed' => array(@$unset_var => 'unset'),
);

foreach($input_arrays as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( array_intersect_key($input_array, $value) );
      var_dump( array_intersect_key($value,$input_array ) );
}
?>
===DONE===
--EXPECT--
*** Testing array_intersect_key() : usage variation ***

--null indexed--
array(1) {
  [""]=>
  string(4) "null"
}
array(1) {
  [""]=>
  string(6) "null 2"
}

--undefined indexed--
array(1) {
  [""]=>
  string(4) "null"
}
array(1) {
  [""]=>
  string(9) "undefined"
}

--unset  indexed--
array(1) {
  [""]=>
  string(4) "null"
}
array(1) {
  [""]=>
  string(5) "unset"
}
===DONE===
