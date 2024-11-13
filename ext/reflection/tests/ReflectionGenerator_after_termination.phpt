--TEST--
Creating ReflectionGenerator is legal after termination.
--FILE--
<?php

function foo() {
    yield;
}

$gens = [
    (new class() {
        function a() {
            yield from foo();
        }
    })->a(),
    (function() {
        yield;
    })(),
    foo(),
];

foreach ($gens as $gen) {
    foreach ($gen as $dummy);

    $ref = new ReflectionGenerator($gen);
    echo $ref->getFunction()->getName(), PHP_EOL;
}

?>
--EXPECTF--
a
{closure:%s:%d}
foo
