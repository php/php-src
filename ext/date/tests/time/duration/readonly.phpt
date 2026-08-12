--TEST--
Time\Duration: readonly
--FILE--
<?php

require __DIR__ . '/helper.inc';

$d = Time\Duration::fromSeconds(1);

try {
    $d->seconds = 2;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo f($d), PHP_EOL;

try {
    (new ReflectionProperty($d, 'seconds'))->setValue($d, 2);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo f($d), PHP_EOL;

try {
    (new ReflectionProperty($d, 'seconds'))->setRawValue($d, 2);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo f($d), PHP_EOL;

try {
    (new ReflectionProperty($d, 'seconds'))->setRawValueWithoutLazyInitialization($d, 2);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo f($d), PHP_EOL;

echo "====", PHP_EOL;

/* Recheck after cloning to verify that "modification allowed during cloning" has no lasting effect. */

$d = clone($d);

try {
    $d->seconds = 2;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo f($d), PHP_EOL;

try {
    (new ReflectionProperty($d, 'seconds'))->setValue($d, 2);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo f($d), PHP_EOL;

try {
    (new ReflectionProperty($d, 'seconds'))->setRawValue($d, 2);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo f($d), PHP_EOL;

try {
    (new ReflectionProperty($d, 'seconds'))->setRawValueWithoutLazyInitialization($d, 2);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo f($d), PHP_EOL;

echo "====", PHP_EOL;

try {
    clone($d, ['seconds' => 2]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "====", PHP_EOL;

var_dump((new ReflectionProperty($d, 'seconds'))->isWritable(null, $d));

echo "====", PHP_EOL;

try {
    (new ReflectionMethod($d, '__construct'))->invoke($d);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo f($d), PHP_EOL;

?>
--EXPECT--
Error: Cannot modify readonly property Time\Duration::$seconds
         +1.000000000
Error: Cannot modify readonly property Time\Duration::$seconds
         +1.000000000
Error: Cannot modify readonly property Time\Duration::$seconds
         +1.000000000
Error: Cannot modify readonly property Time\Duration::$seconds
         +1.000000000
====
Error: Cannot modify readonly property Time\Duration::$seconds
         +1.000000000
Error: Cannot modify readonly property Time\Duration::$seconds
         +1.000000000
Error: Cannot modify readonly property Time\Duration::$seconds
         +1.000000000
Error: Cannot modify readonly property Time\Duration::$seconds
         +1.000000000
====
Error: Cannot modify protected(set) readonly property Time\Duration::$seconds from global scope
====
bool(false)
====
Error: Cannot directly construct Time\Duration, use Time\Duration::from*() methods instead
         +1.000000000
