--TEST--
GH-9415: Randomizer::getInt(0, 2**32 - 1) with Mt19937 always returns 1
--FILE--
<?php

use Random\Randomizer;
use Random\Engine\Mt19937;

$randomizer = new Randomizer(new Mt19937(1234));
// Parameters shifted by -2147483648 to be compatible with 32-bit.
var_dump($randomizer->getInt(-2147483648, 2147483647));

$randomizer = new Randomizer(new Mt19937(4321));
var_dump($randomizer->getInt(-2147483648, 2147483647));

?>
--EXPECT--
int(-1324913873)
int(-1843387587)
