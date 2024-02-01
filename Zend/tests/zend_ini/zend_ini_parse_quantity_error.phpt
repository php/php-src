--TEST--
Test parsing of quantities: errors
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

    '0X',    # Valid prefix with no value
    '0Z',    # Invalid prefix
    '0XK',   # Valid prefix with no value and multiplier

    '++',
    '-+',
    '+ 25',
    '- 25',

    # Null bytes
    " 123\x00K",
    "\x00 123K",
    " \x00123K",
    " 123\x00K",
    " 123K\x00",
    " 123\x00",
];

foreach ($tests as $setting) {
    printf("# \"%s\"\n", addcslashes($setting, "\0..\37!@\177..\377"));
    var_dump(zend_test_zend_ini_parse_quantity($setting));
    print "\n";
}
--EXPECTF--
# "K"

Warning: Invalid quantity "K": no valid leading digits, interpreting as "0" for backwards compatibility in %s%ezend_ini_parse_quantity_error.php on line %d
int(0)

# "1KM"

Warning: Invalid quantity "1KM", interpreting as "1M" for backwards compatibility in %s%ezend_ini_parse_quantity_error.php on line %d
int(1048576)

# "1X"

Warning: Invalid quantity "1X": unknown multiplier "X", interpreting as "1" for backwards compatibility in %s%ezend_ini_parse_quantity_error.php on line %d
int(1)

# "1.0K"

Warning: Invalid quantity "1.0K", interpreting as "1K" for backwards compatibility in %s%ezend_ini_parse_quantity_error.php on line %d
int(1024)

# "0X"

Warning: Invalid quantity "0X": no digits after base prefix, interpreting as "0" for backwards compatibility in %s%ezend_ini_parse_quantity_error.php on line %d
int(0)

# "0Z"

Warning: Invalid prefix "0Z", interpreting as "0" for backwards compatibility in %s%ezend_ini_parse_quantity_error.php on line %d
int(0)

# "0XK"

Warning: Invalid quantity "0XK": no valid leading digits, interpreting as "0" for backwards compatibility in %s%ezend_ini_parse_quantity_error.php on line %d
int(0)

# "++"

Warning: Invalid quantity "++": no valid leading digits, interpreting as "0" for backwards compatibility in %s%ezend_ini_parse_quantity_error.php on line %d
int(0)

# "-+"

Warning: Invalid quantity "-+": no valid leading digits, interpreting as "0" for backwards compatibility in %s%ezend_ini_parse_quantity_error.php on line %d
int(0)

# "+ 25"

Warning: Invalid quantity "+ 25": no valid leading digits, interpreting as "0" for backwards compatibility in %s%ezend_ini_parse_quantity_error.php on line %d
int(0)

# "- 25"

Warning: Invalid quantity "- 25": no valid leading digits, interpreting as "0" for backwards compatibility in %s%ezend_ini_parse_quantity_error.php on line %d
int(0)

# " 123\000K"

Warning: Invalid quantity " 123\x00K", interpreting as " 123K" for backwards compatibility in %s on line %d
int(125952)

# "\000 123K"

Warning: Invalid quantity "\x00 123K": no valid leading digits, interpreting as "0" for backwards compatibility in %s on line %d
int(0)

# " \000123K"

Warning: Invalid quantity " \x00123K": no valid leading digits, interpreting as "0" for backwards compatibility in %s on line %d
int(0)

# " 123\000K"

Warning: Invalid quantity " 123\x00K", interpreting as " 123K" for backwards compatibility in %s on line %d
int(125952)

# " 123K\000"

Warning: Invalid quantity " 123K\x00": unknown multiplier "\x00", interpreting as " 123" for backwards compatibility in %s on line %d
int(123)

# " 123\000"

Warning: Invalid quantity " 123\x00": unknown multiplier "\x00", interpreting as " 123" for backwards compatibility in %s on line %d
int(123)
