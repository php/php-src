--TEST--
Time\Duration::fromMilliseconds() (64 bit variation)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platforms only");
?>
--FILE--
<?php

require __DIR__ . '/../helper.inc';

echo f(Time\Duration::fromMilliseconds(9_223_372_035_999)), PHP_EOL;

try {
    Time\Duration::fromMilliseconds(9_223_372_036_000);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
+9223372035.999000000
Time\TimeException: The maximum representable range is 9_223_372_035 seconds (roughly 292 years)
