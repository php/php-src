--TEST--
Random: Randomizer: nextInt() throws for too large values on 32 Bit
--SKIPIF--
<?php if (PHP_INT_SIZE == 8) die("skip 32-bit only"); ?>
--FILE--
<?php

$randomizer = new \Random\Randomizer(new \Random\Engine\Xoshiro256StarStar());

try {
	var_dump($randomizer->nextInt());
} catch (\Random\RandomException $e) {
	echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Generated value exceeds size of int
