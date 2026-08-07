--TEST--
Time\Duration::multiplyBy() (64 bit variation)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platforms only");
?>
--FILE--
<?php

require __DIR__ . '/../helper.inc';

$d = Time\Duration::fromSeconds(9_223_372_035, 999999999);

echo f($d->multiplyBy(1)), PHP_EOL;

try {
    $d->multiplyBy(2);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "====", PHP_EOL;

$d = Time\Duration::fromSeconds(4_611_686_017, 999999999);

echo f($d->multiplyBy(2)), PHP_EOL;

try {
    $d->multiplyBy(3);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "====", PHP_EOL;

$d = Time\Duration::fromSeconds(3_074_457_345, 333333333);

echo f($d->multiplyBy(3)), PHP_EOL;

try {
    $d->multiplyBy(4);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "====", PHP_EOL;

$d = Time\Duration::fromSeconds(0, 1);

echo f($d->multiplyBy(9_223_372_035_999999999)), PHP_EOL;

try {
    $d->multiplyBy(9_223_372_036_000000000);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
+9223372035.999999999
Time\TimeException: The maximum representable range is 9_223_372_035 seconds (roughly 292 years)
====
+9223372035.999999998
Time\TimeException: The maximum representable range is 9_223_372_035 seconds (roughly 292 years)
====
+9223372035.999999999
Time\TimeException: The maximum representable range is 9_223_372_035 seconds (roughly 292 years)
====
+9223372035.999999999
Time\TimeException: The maximum representable range is 9_223_372_035 seconds (roughly 292 years)
