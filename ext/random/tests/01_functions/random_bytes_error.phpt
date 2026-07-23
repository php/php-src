--TEST--
Test error operation of random_bytes()
--FILE--
<?php
//-=-=-=-

try {
    $bytes = random_bytes();
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $bytes = random_bytes(0);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ArgumentCountError: random_bytes() expects exactly 1 argument, 0 given
ValueError: random_bytes(): Argument #1 ($length) must be greater than 0
