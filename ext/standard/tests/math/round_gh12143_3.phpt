--TEST--
GH-12143: Test rounding of 0.5.
--FILE--
<?php
foreach ([
    0.5,
    -0.5,
] as $number) {
    foreach ([
        'PHP_ROUND_HALF_UP',
        'PHP_ROUND_HALF_DOWN',
        'PHP_ROUND_HALF_EVEN',
        'PHP_ROUND_HALF_ODD',
    ] as $mode) {
        printf("%-20s: %+.17g -> %+.17g\n", $mode, $number, round($number, 0, constant($mode)));
    }
}
?>
--EXPECT--
PHP_ROUND_HALF_UP   : +0.5 -> +1
PHP_ROUND_HALF_DOWN : +0.5 -> +0
PHP_ROUND_HALF_EVEN : +0.5 -> +0
PHP_ROUND_HALF_ODD  : +0.5 -> +1
PHP_ROUND_HALF_UP   : -0.5 -> -1
PHP_ROUND_HALF_DOWN : -0.5 -> -0
PHP_ROUND_HALF_EVEN : -0.5 -> -0
PHP_ROUND_HALF_ODD  : -0.5 -> -1
