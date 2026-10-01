--TEST--
Time\Duration: new
--FILE--
<?php

require __DIR__ . '/helper.inc';

try {
    new Time\Duration();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    (new ReflectionClass(Time\Duration::class))->newInstance();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    (new ReflectionClass(Time\Duration::class))->newInstanceWithoutConstructor();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: Call to private Time\Duration::__construct() from global scope
ReflectionException: Access to non-public constructor of class Time\Duration
ReflectionException: Class Time\Duration is an internal class marked as final that cannot be instantiated without invoking its constructor
