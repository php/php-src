--TEST--
Surprising result with integer literals (hex/binary/octal)
--EXTENSIONS--
gmp
--FILE--
<?php

$values = [
    '0x',
    '0X',
    '0b',
    '0B',
    '0o',
    '0O',
    ''
];

foreach ($values as $value) {
    try {
        var_dump(gmp_init($value));
    } catch (\ValueError $e) {
        echo $e->getMessage(), \PHP_EOL;
    }
}
?>
--EXPECT--
gmp_init(): Argument #1 ($num) is not an integer string
gmp_init(): Argument #1 ($num) is not an integer string
gmp_init(): Argument #1 ($num) is not an integer string
gmp_init(): Argument #1 ($num) is not an integer string
gmp_init(): Argument #1 ($num) is not an integer string
gmp_init(): Argument #1 ($num) is not an integer string
gmp_init(): Argument #1 ($num) is not an integer string
