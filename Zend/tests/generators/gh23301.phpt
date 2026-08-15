--TEST--
GH-23301 (Nested "yield from" yields a value twice when the middle generator ends with "yield from []")
--FILE--
<?php

function inner() {
    yield "B";
}

function middle() {
    yield "A";
    yield from inner();
    yield "C";
    yield from [];
}

function outer() {
    yield from middle();
}

foreach (outer() as $value) {
    echo $value, "\n";
}

?>
--EXPECT--
A
B
C
