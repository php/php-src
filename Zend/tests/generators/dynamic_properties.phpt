--TEST--
It's not possible to assign dynamic properties on a generator
--FILE--
<?php

function gen() {
    yield;
}

$gen = gen();
try {
    $gen->prop = new stdClass;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot create dynamic property Generator::$prop
