--TEST--
Test parsing failures of INI "size" values (e.g. "16M")
--FILE--
<?php

// This test checks invalid formats do throw warnings.

$tests = [
  'K',     # No digits
  '1KM',   # Multiple multipliers.
  '1X',    # Unknown multiplier.
  '1.0K',  # Non integral digits.
];

foreach ($tests as $setting) {
  ini_set('default_socket_timeout', $setting);
}

--EXPECTF--

Warning: Invalid numeric string 'K', no valid leading digits, interpreting as '0' in %s/zend_atol_error.php on line %d

Warning: Invalid numeric string '1KM', interpreting as '1M' in %s/zend_atol_error.php on line %d

Warning: Invalid numeric string '1X', interpreting as '1' in %s/zend_atol_error.php on line %d

Warning: Invalid numeric string '1.0K', interpreting as '1K' in %s/zend_atol_error.php on line %d
