--TEST--
Random: NumberGenerator: clone
--FILE--
<?php

$generators = [];
$generators[] = new Random\NumberGenerator\XorShift128Plus(\random_int(\PHP_INT_MIN, \PHP_INT_MAX));
$generators[] = new Random\NumberGenerator\MersenneTwister(\random_int(\PHP_INT_MIN, \PHP_INT_MAX), MT_RAND_MT19937);
$generators[] = new Random\NumberGenerator\MersenneTwister(\random_int(\PHP_INT_MIN, \PHP_INT_MAX), MT_RAND_PHP);
$generators[] = new Random\NumberGenerator\CombinedLCG(\random_int(\PHP_INT_MIN, \PHP_INT_MAX));
$generators[] = new Random\NumberGenerator\Secure(); 
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
    $generator->generate();

    try {
        $clone_generator = clone $generator;
    } catch (Throwable $e) {
        echo $e->getMessage() . PHP_EOL;
        continue;
    }

    echo $generator::class . ': ';
    if ($generator->generate() === $clone_generator->generate()) {
        echo "success\n";
    } else {
        echo "failure\n";
    }
}

?>
--EXPECTF--
Random\NumberGenerator\XorShift128Plus: success
Random\NumberGenerator\MersenneTwister: success
Random\NumberGenerator\MersenneTwister: success
Random\NumberGenerator\CombinedLCG: success
Trying to clone an uncloneable object of class Random\NumberGenerator\Secure
Random\NumberGenerator@anonymous%s: success
UserNumberGenerator: success
