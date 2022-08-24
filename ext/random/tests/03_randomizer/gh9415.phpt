--TEST--
GH-9415: Randomizer::getInt(0, 2**32 - 1) with Mt19937 always returns 1
--FILE--
<?php

use Random\Randomizer;
use Random\Engine\Mt19937;

$r = new Randomizer(new Mt19937(1234));
var_dump($r->getInt(0, 2**32 - 1));

$r = new Randomizer(new Mt19937());
var_dump($r->getInt(0, 2**32 - 1) !== 1 || $r->getInt(0, 2**32 - 1) !== $r->getInt(0, 2**32 - 1));

?>
--EXPECT--
int(822569775)
bool(true)
