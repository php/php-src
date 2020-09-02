--TEST--
Test array_diff_ukey() function : usage variation - Passing boolean indexed array
--FILE--
<?php
echo "*** Testing array_diff_ukey() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$input_array = array(0 => '0', 1 => '1', -10 => '-10', 'true' => 1, 'false' => 0);
$boolean_indx_array = array(true => 'boolt', false => 'boolf', TRUE => 'boolT', FALSE => 'boolF');

function key_compare_func($key1, $key2)
{
  return strcasecmp($key1, $key2);
}

echo "\n-- Testing array_diff_ukey() function with boolean indexed array --\n";

var_dump( array_diff_ukey($boolean_indx_array, $input_array, 'key_compare_func') );
var_dump( array_diff_ukey($input_array, $boolean_indx_array, 'key_compare_func') );

?>
--EXPECT--
*** Testing array_diff_ukey() : usage variation ***

-- Testing array_diff_ukey() function with boolean indexed array --
array(0) {
}
array(3) {
  [-10]=>
  string(3) "-10"
  ["true"]=>
  int(1)
  ["false"]=>
  int(0)
}
