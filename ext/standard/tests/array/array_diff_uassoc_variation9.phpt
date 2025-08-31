--TEST--
Test array_diff_uassoc() function : usage variation - Passing integer indexed array
--FILE--
<?php
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
--EXPECT--
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
