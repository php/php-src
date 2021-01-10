--TEST--
Test class: userland implemented classes.
--FILE--
<?php

class FixedNumberGenerator implements \RNG\RNGInterface
{
    protected int $count = 0;

    public function next(): int
    {
        return $this->count++;
    }

    public function next64(): int
    {
        return $this->count++ * 2;
    }
}

$rng = new FixedNumberGenerator();
$array = range(1, 100);

$rng->next() . PHP_EOL;
$rng->next64() . PHP_EOL;
array_rand($array, 1, $rng);
array_rand($array, 2, $rng);
shuffle($array, $rng);
str_shuffle('foobar', $rng);
rng_rand($rng, 1, 1000) . PHP_EOL;
rng_bytes($rng, 100) . PHP_EOL;

class XorShift128PlusEx extends \RNG\XorShift128Plus
{
    public function next(): int
    {
        return parent::next() + 1;
    }

    public function next64(): int
    {
        return parent::next() + 1;
    }
}

$origin = new \RNG\XorShift128Plus(12345);
$extend = new XorShift128PlusEx(12345);

for ($i = 0; $i < 100000; $i++) {
    $origin_next = $origin->next();
    $extend_next = $extend->next();

    if (($origin_next + 1) !== $extend_next) {
        die("NG, userland extended class is broken.");
    }
}

die('OK, userland implementation works correctly.');
?>
--EXPECT--
OK, userland implementation works correctly.
