--TEST--
GH-13243 (intval() does not detect the explicit octal "0o" prefix)
--FILE--
<?php

$isspaceChars = " \t\n\r\f\v";

$goodInputs = [
    '0o17',
    '0O17',
    '+0o17',
    '-0o17',
    $isspaceChars . '0o17',
    $isspaceChars . '-0o17',
    '0o',
    '-0o',
    '0o0',
    '0o007',
    '0o7 7',
];

$badInputs = [
    'o17',
    '0o8',
    '--0o17',
    '0oo17',
    '0 o17',
];

foreach ([0, 8] as $base) {
    echo "--- Good Inputs - Base = $base ---\n";
    foreach ($goodInputs as $input) {
        var_dump(intval($input, $base));
    }
    var_dump(intval('0o' . str_repeat('7', 30), $base) === PHP_INT_MAX);
    var_dump(intval('-0o' . str_repeat('7', 30), $base) === PHP_INT_MIN);
}

echo "--- Bad Inputs - Base = 0 ---\n";
foreach ($badInputs as $input) {
    var_dump(intval($input, 0));
}

echo "--- Prefix not applied for other bases or casts ---\n";
var_dump(intval('0o17'));
var_dump(intval('0o17', 10));
var_dump(intval('0o17', 2));
var_dump(intval('0o17', 16));
var_dump((int) '0o17');

echo "--- Existing prefixes unchanged ---\n";
var_dump(intval('-0b101', 0));
var_dump(intval('-0b101', 8));
var_dump(intval('-0x1f', 0));
var_dump(intval('-017', 0));
var_dump(intval('017', 8));

?>
--EXPECT--
--- Good Inputs - Base = 0 ---
int(15)
int(15)
int(15)
int(-15)
int(15)
int(-15)
int(0)
int(0)
int(0)
int(7)
int(7)
bool(true)
bool(true)
--- Good Inputs - Base = 8 ---
int(15)
int(15)
int(15)
int(-15)
int(15)
int(-15)
int(0)
int(0)
int(0)
int(7)
int(7)
bool(true)
bool(true)
--- Bad Inputs - Base = 0 ---
int(0)
int(0)
int(0)
int(0)
int(0)
--- Prefix not applied for other bases or casts ---
int(0)
int(0)
int(0)
int(0)
int(0)
--- Existing prefixes unchanged ---
int(-5)
int(0)
int(-31)
int(-15)
int(15)
