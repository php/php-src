--TEST--
Bug #21027: strtotime() decimal weeks parsing issue
--DESCRIPTION--
strtotime() incorrectly interprets decimal week values like "2.37685 weeks".
The parser misinterprets it as "02:37 (hours) + 685 weeks" instead of treating
it as a single decimal value of 2.37685 weeks.
--INI--
date.timezone=UTC
--FILE--
<?php
$base = 0;

echo "=== Bug #21027: Decimal weeks parsing ===\n";

$input = "2.37685 weeks";
$result = strtotime($input, $base);

$buggy = 685 * 604800 + 2 * 3600 + 37 * 60;

echo "Input: '$input'\n";

if ($result > 1000000 && $result < 2000000) {
    echo "PASS: Decimal weeks parsed correctly (result ~1.4M seconds)\n";
} else if ($result > 400000000) {
    echo "FAIL: Parser split '2.37685 weeks' into '02:37' + '685 weeks'\n";
} else {
    echo "FAIL: Unexpected result: $result\n";
}

echo "\n=== Additional decimal relative tests ===\n";

$tests = [
    '1.5 weeks' => [907000, 908000],
    '0.5 weeks' => [302000, 303000],
    '2.5 days' => [215000, 217000],
    '1.5 hours' => [5400, 5401],
    '2.25 minutes' => [135, 136],
    '-1.5 days' => [-130000, -129000],
    '-2.5 weeks' => [-1513000, -1511000],
];

foreach ($tests as $input => $range) {
    $result = strtotime($input, $base);
    if ($result >= $range[0] && $result <= $range[1]) {
        echo "OK: strtotime('$input')\n";
    } else {
        echo "FAIL: strtotime('$input') = $result (expected {$range[0]} to {$range[1]})\n";
    }
}

echo "\nDone.\n";
?>
--EXPECT--
=== Bug #21027: Decimal weeks parsing ===
Input: '2.37685 weeks'
PASS: Decimal weeks parsed correctly (result ~1.4M seconds)

=== Additional decimal relative tests ===
OK: strtotime('1.5 weeks')
OK: strtotime('0.5 weeks')
OK: strtotime('2.5 days')
OK: strtotime('1.5 hours')
OK: strtotime('2.25 minutes')
OK: strtotime('-1.5 days')
OK: strtotime('-2.5 weeks')

Done.
