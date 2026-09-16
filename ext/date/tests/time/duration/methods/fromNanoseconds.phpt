--TEST--
Time\Duration::fromNanoseconds()
--FILE--
<?php

require __DIR__ . '/../helper.inc';

echo f(Time\Duration::fromNanoseconds(0)), PHP_EOL;
echo f(Time\Duration::fromNanoseconds(1)), PHP_EOL;
echo f(Time\Duration::fromNanoseconds(999999999)), PHP_EOL;
echo f(Time\Duration::fromNanoseconds(1_000000000)), PHP_EOL;
echo f(Time\Duration::fromNanoseconds(2_147483647)), PHP_EOL;

try {
    Time\Duration::fromNanoseconds(-1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
         +0.000000000
         +0.000000001
         +0.999999999
         +1.000000000
         +2.147483647
ValueError: Time\Duration::fromNanoseconds(): Argument #1 ($nanoseconds) must be greater than or equal to 0
