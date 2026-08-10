--TEST--
Time\Duration::fromSeconds()
--FILE--
<?php

require __DIR__ . '/../helper.inc';

echo f(Time\Duration::fromSeconds(0)), PHP_EOL;
echo f(Time\Duration::fromSeconds(1)), PHP_EOL;
echo f(Time\Duration::fromSeconds(2147483647)), PHP_EOL;

echo f(Time\Duration::fromSeconds(0, 0)), PHP_EOL;
echo f(Time\Duration::fromSeconds(0, 1)), PHP_EOL;
echo f(Time\Duration::fromSeconds(1, 1)), PHP_EOL;
echo f(Time\Duration::fromSeconds(2147483647, 999999999)), PHP_EOL;

try {
    Time\Duration::fromSeconds(-1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    Time\Duration::fromSeconds(0, -1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    Time\Duration::fromSeconds(0, 1000000000);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
         +0.000000000
         +1.000000000
+2147483647.000000000
         +0.000000000
         +0.000000001
         +1.000000001
+2147483647.999999999
ValueError: Time\Duration::fromSeconds(): Argument #1 ($seconds) must be greater than or equal to 0
ValueError: Time\Duration::fromSeconds(): Argument #2 ($nanoseconds) must be greater than or equal to 0
ValueError: Time\Duration::fromSeconds(): Argument #2 ($nanoseconds) must be less than 1_000_000_000
