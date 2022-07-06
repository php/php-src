--TEST--
Test array_diff_ukey() function : usage variation - Passing integer indexed array
--FILE--
<?php
echo "*** Testing array_diff_ukey() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$input_array = array(-07 => '-07', 0xA => '0xA');

$input_arrays = array(
      'decimal indexed' => array(10 => '10', '-17' => '-17'),
      'octal indexed' => array(-011 => '-011', 012 => '012'),
      'hexa  indexed' => array(0x12 => '0x12', -0x7 => '-0x7', ),
);

function key_compare_func($key1, $key2)
{
  return strcasecmp($key1, $key2);
}

foreach($input_arrays as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( array_diff_ukey($value, $input_array, 'key_compare_func') );
      var_dump( array_diff_ukey($input_array, $value, 'key_compare_func') );
}
?>
--EXPECT--
*** Testing array_diff_ukey() : usage variation ***

--decimal indexed--
array(1) {
  [-17]=>
  string(3) "-17"
}
array(1) {
  [-7]=>
  string(3) "-07"
}

--octal indexed--
array(1) {
  [-9]=>
  string(4) "-011"
}
array(1) {
  [-7]=>
  string(3) "-07"
}

--hexa  indexed--
array(1) {
  [18]=>
  string(4) "0x12"
}
array(1) {
  [10]=>
  string(3) "0xA"
}
