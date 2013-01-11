--TEST--
Cloning a generator after an object method was called
--FILE--
<?php

class A { public function b() { } }

function gen() {
    $a = new A;
    $a->b();
    yield;
}

$g1 = gen();
$g1->rewind();
$g2 = clone $g1; 

echo "Done";
--EXPECT--
Done
