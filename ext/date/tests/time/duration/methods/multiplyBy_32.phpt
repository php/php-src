--TEST--
Time\Duration::multiplyBy() (32 bit variation)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platforms only");
?>
--FILE--
<?php

require __DIR__ . '/../helper.inc';

$d = Time\Duration::fromSeconds(2_147_483_647, 999999999);

echo f($d->multiplyBy(1)), PHP_EOL;

try {
    $d->multiplyBy(2);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "====", PHP_EOL;

$d = Time\Duration::fromSeconds(1_073_741_823, 999999999);

echo f($d->multiplyBy(2)), PHP_EOL;

try {
    $d->multiplyBy(3);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "====", PHP_EOL;

$d = Time\Duration::fromSeconds(715_827_882, 666666666);

echo f($d->multiplyBy(3)), PHP_EOL;

try {
    $d->multiplyBy(4);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
+2147483647.999999999
Time\TimeException: The maximum representable range is 2_147_483_647 seconds (roughly 68 years)
====
+2147483647.999999998
Time\TimeException: The maximum representable range is 2_147_483_647 seconds (roughly 68 years)
====
+2147483647.999999998
Time\TimeException: The maximum representable range is 2_147_483_647 seconds (roughly 68 years)
