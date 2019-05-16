--TEST--
Test intval() function with "0b" string prefix
--FILE--
<?php

$isspaceChars = " \t\n\r\f\v";

$goodInputs = [
    '0b1111111111111111111111111111111',
    '+0b1111111111111111111111111111111',
    '-0b1111111111111111111111111111111',
    $isspaceChars . '0b1111111111111111111111111111111',
    $isspaceChars . '+0b1111111111111111111111111111111',
    $isspaceChars . '-0b1111111111111111111111111111111',
    '0b',
    '0B',
    '0B1',
    '0b000',
    '0b001',
    '0b00100',
    '0b1 1'
];

$badInputs = [
    'b101',
    '0b00200',
    '--0b123',
    '++0b123',
    '0bb123',
    '0 b123',
];

print "--- Good Inputs - Base = 0 ---\n";

foreach ($goodInputs as $input) {
    var_dump(
        intval($input, 0)
    );
}

print "--- Good Inputs - Base = 2 ---\n";

foreach ($goodInputs as $input) {
    var_dump(
        intval($input, 2)
    );
}

print "--- Good Inputs - Base = default ---\n";

foreach ($goodInputs as $input) {
    var_dump(
        intval($input)
    );
}

print "--- Bad Inputs - Base = 0 ---\n";

foreach ($badInputs as $input) {
    var_dump(
        intval($input, 0)
    );
}

print '--- Done ---';

?>
--EXPECT--
--- Good Inputs - Base = 0 ---
int(2147483647)
int(2147483647)
int(-2147483647)
int(2147483647)
int(2147483647)
int(-2147483647)
int(0)
int(0)
int(1)
int(0)
int(1)
int(4)
int(1)
--- Good Inputs - Base = 2 ---
int(2147483647)
int(2147483647)
int(-2147483647)
int(2147483647)
int(2147483647)
int(-2147483647)
int(0)
int(0)
int(1)
int(0)
int(1)
int(4)
int(1)
--- Good Inputs - Base = default ---
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
--- Bad Inputs - Base = 0 ---
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
--- Done ---
