--TEST--
ReflectionGenerator::isClosed: ->valid() terminates an empty generator.
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
    var_dump($ref->isClosed());
    var_dump($gen->valid());
    var_dump($ref->isClosed());
    try {
        var_dump($ref->getExecutingLine());
    } catch (\Exception $e) {
        echo $e->getMessage(), PHP_EOL;
    }

    echo PHP_EOL;
}

?>
--EXPECT--
bool(false)
bool(true)
bool(false)
int(10)

bool(false)
bool(false)
bool(true)
Cannot fetch information from a closed Generator
