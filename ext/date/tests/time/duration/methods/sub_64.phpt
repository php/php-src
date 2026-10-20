--TEST--
Time\Duration::sub() (64 bit variation)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platforms only");
?>
--FILE--
<?php

require __DIR__ . '/../helper.inc';

$a = Time\Duration::fromSeconds(0, 1)->negate();
$b = Time\Duration::fromSeconds(9_223_372_035, 999999999);

try {
    $a->sub($b);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Time\TimeException: The maximum representable range is 9_223_372_035 seconds (roughly 292 years)
