--TEST--
Generators cannot be cloned
--FILE--
<?php

function gen() {
    yield;
}


try {
    $gen = gen();
    clone $gen;
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: Trying to clone an uncloneable object of class Generator
