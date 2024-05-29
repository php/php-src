--TEST--
ReflectionGenerator::isClosed
--FILE--
<?php

function empty_generator() {
    return;
    yield;
}

$gens = [
    (function() {
        yield;
    })(),
    empty_generator(),
];

foreach ($gens as $gen) {
    $ref = new ReflectionGenerator($gen);
    var_dump($ref->getExecutingLine());
    var_dump($ref->isClosed());
    var_dump($ref->getExecutingLine());
    foreach ($gen as $dummy);
    var_dump($ref->isClosed());
    try {
        var_dump($ref->getExecutingLine());
    } catch (\Exception $e) {
        echo $e->getMessage(), PHP_EOL;
    }

    echo PHP_EOL;
}

?>
--EXPECTF--
int(10)
bool(false)
int(10)
bool(true)
Cannot fetch information from a closed Generator

int(4)
bool(false)
int(4)
bool(true)
Cannot fetch information from a closed Generator
