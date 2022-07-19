--TEST--
Random: Randomizer: basic
--FILE--
<?php

$engines = [];
$engines[] = new Random\Engine\Mt19937(\random_int(\PHP_INT_MIN, \PHP_INT_MAX), MT_RAND_MT19937);
$engines[] = new Random\Engine\Mt19937(\random_int(\PHP_INT_MIN, \PHP_INT_MAX), MT_RAND_PHP);
$engines[] = new Random\Engine\PcgOneseq128XslRr64(\random_int(\PHP_INT_MIN, \PHP_INT_MAX));
$engines[] = new Random\Engine\Xoshiro256StarStar(\random_int(\PHP_INT_MIN, \PHP_INT_MAX));
$engines[] = new Random\Engine\Secure(); 
$engines[] = new class () implements Random\Engine {
    public function generate(): string
    {
        return \random_bytes(16);
    }
};
class UserEngine implements Random\Engine
{
    public function generate(): string
    {
        return \random_bytes(16);
    }
}
$engines[] = new UserEngine();

foreach ($engines as $engine) {
    $randomizer = new Random\Randomizer($engine);

    // nextInt
    for ($i = 0; $i < 1000; $i++) {
        try {
            $randomizer->nextInt();
        } catch (\RuntimeException $e) {
            if ($e->getMessage() !== 'Generated value exceeds size of int') {
                die($engine::class . ': nextInt: failure: {$e->getMesasge()}');
                throw $e;
            }
        }
    }

    // getInt
    for ($i = 0; $i < 1000; $i++) {
        $result = $randomizer->getInt(-50, 50);
        if ($result > 50 || $result < -50) {
            die($engine::class . ': getInt: failure');
        }
    }

    // getBytes
    for ($i = 0; $i < 1000; $i++) {
        $length = \random_int(1, 1024);
        if (\strlen($randomizer->getBytes($length)) !== $length) {
            die($engine::class . ': getBytes: failure.');
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

        die($engine::class . ': shuffleArray: failure.');
    })();

    // shuffleBytes
    $string = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.';
    $shuffled_string = $randomizer->shuffleBytes($string);
    if ($string === $shuffled_string) {
        die($engine::class . ': shuffleBytes: failure.');
    }
}

die('success');
?>
--EXPECTF--
success
