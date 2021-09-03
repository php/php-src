--TEST--
Generators can't be counted
--FILE--
<?php

function gen() { yield; }

$gen = gen();

try {
    count($gen);
} catch (\TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
count(): Argument #1 ($value) must be of type Countable|array, Generator given
