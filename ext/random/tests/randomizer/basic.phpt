--TEST--
Random: Randomizer: 
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
    $randomizer = new Random\Randomizer($generator);

    // getInt
    for ($i = 0; $i < 1000; $i++) {
        $result = $randomizer->getInt(-50, 50);
        if ($result > 50 || $result < -50) {
            die($generator::class . ': getInt: failure');
        }
    }

    // getBytes
    for ($i = 0; $i < 1000; $i++) {
        if (\strlen($randomizer->getBytes(1024)) !== 1024) {
            die($generator::class . ': getBytes: failure.');
        }
    }

    // shuffleArray
    $array = range(1, 1000);
    $shuffled_array = $randomizer->shuffleArray($array);
    (function () use ($array, $shuffled_array): void {
        for ($i = 0; $i < count($array); $i++) {
            if ($array[$i] !== $shuffled_array[$i]) {
                return;
            }
        }

        die($generator::class . ': shuffleArray: failure.');
    })();

    // shuffleString
    $string = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.';
    $shuffled_string = $randomizer->shuffleString($string);
    if ($string === $shuffled_string) {
        die($generator::class . ': shuffleString: failure.');
    }
}

die('success');
?>
--EXPECTF--
success
