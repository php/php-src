--TEST--
Test Random: methods
--FILE--
<?php

$rngs = [];
foreach (include __DIR__ . DIRECTORY_SEPARATOR . 'number_generator' . DIRECTORY_SEPARATOR . 'common.inc' as $klass => $is_clone_or_serializable) {
    $rngs[] = new $klass();
}

class UserRNG implements Random\NumberGenerator\RandomNumberGenerator
{
    protected int $current = 0;

    public function generate(): int
    {
        return ++$this->current;
    }
}
$rngs[] = new UserRNG();

foreach ($rngs as $rng) {
    $instance = new Random($rng);

    // nextInt
    $instance->nextInt();

    // getInt
    for ($i = 0; $i < 100; $i++) {
        $generated_int = $instance->getInt(-50, 50);
        if ($generated_int > 50 || $generated_int < -50) {
            die("failure: getInt: i: ${i}, generated: ${generated_int}, rng: " . $rng::class);
        }
    }

    // getBytes
    for ($i = 0; $i < 100; $i++) {
        $length = 1024;
        $generated_bytes = $instance->getBytes($length);
        $generated_length = strlen($generated_bytes);
        if ($generated_length !== $length) {
            die("failure: getBytes: i: ${i}, length: ${generated_length}, rng: " . $rng::class);
        }
    }

    // shuffleArray
    $array = range(1, 10000);
    $shuffled_array = $instance->shuffleArray($array);
    (function () use ($rng, $array, $shuffled_array): void {
        for ($i = 0; $i < count($array); $i++) {
            if ($array[$i] !== $shuffled_array[$i]) {
                return;
            }
        }

        die("failure: shuffleArray: rng: " . $rng::class);
    })();

    // shuffleString
    $string = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.';
    $shuffled_string = $instance->shuffleString($string);
    if ($string === $shuffled_string) {
        die("failure: shuffleString: rng: " . $rng::class);
    }
}

die('success');
?>
--EXPECT--
success
