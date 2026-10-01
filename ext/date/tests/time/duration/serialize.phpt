--TEST--
Time\Duration: serialize()
--FILE--
<?php

require __DIR__ . '/helper.inc';

var_dump($serialized = serialize(Time\Duration::fromSeconds(1, 2)->negate()));
echo f($unserialized = unserialize($serialized)), PHP_EOL;
var_dump(serialize($unserialized));
echo f($unserialized->add($unserialized)), PHP_EOL;

try {
    // $negative is not bool, but coercible.
    echo f(unserialize('O:13:"Time\Duration":3:{s:7:"seconds";i:1;s:11:"nanoseconds";i:1;s:8:"negative";i:999;}')), PHP_EOL;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    // $negative is not bool and not coercible.
    unserialize('O:13:"Time\Duration":3:{s:7:"seconds";i:1;s:11:"nanoseconds";i:1;s:8:"negative";N;}');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    // $seconds is negative.
    unserialize('O:13:"Time\Duration":3:{s:7:"seconds";i:-1;s:11:"nanoseconds";i:1;s:8:"negative";b:0;}');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    // Dynamic property.
    unserialize('O:13:"Time\Duration":4:{s:7:"seconds";i:1;s:11:"nanoseconds";i:1;s:8:"negative";b:0;s:3:"foo";N;}');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}


try {
    // Out of range nanoseconds
    unserialize('O:13:"Time\Duration":3:{s:7:"seconds";i:1;s:11:"nanoseconds";i:1000000000;s:8:"negative";b:0;}');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    Time\Duration::fromSeconds(1, 1)
        ->__unserialize([
            'seconds' => 2,
            'nanoseconds' => 2,
            'negative' => true,
        ]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
string(85) "O:13:"Time\Duration":3:{s:7:"seconds";i:1;s:11:"nanoseconds";i:2;s:8:"negative";b:1;}"
         -1.000000002
string(85) "O:13:"Time\Duration":3:{s:7:"seconds";i:1;s:11:"nanoseconds";i:2;s:8:"negative";b:1;}"
         -2.000000004
Exception: Invalid serialization data for Time\Duration object
Exception: Invalid serialization data for Time\Duration object
Exception: Invalid serialization data for Time\Duration object
Exception: Invalid serialization data for Time\Duration object
Exception: Invalid serialization data for Time\Duration object
Exception: Invalid serialization data for Time\Duration object
