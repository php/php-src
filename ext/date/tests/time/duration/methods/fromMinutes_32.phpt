--TEST--
Time\Duration::fromMinutes() (32 bit variation)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platforms only");
?>
--FILE--
<?php

require __DIR__ . '/../helper.inc';

echo f(Time\Duration::fromMinutes(35791394)), PHP_EOL;

try {
    echo f(Time\Duration::fromMinutes(35791395)), PHP_EOL;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
+2147483640.000000000
Time\TimeException: The maximum representable range is 2_147_483_647 seconds (roughly 68 years)
