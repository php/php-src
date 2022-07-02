--TEST--
BCMath functions return result with requested scale
--EXTENSIONS--
bcmath
--FILE--
<?php
echo
    'bcadd:    ', bcadd('2', '1', 5), PHP_EOL,
    'bcdiv:    ', bcdiv('2', '1', 5), PHP_EOL,
    'bcmul:    ', bcmul('2', '1', 5), PHP_EOL,
    'bcpow:    ', bcpow('2', '1', 5), PHP_EOL,
    'bcpowmod: ', bcpowmod('2', '1', '3', 5), PHP_EOL,
    'bcsqrt:   ', bcsqrt('4', 5), PHP_EOL,
    'bcsub:    ', bcsub('2', '1', 5), PHP_EOL;
?>
--EXPECT--
bcadd:    3.00000
bcdiv:    2.00000
bcmul:    2.00000
bcpow:    2.00000
bcpowmod: 2.00000
bcsqrt:   2.00000
bcsub:    1.00000
