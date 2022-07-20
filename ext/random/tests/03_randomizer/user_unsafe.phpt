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
    echo $e, PHP_EOL;
}

echo PHP_EOL, "-------", PHP_EOL, PHP_EOL;

try {
    var_dump(bin2hex($randomizer->getBytes(1)));
} catch (\RuntimeException $e) {
    echo $e, PHP_EOL;
}

echo PHP_EOL, "-------", PHP_EOL, PHP_EOL;

try {
    var_dump($randomizer->shuffleArray(\range(1, 10)));
} catch (\RuntimeException $e) {
    echo $e, PHP_EOL;
}

echo PHP_EOL, "-------", PHP_EOL, PHP_EOL;

try {
    var_dump($randomizer->shuffleBytes('foobar'));
} catch (\RuntimeException $e) {
    echo $e, PHP_EOL;
}

echo PHP_EOL, "=======", PHP_EOL, PHP_EOL;

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
    echo $e, PHP_EOL;
}

echo PHP_EOL, "-------", PHP_EOL, PHP_EOL;

try {
    var_dump(bin2hex($randomizer->getBytes(1)));
} catch (\RuntimeException $e) {
    echo $e, PHP_EOL;
}

echo PHP_EOL, "-------", PHP_EOL, PHP_EOL;

try {
    var_dump($randomizer->shuffleArray(\range(1, 10)));
} catch (\RuntimeException $e) {
    echo $e, PHP_EOL;
}

echo PHP_EOL, "-------", PHP_EOL, PHP_EOL;

try {
    var_dump($randomizer->shuffleBytes('foobar'));
} catch (\RuntimeException $e) {
    echo $e, PHP_EOL;
}

?>
--EXPECTF--
DomainException: A random engine must return a non-empty string in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->getInt(%d, %d)
#1 {main}

Next RuntimeException: Random number generation failed in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->getInt(%d, %d)
#1 {main}

-------

DomainException: A random engine must return a non-empty string in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->getBytes(1)
#1 {main}

Next RuntimeException: Random number generation failed in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->getBytes(1)
#1 {main}

-------

DomainException: A random engine must return a non-empty string in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->shuffleArray(Array)
#1 {main}

Next RuntimeException: Random number generation failed in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->shuffleArray(Array)
#1 {main}

-------

DomainException: A random engine must return a non-empty string in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->shuffleBytes('foobar')
#1 {main}

Next RuntimeException: Random number generation failed in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->shuffleBytes('foobar')
#1 {main}

=======

int(%d)

-------

string(2) "ff"

-------

RuntimeException: Random number generation failed in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->shuffleArray(Array)
#1 {main}

-------

RuntimeException: Random number generation failed in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->shuffleBytes('foobar')
#1 {main}
