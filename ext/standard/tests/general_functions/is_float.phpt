--TEST--
Test is_float()
--FILE--
<?php

// different valid float values
$floats = [
  PHP_INT_MIN -1,
  PHP_INT_MAX +1,
  0.0,
  -0.1,
  10.0000000000000000005,
  10.5e+5,
  1e5,
  -1e5,
  1e-5,
  -1e-5,
  1e+5,
  -1e+5,
  1E5,
  -1E5,
  1E+5,
  -1E+5,
  1E-5,
  -1E-5,
  .5e+7,
  -.5e+7,
  .6e-19,
  -.6e-19,
  .05E+44,
  -.05E+44,
  .0034E-30,
  -.0034E-30,
];

foreach ($floats as $float ) {
	if (!is_float($float)) {
		echo "Value $float should be a float!\n";
	}
}

// null, bool, int, objects, arrays, and resources

$not_floats = [
  new stdClass(),
  STDOUT,
  array(),
  array(NULL),
  null,
  true,  // boolean
  false,
  '',
  '0',
  '0.0',
  '0.5',
  '1e5',
  '1.5e6_string',

  1,  // integers, hex and octal
  -1,
  0,
  12345,
  0xFF55,
  -0x673,
  0123,
  -0123,
];

foreach ($not_floats as $value ) {
	if (is_float($value)) {
		echo "Value $value should not be a float!\n";
	}
}

echo "Done\n";

?>
--EXPECT--
Done
