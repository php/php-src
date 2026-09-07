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

// Reading a pre-primed generator through two nested "yield from" levels primes
// the whole chain but advances nothing. A later next() on a middle level then
// advances the shared generator, like any other next() would.
echo "shared primed:\n";
function counter() {
    yield 1;
    yield 2;
}
$gen1 = counter();
$gen1->valid();
$gen2 = outer($gen1);
$gen3 = outer($gen2);
var_dump($gen3->current());
$gen2->next();
var_dump($gen2->current());
$gen2->next();
var_dump($gen2->current());

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
int(1)
int(2)
NULL
