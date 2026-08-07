--TEST--
Time\Duration::fromSeconds() (64 bit variation)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platforms only");
?>
--FILE--
<?php

require __DIR__ . '/../helper.inc';

echo f(Time\Duration::fromSeconds(9_223_372_035)), PHP_EOL;
echo f(Time\Duration::fromSeconds(9_223_372_035, 999999999)), PHP_EOL;

try {
    Time\Duration::fromSeconds(9_223_372_036);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
+9223372035.000000000
+9223372035.999999999
Time\TimeException: The maximum representable range is 9_223_372_035 seconds (roughly 292 years)
