--TEST--
Test array_diff_ukey() function : usage variation - Passing null,unset and undefined variable indexed array
--FILE--
<?php
/* Prototype  : array array_diff_ukey(array arr1, array arr2 [, array ...], callback key_comp_func)
 * Description: Returns the entries of arr1 that have keys which are not present in any of the others arguments.
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_diff_ukey() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$input_array = array(10 => '10', "" => 'empty');

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
      var_dump( array_diff_ukey($value, $input_array, 'strcasecmp') );
      var_dump( array_diff_ukey($input_array, $value, 'strcasecmp') );
}

?>
===DONE===
--EXPECTF--
*** Testing array_diff_ukey() : usage variation ***

--null indexed--
array(0) {
}
array(1) {
  [10]=>
  string(2) "10"
}

--undefined indexed--
array(0) {
}
array(1) {
  [10]=>
  string(2) "10"
}

--unset  indexed--
array(0) {
}
array(1) {
  [10]=>
  string(2) "10"
}
===DONE===
