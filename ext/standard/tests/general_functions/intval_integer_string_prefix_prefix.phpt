--TEST--
Test intval() function with integer strings with prefixes
--FILE--
<?php

$signs = [
    '',
    '+',
    '-',
];

$whitespaces = [
    '',
    " \t\n\r\f\v",
];

$bases = [
    2 => [
        [
            '0b',
            '0B',
        ],
        [
            '1111111111111111111111111111111' => 0b1111111111111111111111111111111,
            str_repeat('1', 65) => PHP_INT_MAX,
            '1' => 1,
            '000' => 0,
            '001' => 1,
            '00100' => 0b100,
        ],
    ],
    8 => [
        [
            '0o',
            '0O',
        ],
        [
            '7777777' => 0o7777777,
            str_repeat('7', 22) => PHP_INT_MAX,
            '7' => 0o7,
            '000' => 0,
            '007' => 0o7,
            '00700' => 0o700,
        ],
    ],
    16 => [
        [
            '0x',
            '0X',
        ],
        [
            'FFFFFFF' => 0xFFFFFFF,
            str_repeat('F', 18) => PHP_INT_MAX,
            'A' => 0xA,
            '000' => 0,
            '00A' => 0xA,
            '00A00' => 0xA00,
        ],
    ],
];

$i = 0;
echo "Checking well formed values:\n";
foreach ($bases as $base => [$prefixes, $values]) {
    foreach ($values as $v => $expected_result) {
        foreach ($signs as $sign) {
            if ($sign == '-') {
                if ($expected_result == PHP_INT_MAX) {
                    $expected_result = PHP_INT_MIN;
                } else {
                    $expected_result *= -1;
                }
            }
            foreach ($whitespaces as $whitespace_prefix) {
                foreach ($whitespaces as $whitespace_suffix) {
                    foreach ($prefixes as $prefix) {
                        ++$i;
                        $s = $whitespace_prefix . $sign . $prefix . $v . $whitespace_suffix;
                        $base0 = intval($s, 0);
                        $baseGiven = intval($s, $base);
                        if ($base0 != $baseGiven) {
                            echo "Base 0 and $base don't agree for:\n$s\n";
                            continue;
                        }
                        if ($base0 != $expected_result) {
                            echo "Base 0 is not the expected result for:\n$s\nExpected:\n$expected_result\nReality:\n$base0\n";
                            continue;
                        }
                        /* Check that with prefix the default behaviour is 0 */
                        if ($prefix != '') {
                            $baseDefault = intval($s);
                            if ($baseDefault != 0) {
                                echo "Base default is not 0:\n$s\n";
                                continue;
                            }
                        }
                    }
                }
            }
        }
    }
}

echo $i, ' values checked', "\n";

$badInputs = [
    '0b',
    '0B',
    'b101',
    '0b00200',
    '--0b123',
    '++0b123',
    '0bb123',
    '0 b123',
    '0b1 1',
];

print "--- Bad Inputs - Base = 0 ---\n";
foreach ($badInputs as $input) {
    var_dump(
        intval($input, 0)
    );
}

print '--- Done ---';

?>
--EXPECT--
Checking well formed values:
432 values checked
--- Bad Inputs - Base = 0 ---
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(1)
--- Done ---
