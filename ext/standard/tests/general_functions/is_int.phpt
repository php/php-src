--TEST--
Test is_int()
--FILE--
<?php
// different valid  integer values
$valid_ints = [
  0,
  1,
  -1,
  PHP_INT_MAX,
  PHP_INT_MIN,
  2147483640,
];

foreach ($valid_ints as $value ) {
	if (!is_int($value)) {
		echo "Value $value should be an int!\n";
	}
}

$not_int_types = [
  /* float values */
  PHP_INT_MIN-1,
  PHP_INT_MAX+1,
  0.0,
  -0.1,
  1.0,
  1e5,
  -1e6,
  1E8,
  -1E9,
  10.0000000000000000005,
  10.5e+5,

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
];

foreach ($not_int_types as $type ) {
	if (is_int($type)) {
		echo "Value $type should not be an int!\n";
	}
}

echo "Done\n";

?>
--EXPECT--
Done
