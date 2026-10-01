--TEST--
Time\Duration::fromMicroseconds()
--FILE--
<?php

require __DIR__ . '/../helper.inc';

echo f(Time\Duration::fromMicroseconds(0)), PHP_EOL;
echo f(Time\Duration::fromMicroseconds(1)), PHP_EOL;
echo f(Time\Duration::fromMicroseconds(999999)), PHP_EOL;
echo f(Time\Duration::fromMicroseconds(1_000000)), PHP_EOL;
echo f(Time\Duration::fromMicroseconds(2147_483647)), PHP_EOL;

try {
    Time\Duration::fromMicroseconds(-1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
         +0.000000000
         +0.000001000
         +0.999999000
         +1.000000000
      +2147.483647000
ValueError: Time\Duration::fromMicroseconds(): Argument #1 ($microseconds) must be greater than or equal to 0
