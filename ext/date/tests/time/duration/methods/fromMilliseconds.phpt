--TEST--
Time\Duration::fromMilliseconds()
--FILE--
<?php

require __DIR__ . '/../helper.inc';

echo f(Time\Duration::fromMilliseconds(0)), PHP_EOL;
echo f(Time\Duration::fromMilliseconds(1)), PHP_EOL;
echo f(Time\Duration::fromMilliseconds(999)), PHP_EOL;
echo f(Time\Duration::fromMilliseconds(1_000)), PHP_EOL;
echo f(Time\Duration::fromMilliseconds(2147483_647)), PHP_EOL;

try {
    Time\Duration::fromMilliseconds(-1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
         +0.000000000
         +0.001000000
         +0.999000000
         +1.000000000
   +2147483.647000000
ValueError: Time\Duration::fromMilliseconds(): Argument #1 ($milliseconds) must be greater than or equal to 0
