--TEST--
BcMath\Number round() - has trailing zeros
--EXTENSIONS--
bcmath
--FILE--
<?php
foreach (RoundingMode::cases() as $mode) {
    echo "========== {$mode->name} ==========\n";
    foreach ([
        ['0.00', 1],
        ['0.1', 5],
        ['-0.300', 5],
        ['1.995', 2],
    ] as [$value, $precision]) {
        $number = new BcMath\Number($value);
        $output = 'value is ';
        $output .= str_pad($value, 6, ' ', STR_PAD_RIGHT) . ', ';
        $output .= "precision is {$precision}: ";
        $output .= $number->round($precision, $mode) . "\n";
        echo $output;
    }
    echo "\n";
}
?>
--EXPECT--
========== HalfAwayFromZero ==========
value is 0.00  , precision is 1: 0.0
value is 0.1   , precision is 5: 0.10000
value is -0.300, precision is 5: -0.30000
value is 1.995 , precision is 2: 2.00

========== HalfTowardsZero ==========
value is 0.00  , precision is 1: 0.0
value is 0.1   , precision is 5: 0.10000
value is -0.300, precision is 5: -0.30000
value is 1.995 , precision is 2: 1.99

========== HalfEven ==========
value is 0.00  , precision is 1: 0.0
value is 0.1   , precision is 5: 0.10000
value is -0.300, precision is 5: -0.30000
value is 1.995 , precision is 2: 2.00

========== HalfOdd ==========
value is 0.00  , precision is 1: 0.0
value is 0.1   , precision is 5: 0.10000
value is -0.300, precision is 5: -0.30000
value is 1.995 , precision is 2: 1.99

========== TowardsZero ==========
value is 0.00  , precision is 1: 0.0
value is 0.1   , precision is 5: 0.10000
value is -0.300, precision is 5: -0.30000
value is 1.995 , precision is 2: 1.99

========== AwayFromZero ==========
value is 0.00  , precision is 1: 0.0
value is 0.1   , precision is 5: 0.10000
value is -0.300, precision is 5: -0.30000
value is 1.995 , precision is 2: 2.00

========== NegativeInfinity ==========
value is 0.00  , precision is 1: 0.0
value is 0.1   , precision is 5: 0.10000
value is -0.300, precision is 5: -0.30000
value is 1.995 , precision is 2: 1.99

========== PositiveInfinity ==========
value is 0.00  , precision is 1: 0.0
value is 0.1   , precision is 5: 0.10000
value is -0.300, precision is 5: -0.30000
value is 1.995 , precision is 2: 2.00
