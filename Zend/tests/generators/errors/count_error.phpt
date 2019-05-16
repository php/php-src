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
Warning: count(): Parameter must be an array or an object that implements Countable in %s on line %d
