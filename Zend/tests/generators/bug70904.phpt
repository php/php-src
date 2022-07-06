--TEST--
Bug #70904 (yield from incorrectly marks valid generator as finished)
--FILE--
<?php

function g1() {
    yield 1;
}

function g2($g1) {
    yield from $g1;
    echo "reached!\n";
    yield 2;
}

$g1 = g1();
$g2 = g2($g1);

var_dump($g2->valid());
var_dump($g2->current());
$g1->next();
var_dump($g1->valid());
var_dump($g2->valid());
var_dump($g2->current());
$g2->next();
var_dump($g2->valid());
var_dump($g2->current());
$g2->next();
var_dump($g2->valid());
var_dump($g2->current());

?>
--EXPECT--
bool(true)
int(1)
bool(false)
bool(true)
int(1)
reached!
bool(true)
int(2)
bool(false)
NULL
