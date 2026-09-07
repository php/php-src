--TEST--
GH-23301 (Nested "yield from" yields a value twice when the middle generator delegates again)
--FILE--
<?php

function inner() {
    yield "B";
}

function middle() {
    yield "A";
    yield from inner();
    yield "C";
    yield from ["D"];
}

function delegate($gen) {
    yield from $gen;
}

foreach (delegate(middle()) as $value) {
    echo $value, "\n";
}

function tail($inner) {
    yield from $inner;
    yield from ["E"];
}

$middle = tail(inner());
$outer = delegate($middle);

var_dump($outer->current());
$middle->next();
var_dump($middle->current());

?>
--EXPECT--
A
B
C
D
string(1) "B"
string(1) "E"
