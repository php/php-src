--TEST--
Random: Randomizer: 
--FILE--
<?php

$generators = [];
$generators[] = new Random\NumberGenerator\XorShift128Plus(\random_int(\PHP_INT_MIN, \PHP_INT_MAX));
$generators[] = new Random\NumberGenerator\MersenneTwister(\random_int(\PHP_INT_MIN, \PHP_INT_MAX), MT_RAND_MT19937);
$generators[] = new Random\NumberGenerator\MersenneTwister(\random_int(\PHP_INT_MIN, \PHP_INT_MAX), MT_RAND_PHP);
$generators[] = new Random\NumberGenerator\CombinedLCG(\random_int(\PHP_INT_MIN, \PHP_INT_MAX));
$generators[] = new class () implements Random\NumberGenerator {
    private int $count = 0;

    public function generate(): int
    {
        return ++$this->count;
    }
};
class UserNumberGenerator implements Random\NumberGenerator
{
    private int $count = 0;

    public function generate(): int
    {
        return ++$this->count;
    }
}
$generators[] = new UserNumberGenerator();

foreach ($generators as $generator) {
    $randomizer = new Random\Randomizer($generator);
    $randomizer->getInt(\PHP_INT_MIN, \PHP_INT_MAX);
    try {
        $randomizer2 = unserialize(serialize($randomizer));
    } catch (\Exception $e) {
        echo $e->getMessage() . PHP_EOL;
        continue;
    }

    if ($randomizer->getInt(\PHP_INT_MIN, \PHP_INT_MAX) !== $randomizer2->getInt(\PHP_INT_MIN, \PHP_INT_MAX)) {
        die($generator::class . ': failure.');
    }

    echo $generator::class . ': success' . PHP_EOL;
}

die('success');
?>
--EXPECTF--
Random\NumberGenerator\XorShift128Plus: success
Random\NumberGenerator\MersenneTwister: success
Random\NumberGenerator\MersenneTwister: success
Random\NumberGenerator\CombinedLCG: success
Serialization of 'Random\NumberGenerator@anonymous' is not allowed
UserNumberGenerator: success
success
