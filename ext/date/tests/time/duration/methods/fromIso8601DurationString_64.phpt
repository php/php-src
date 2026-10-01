--TEST--
Time\Duration::fromIso8601DurationString() (64 bit variation)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platforms only");
?>
--FILE--
<?php

require __DIR__ . '/../helper.inc';

$specifications = [
    'PT2147483648S',
    'PT9223372035S',
    'PT9223372036S',
];

foreach ($specifications as $specification) {
    printf("%-25s: ", $specification);

    try {
        echo f(Time\Duration::fromIso8601DurationString($specification)), PHP_EOL;
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

?>
--EXPECT--
PT2147483648S            : +2147483648.000000000
PT9223372035S            : +9223372035.000000000
PT9223372036S            : Time\TimeException: The maximum representable range is 9_223_372_035 seconds (roughly 292 years)
