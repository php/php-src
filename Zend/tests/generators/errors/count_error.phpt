--TEST--
Generators can't be counted
--FILE--
<?php

function gen() { yield; }

$gen = gen();

try {
    count($gen);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: count(): Argument #1 ($value) must be of type Countable|array, Generator given
