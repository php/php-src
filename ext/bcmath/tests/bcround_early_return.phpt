--TEST--
bcround() function with early return
--EXTENSIONS--
bcmath
--FILE--
<?php

$early_return_cases = [
    ['123', -4],
    ['123.123456', -4],
    ['123', 1],
    ['123.5', 1],
    ['123.5', 2],
    ['123.0000000000000000000001', 22],
    ['123.0000000000000000000001', 23],
    ['-123', -4],
    ['-123.123456', -4],
    ['-123', 1],
    ['-123.5', 1],
    ['-123.5', 2],
    ['-123.0000000000000000000001', 22],
    ['-123.0000000000000000000001', 23],
    ['0', 0],
    ['0.0', 0],
    ['0.0000', 0],
    ['-0', 0],
    ['-0.0', 0],
    ['-0.0000', 0],
];

$results = [
    RoundingMode::HalfAwayFromZero->name => [],
];
foreach ($early_return_cases as [$num, $precision]) {
    $result = str_pad("[{$num}, {$precision}]", 33, ' ', STR_PAD_LEFT) . ' => ' . bcround($num, $precision, RoundingMode::HalfAwayFromZero) . "\n";
    echo $result;
    $results[RoundingMode::HalfAwayFromZero->name][] = $result;
}

echo "\n";

foreach (RoundingMode::cases() as $mode) {
    $results[$mode->name] = [];
    foreach ($early_return_cases as [$num, $precision]) {
        $result = str_pad("[{$num}, {$precision}]", 33, ' ', STR_PAD_LEFT) . ' => ' . bcround($num, $precision, $mode) . "\n";
        $results[$mode->name][] = $result;
    }

    if ($results[RoundingMode::HalfAwayFromZero->name] === $results[$mode->name]) {
        echo str_pad($mode->name, 24, ' ', STR_PAD_LEFT) . ": result is same to HalfAwayFromZero\n";
    } else {
        echo str_pad($mode->name, 24, ' ', STR_PAD_LEFT) . ": result is not same to HalfAwayFromZero, failed\n";
    }
}
?>
--EXPECT--
                        [123, -4] => 0
                 [123.123456, -4] => 0
                         [123, 1] => 123.0
                       [123.5, 1] => 123.5
                       [123.5, 2] => 123.50
 [123.0000000000000000000001, 22] => 123.0000000000000000000001
 [123.0000000000000000000001, 23] => 123.00000000000000000000010
                       [-123, -4] => 0
                [-123.123456, -4] => 0
                        [-123, 1] => -123.0
                      [-123.5, 1] => -123.5
                      [-123.5, 2] => -123.50
[-123.0000000000000000000001, 22] => -123.0000000000000000000001
[-123.0000000000000000000001, 23] => -123.00000000000000000000010
                           [0, 0] => 0
                         [0.0, 0] => 0
                      [0.0000, 0] => 0
                          [-0, 0] => 0
                        [-0.0, 0] => 0
                     [-0.0000, 0] => 0

        HalfAwayFromZero: result is same to HalfAwayFromZero
         HalfTowardsZero: result is same to HalfAwayFromZero
                HalfEven: result is same to HalfAwayFromZero
                 HalfOdd: result is same to HalfAwayFromZero
             TowardsZero: result is same to HalfAwayFromZero
            AwayFromZero: result is same to HalfAwayFromZero
        NegativeInfinity: result is same to HalfAwayFromZero
        PositiveInfinity: result is same to HalfAwayFromZero
