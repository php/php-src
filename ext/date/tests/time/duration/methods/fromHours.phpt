--TEST--
Time\Duration::fromHours()
--FILE--
<?php

require __DIR__ . '/../helper.inc';

echo f(Time\Duration::fromHours(0)), PHP_EOL;
echo f(Time\Duration::fromHours(1)), PHP_EOL;
echo f(Time\Duration::fromHours(596523)), PHP_EOL;

try {
    Time\Duration::fromHours(-1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
         +0.000000000
      +3600.000000000
+2147482800.000000000
ValueError: Time\Duration::fromHours(): Argument #1 ($hours) must be greater than or equal to 0
