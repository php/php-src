--TEST--
Time\Duration::sub() (32 bit variation)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platforms only");
?>
--FILE--
<?php

require __DIR__ . '/../helper.inc';

$a = Time\Duration::fromSeconds(0, 1)->negate();
$b = Time\Duration::fromSeconds(2_147_483_647, 999999999);

try {
    $a->sub($b);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Time\TimeException: The maximum representable range is 2_147_483_647 seconds (roughly 68 years)
