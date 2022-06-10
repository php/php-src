--TEST--
Test zend_ini_parse_quantity() overflow handling
--EXTENSIONS--
zend_test
--FILE--
<?php

function increment(string $s): string {
    if (strlen($s) === 0) {
        return '1';
    }

    $digit = intval($s[strlen($s)-1]);
    $prefix = substr($s, 0, -1);

    if ($digit === 9) {
        return increment($prefix) . '0';
    }

    return $prefix . strval($digit+1);
}

function decrement(string $s): string {
    assert($s[0] === '-');
    return '-' . increment(substr($s, 1));
}

$tests = [
    'No overflow 001' => '0',
    'No overflow 002' => '1',
    'No overflow 003' => '100',
    'No overflow 004' => strval(PHP_INT_MAX),
    'No overflow 005' => strval(PHP_INT_MIN),
    'No overflow 006' => '2K',
    'No overflow 007' => '-2K',
    'Subject overflow 001' => increment(strval(PHP_INT_MAX)),
    'Subject overflow 002' => decrement(strval(PHP_INT_MIN)),
    'Multiplier overflow 001' => strval(PHP_INT_MAX).'K',
    'Multiplier overflow 002' => strval(PHP_INT_MIN).'K',
];

foreach ($tests as $name => $value) {
    printf("# %s: \"%s\"\n", $name, $value);
    printf("%d\n", zend_test_zend_ini_parse_quantity($value));
    print "\n";
    print "----------\n";
}

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
# No overflow 005: "-%d"
-%d

----------
# No overflow 006: "2K"
2048

----------
# No overflow 007: "-2K"
-2048

----------
# Subject overflow 001: "%d"

Warning: Invalid quantity "%d": value is out of range, using overflow result for backwards compatibility in %s on line %d
%s

----------
# Subject overflow 002: "-%d"

Warning: Invalid quantity "-%d": value is out of range, using overflow result for backwards compatibility in %s on line %d
%s

----------
# Multiplier overflow 001: "%dK"

Warning: Invalid quantity "%dK": value is out of range, using overflow result for backwards compatibility in %s on line %d
%s

----------
# Multiplier overflow 002: "-%dK"

Warning: Invalid quantity "-%dK": value is out of range, using overflow result for backwards compatibility in %s on line %d
%s

----------
