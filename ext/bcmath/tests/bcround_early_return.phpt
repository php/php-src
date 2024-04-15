--TEST--
bcround() function with early return
--EXTENSIONS--
bcmath
--FILE--
<?php
$otherModes = [
    'PHP_ROUND_HALF_DOWN',
    'PHP_ROUND_HALF_EVEN',
    'PHP_ROUND_HALF_ODD',
    'PHP_ROUND_FLOOR',
    'PHP_ROUND_CEILING',
    'PHP_ROUND_AWAY_FROM_ZERO',
    'PHP_ROUND_TOWARD_ZERO',
];

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
    'PHP_ROUND_HALF_UP' => [],
    'PHP_ROUND_HALF_DOWN' => [],
    'PHP_ROUND_HALF_EVEN' => [],
    'PHP_ROUND_HALF_ODD' => [],
    'PHP_ROUND_FLOOR' => [],
    'PHP_ROUND_CEIL' => [],
    'PHP_ROUND_AWAY_FROM_ZERO' => [],
    'PHP_ROUND_TOWARD_ZERO' => [],
];
foreach ($early_return_cases as [$num, $precision]) {
    $result = str_pad("[{$num}, {$precision}]", 33, ' ', STR_PAD_LEFT) . ' => ' . bcround($num, $precision, PHP_ROUND_HALF_UP) . "\n";
    echo $result;
    $results['PHP_ROUND_HALF_UP'][] = $result;
}

echo "\n";

foreach ($otherModes as $mode) {
    foreach ($early_return_cases as [$num, $precision]) {
        $result = str_pad("[{$num}, {$precision}]", 33, ' ', STR_PAD_LEFT) . ' => ' . bcround($num, $precision, constant($mode)) . "\n";
        $results[$mode][] = $result;
    }

    if ($results['PHP_ROUND_HALF_UP'] === $results[$mode]) {
        echo str_pad($mode, 24, ' ', STR_PAD_LEFT) . ": result is same to PHP_ROUND_HALF_UP\n";
    } else {
        echo str_pad($mode, 24, ' ', STR_PAD_LEFT) . ": result is not same to PHP_ROUND_HALF_UP, failed\n";
    }
}
?>
--EXPECT--
                        [123, -4] => 0
                 [123.123456, -4] => 0
                         [123, 1] => 123
                       [123.5, 1] => 123.5
                       [123.5, 2] => 123.5
 [123.0000000000000000000001, 22] => 123.0000000000000000000001
 [123.0000000000000000000001, 23] => 123.0000000000000000000001
                       [-123, -4] => 0
                [-123.123456, -4] => 0
                        [-123, 1] => -123
                      [-123.5, 1] => -123.5
                      [-123.5, 2] => -123.5
[-123.0000000000000000000001, 22] => -123.0000000000000000000001
[-123.0000000000000000000001, 23] => -123.0000000000000000000001
                           [0, 0] => 0
                         [0.0, 0] => 0
                      [0.0000, 0] => 0
                          [-0, 0] => 0
                        [-0.0, 0] => 0
                     [-0.0000, 0] => 0

     PHP_ROUND_HALF_DOWN: result is same to PHP_ROUND_HALF_UP
     PHP_ROUND_HALF_EVEN: result is same to PHP_ROUND_HALF_UP
      PHP_ROUND_HALF_ODD: result is same to PHP_ROUND_HALF_UP
         PHP_ROUND_FLOOR: result is same to PHP_ROUND_HALF_UP
       PHP_ROUND_CEILING: result is same to PHP_ROUND_HALF_UP
PHP_ROUND_AWAY_FROM_ZERO: result is same to PHP_ROUND_HALF_UP
   PHP_ROUND_TOWARD_ZERO: result is same to PHP_ROUND_HALF_UP
