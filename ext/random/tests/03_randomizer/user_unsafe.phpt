--TEST--
Random: Randomizer: User: Engine unsafe
--FILE--
<?php

// Empty generator
$randomizer = (new \Random\Randomizer(
    new class () implements \Random\Engine {
        public function generate(): string
        {
            return '';
        }
    }
));

try {
    var_dump($randomizer->getInt(\PHP_INT_MIN, \PHP_INT_MAX));
} catch (\RuntimeException $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    var_dump(bin2hex($randomizer->getBytes(1)));
} catch (\RuntimeException $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    var_dump($randomizer->shuffleArray(\range(1, 10)));
} catch (\RuntimeException $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    var_dump($randomizer->shuffleBytes('foobar'));
} catch (\RuntimeException $e) {
    echo $e->getMessage() . PHP_EOL;
}

// Infinite loop
$randomizer = (new \Random\Randomizer(
    new class () implements \Random\Engine {
        public function generate(): string
        {
            return "\xff\xff\xff\xff\xff\xff\xff\xff";
        }
    }
));

try {
    var_dump($randomizer->getInt(\PHP_INT_MIN, \PHP_INT_MAX));
} catch (\RuntimeException $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    var_dump(bin2hex($randomizer->getBytes(1)));
} catch (\RuntimeException $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    var_dump($randomizer->shuffleArray(\range(1, 10)));
} catch (\RuntimeException $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    var_dump($randomizer->shuffleBytes('foobar'));
} catch (\RuntimeException $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECTF--
Random number generation failed
Random number generation failed
Random number generation failed
Random number generation failed
int(%d)
string(2) "ff"
Random number generation failed
Random number generation failed
