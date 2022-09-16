--TEST--
Test zend_ini_parse_uquantity() overflow handling
--EXTENSIONS--
zend_test
--FILE--
<?php

$tests = [
    'No overflow 001' => '0',
    'No overflow 002' => '1',
    'No overflow 003' => '100',
    'No overflow 004' => strval(PHP_INT_MAX),
    'No overflow 005' => '2K',
    'No overflow 006' => '-1',
    'No overflow 007' => ' -1',
    'No overflow 008' => '-1 ',
    'No overflow 009' => ' -1 ',
    'Subject overflow 001' => base_convert(str_repeat('1', PHP_INT_SIZE*8+1), 2, 10),
    'Subject overflow 002' => '-'.base_convert(str_repeat('1', PHP_INT_SIZE*8+1), 2, 10),
    'Subject overflow 003' => strval(PHP_INT_MIN),
    'Subject overflow 004' => '-2',
    'Subject overflow 005' => '-1K',
    'Subject overflow 006' => '-1 K',
    'Multiplier overflow 001' => strval(PHP_INT_MAX).'K',
];

foreach ($tests as $name => $value) {
    printf("# %s: \"%s\"\n", $name, $value);
    printf("%u\n", zend_test_zend_ini_parse_uquantity($value));
    print "\n";
    print "----------\n";
}

printf("# zend_test_zend_ini_parse_uquantity(\"-1\") === -1\n");
var_dump(zend_test_zend_ini_parse_uquantity("-1") === -1);

--EXPECTF--
# No overflow 001: "0"
0

----------
# No overflow 002: "1"
1

----------
# No overflow 003: "100"
100

----------
# No overflow 004: "%d"
%d

----------
# No overflow 005: "2K"
2048

----------
# No overflow 006: "-1"
%d

----------
# No overflow 007: " -1"
%d

----------
# No overflow 008: "-1 "
%d

----------
# No overflow 009: " -1 "
%d

----------
# Subject overflow 001: "%d"

Warning: Invalid quantity "%d": value is out of range, using overflow result for backwards compatibility in %s on line %d
%d

----------
# Subject overflow 002: "-%d"

Warning: Invalid quantity "-%d": value is out of range, using overflow result for backwards compatibility in %s on line %d
%d

----------
# Subject overflow 003: "-%d"

Warning: Invalid quantity "-%d": value is out of range, using overflow result for backwards compatibility in %s on line %d
%d

----------
# Subject overflow 004: "-2"

Warning: Invalid quantity "-2": value is out of range, using overflow result for backwards compatibility in %s on line %d
%d

----------
# Subject overflow 005: "-1K"

Warning: Invalid quantity "-1K": value is out of range, using overflow result for backwards compatibility in %s on line %d
%d

----------
# Subject overflow 006: "-1 K"

Warning: Invalid quantity "-1 K": value is out of range, using overflow result for backwards compatibility in %s on line %d
%d

----------
# Multiplier overflow 001: "%dK"

Warning: Invalid quantity "%dK": value is out of range, using overflow result for backwards compatibility in %s on line %d
%d

----------
# zend_test_zend_ini_parse_uquantity("-1") === -1
bool(true)
