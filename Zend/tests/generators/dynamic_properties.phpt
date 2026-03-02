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
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot create dynamic property Generator::$prop
