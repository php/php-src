--TEST--
Tests cloning a generator with properties
--FILE--
<?php

function gen() { yield; }

$g1 = gen();
$g1->prop = 'val';

$g2 = clone $g1;
unset($g1);

var_dump($g2->prop);

?>
--EXPECT--
string(3) "val"
