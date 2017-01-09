--TEST--
Test intval() function with "0b" string prefix
--SKIPIF--
--FILE--
<?php

$goodInputs = [
    '0b11111111111111111111111111111111',
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

$isspaceChars = " \t\n\r\f\v";

print "--- Good Inputs ---\n";

foreach ($goodInputs as $input) {
    var_dump(
        intval($input, 0),
        intval('+' . $input, 0),
        intval('-' . $input, 0),
        intval($isspaceChars . $input, 0),
        intval($isspaceChars . '+' . $input, 0),
        intval($isspaceChars . '-' . $input, 0)
    );
}

print "--- Bad Inputs ---\n";

foreach ($badInputs as $input) {
    var_dump(
        intval($input, 0),
        intval('+' . $input, 0),
        intval('-' . $input, 0),
        intval($isspaceChars . $input, 0),
        intval($isspaceChars . '+' . $input, 0),
        intval($isspaceChars . '-' . $input, 0)
    );
}

print '--- Done ---';

?>
--EXPECTF--
--- Good Inputs ---
int(4294967295)
int(4294967295)
int(-4294967295)
int(4294967295)
int(4294967295)
int(-4294967295)
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
int(1)
int(1)
int(-1)
int(1)
int(1)
int(-1)
int(4)
int(4)
int(-4)
int(4)
int(4)
int(-4)
int(1)
int(1)
int(-1)
int(1)
int(1)
int(-1)
--- Bad Inputs ---
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
--- Done ---
