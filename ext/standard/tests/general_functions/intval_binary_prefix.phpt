--TEST--
Test intval() function with "0b" string prefix
--SKIPIF--
--FILE--
<?php

$inputs = [
    '0b11111111111111111111111111111111',
    '-0b11111111111111111111111111111111',
    '0b',
    '-0b',
    '0b000',
    '-0b000',
    '0b001',
    '-0b001',
    '0b00100',
    '-0b00100',
    '0b00200',
    '-0b00200',
];

foreach ($inputs as $input) {
	var_dump(intval($input, 0));
}

print '--- Done ---';

?>
--EXPECTF--
int(4294967295)
int(-4294967295)
int(0)
int(0)
int(0)
int(0)
int(1)
int(-1)
int(4)
int(-4)
int(0)
int(0)
--- Done ---
