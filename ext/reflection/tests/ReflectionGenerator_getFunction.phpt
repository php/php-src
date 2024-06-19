--TEST--
ReflectionGenerator::getFunction() is legal after termination.
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
    $ref = new ReflectionGenerator($gen);

    echo "Before: ", $ref->getFunction()->getName(), PHP_EOL;

    foreach ($gen as $dummy) {
        echo "Inside: ", $ref->getFunction()->getName(), PHP_EOL;
    }

    echo "After: ", $ref->getFunction()->getName(), PHP_EOL;
}

?>
--EXPECTF--
Before: a
Inside: a
After: a
Before: {closure:%s:%d}
Inside: {closure:%s:%d}
After: {closure:%s:%d}
Before: foo
Inside: foo
After: foo
