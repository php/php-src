--TEST--
GH-15375 (Nested "yield from" skips items after valid()/next() on the inner generator)
--FILE--
<?php

function arrayProvider() {
    yield ['one', 'two', 'three'];
    yield ['four', 'five', 'six'];
    yield ['seven', 'eight', 'nine'];
}

function iterateValues(array $array) {
    foreach ($array as $value) {
        yield $value;
    }
}

function withValid() {
    foreach (arrayProvider() as $array) {
        $iterator = iterateValues($array);
        if ($iterator->valid()) {
            yield from $iterator;
        }
    }
}

function withNext() {
    foreach (arrayProvider() as $array) {
        $iterator = iterateValues($array);
        $iterator->next();
        yield from $iterator;
    }
}

function outer(Generator $inner) {
    yield from $inner;
}

echo "valid():\n";
foreach (outer(withValid()) as $s) {
    echo $s, "\n";
}

echo "next():\n";
foreach (outer(withNext()) as $s) {
    echo $s, "\n";
}

// A shared, pre-primed generator consumed through two nested "yield from"
// levels must still present its current value once to each consumer (the fix
// must not over-clear the middle level's first-touch).
echo "shared primed:\n";
function counter() {
    yield 1;
    yield 2;
}
$gen1 = counter();
$gen1->valid();
$gen2 = outer($gen1);
$gen3 = outer($gen2);
echo "gen3 current: ", $gen3->current(), "\n";
$gen2->next();
echo "gen2 current: ", $gen2->current(), "\n";
$gen2->next();
echo "gen2 current: ", $gen2->current(), "\n";

?>
--EXPECT--
valid():
one
two
three
four
five
six
seven
eight
nine
next():
two
three
five
six
eight
nine
shared primed:
gen3 current: 1
gen2 current: 1
gen2 current: 2
