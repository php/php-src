--TEST--
Generators can't be counted
--FILE--
<?php

function gen() { yield; }

$gen = gen();

try {
    count($gen);
} catch (Exception $e) {
    echo $e;
}

?>
--EXPECTF--
Warning: count(): Argument #1 ($var) must be of type Countable|array, Generator given in %s on line %d
