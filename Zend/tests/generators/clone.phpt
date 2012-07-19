--TEST--
Generators can be cloned
--FILE--
<?php

function firstN($end) {
    for ($i = 0; $i < $end; ++$i) {
        yield $i;
    }
}

$g1 = firstN(5);
var_dump($g1->current());
$g1->next();

$g2 = clone $g1;
var_dump($g2->current());
$g2->next();

var_dump($g2->current());
var_dump($g1->current());

$g1->next();
var_dump($g1->current());

?>
--EXPECT--
int(0)
int(1)
int(2)
int(1)
int(2)
