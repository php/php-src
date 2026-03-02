--TEST--
Random: Randomizer: nextInt(): Throws for 64 bit engines on 32 bit platforms
--SKIPIF--
<?php if (PHP_INT_SIZE == 8) die("skip 32-bit only"); ?>
--FILE--
<?php

use Random\Randomizer;
use Random\Engine\Xoshiro256StarStar;

$randomizer = new Randomizer(new Xoshiro256StarStar());

try {
    var_dump($randomizer->nextInt());
} catch (Random\RandomException $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Generated value exceeds size of int
