--TEST--
GH-9415: Randomizer::getInt(0, 2**32 - 1) with Mt19937 always returns 1
--FILE--
<?php

use Random\Randomizer;
use Random\Engine\Mt19937;

$randomizer = new Randomizer(new Mt19937(1234));
var_dump($randomizer->getInt(0, 2**32 - 1));

$randomizer = new Randomizer(new Mt19937());
var_dump($randomizer->getInt(0, 2**32 - 1) !== 1 || $randomizer->getInt(0, 2**32 - 1) !== $randomizer->getInt(0, 2**32 - 1));

?>
--EXPECT--
int(822569775)
bool(true)
