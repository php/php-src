--TEST--
Test parsing of quantities (e.g. "16M"): errors
--EXTENSIONS--
zend_test
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
  var_dump(zend_test_zend_ini_parse_quantity($setting));
}

--EXPECTF--
Warning: Invalid numeric string 'K': no valid leading digits, interpreting as '0' for backwards compatibility in %s%ezend_ini_parse_quantity_error.php on line %d
int(0)

Warning: Invalid numeric string '1KM', interpreting as '1M' for backwards compatibility in %s%ezend_ini_parse_quantity_error.php on line %d
int(1048576)

Warning: Invalid numeric string '1X': unknown multipler 'X', interpreting as '1' for backwards compatibility in %s%ezend_ini_parse_quantity_error.php on line %d
int(1)

Warning: Invalid numeric string '1.0K', interpreting as '1K' for backwards compatibility in %s%ezend_ini_parse_quantity_error.php on line %d
int(1024)
