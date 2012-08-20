--TEST--
Cloning a generator with a foreach loop properly adds a ref for the loop var
--FILE--
<?php

function gen() {
    foreach ([1, 2, 3] as $i) {
        yield $i;
    }
}

$g1 = gen();
var_dump($g1->current());

$g2 = clone $g1;
var_dump($g2->current());

$g1->next();
$g2->next();
var_dump($g1->current());
var_dump($g2->current());

unset($g1);
$g2->next();
var_dump($g2->current());

?>
--EXPECT--
int(1)
int(1)
int(2)
int(2)
int(3)
