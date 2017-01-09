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
        $input,
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
        $input,
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
string(34) "0b11111111111111111111111111111111"
int(4294967295)
int(4294967295)
int(-4294967295)
int(4294967295)
int(4294967295)
int(-4294967295)
string(2) "0b"
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
string(2) "0B"
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
string(3) "0B1"
int(1)
int(1)
int(-1)
int(1)
int(1)
int(-1)
string(5) "0b000"
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
string(5) "0b001"
int(1)
int(1)
int(-1)
int(1)
int(1)
int(-1)
string(7) "0b00100"
int(4)
int(4)
int(-4)
int(4)
int(4)
int(-4)
string(5) "0b1 1"
int(1)
int(1)
int(-1)
int(1)
int(1)
int(-1)
--- Bad Inputs ---
string(4) "b101"
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
string(7) "0b00200"
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
string(7) "--0b123"
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
string(7) "++0b123"
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
string(6) "0bb123"
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
string(6) "0 b123"
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
--- Done ---
