--TEST--
Time\Duration::fromMinutes()
--FILE--
<?php

require __DIR__ . '/../helper.inc';

echo f(Time\Duration::fromMinutes(0)), PHP_EOL;
echo f(Time\Duration::fromMinutes(1)), PHP_EOL;
echo f(Time\Duration::fromMinutes(35791394)), PHP_EOL;

try {
    Time\Duration::fromMinutes(-1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
         +0.000000000
        +60.000000000
+2147483640.000000000
ValueError: Time\Duration::fromMinutes(): Argument #1 ($minutes) must be greater than or equal to 0
