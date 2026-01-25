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
/*
 * Bug: strtotime("2.37685 weeks") returns incorrect result
 *
 * Before fix:
 *   - "2.37" is matched as time (hour:minute with dot separator)
 *   - "685 weeks" is parsed as relative offset
 *   - Result: 685 weeks + 2:37 = ~414 million seconds (WRONG)
 *
 * After fix:
 *   - "2.37685 weeks" is matched as decimal relative
 *   - Result: 2.37685 * 7 * 86400 = ~1,437,030 seconds (CORRECT)
 */

$base = 0;

echo "=== Bug #21027: Decimal weeks parsing ===\n";

$input = "2.37685 weeks";
$result = strtotime($input, $base);

$expected = (int)(2.37685 * 604800);

$buggy = 685 * 604800 + 2 * 3600 + 37 * 60;

echo "Input: '$input'\n";
echo "Result: $result\n";
echo "Expected: ~$expected\n";

if (abs($result - $expected) < 10) {
    echo "PASS: Decimal weeks parsed correctly\n";
} else if (abs($result - $buggy) < 100) {
    echo "FAIL: Parser split '2.37685 weeks' into '02:37' + '685 weeks'\n";
} else {
    echo "FAIL: Unexpected result (diff from expected: " . abs($result - $expected) . ")\n";
}

echo "\n=== Additional decimal relative tests ===\n";

$tests = [
    '1.5 weeks' => 1.5 * 604800,
    '0.5 weeks' => 0.5 * 604800,
    '2.5 days' => 2.5 * 86400,
    '1.5 hours' => 1.5 * 3600,
    '2.25 minutes' => 2.25 * 60,
    '1.5 seconds' => 1.5,
    '-1.5 days' => -1.5 * 86400,
    '-2.5 weeks' => -2.5 * 604800,
];

foreach ($tests as $input => $expected) {
    $result = strtotime($input, $base);
    $expected_int = (int)$expected;
    $diff = abs($result - $expected_int);
    if ($diff <= 1) {
        echo "OK: strtotime('$input') = $result\n";
    } else {
        echo "FAIL: strtotime('$input') = $result (expected ~$expected_int, diff=$diff)\n";
    }
}

echo "\nDone.\n";
?>
--EXPECT--
=== Bug #21027: Decimal weeks parsing ===
Input: '2.37685 weeks'
Result: 1437029
Expected: ~1437029
PASS: Decimal weeks parsed correctly

=== Additional decimal relative tests ===
OK: strtotime('1.5 weeks') = 907200
OK: strtotime('0.5 weeks') = 302400
OK: strtotime('2.5 days') = 216000
OK: strtotime('1.5 hours') = 5400
OK: strtotime('2.25 minutes') = 135
OK: strtotime('1.5 seconds') = 1
OK: strtotime('-1.5 days') = -129600
OK: strtotime('-2.5 weeks') = -1512000

Done.
