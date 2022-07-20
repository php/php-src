--TEST--
Random: Randomizer: User: Engine unsafe
--FILE--
<?php

use Random\Randomizer;

final class EmptyStringEngine implements \Random\Engine {
    public function generate(): string
    {
        return '';
    }
}

try {
    var_dump((new Randomizer(new EmptyStringEngine()))->getInt(0, 123));
} catch (\RuntimeException $e) {
    echo $e, PHP_EOL;
}

echo PHP_EOL, "-------", PHP_EOL, PHP_EOL;

try {
    var_dump(bin2hex((new Randomizer(new EmptyStringEngine()))->getBytes(1)));
} catch (\RuntimeException $e) {
    echo $e, PHP_EOL;
}

echo PHP_EOL, "-------", PHP_EOL, PHP_EOL;

try {
    var_dump((new Randomizer(new EmptyStringEngine()))->shuffleArray(\range(1, 10)));
} catch (\RuntimeException $e) {
    echo $e, PHP_EOL;
}

echo PHP_EOL, "-------", PHP_EOL, PHP_EOL;

try {
    var_dump((new Randomizer(new EmptyStringEngine()))->shuffleBytes('foobar'));
} catch (\RuntimeException $e) {
    echo $e, PHP_EOL;
}

echo PHP_EOL, "=======", PHP_EOL, PHP_EOL;

// Infinite loop

final class HeavilyBiasedEngine implements \Random\Engine {
    public function generate(): string
    {
        return "\xff\xff\xff\xff\xff\xff\xff\xff";
    }
}

try {
    var_dump((new Randomizer(new HeavilyBiasedEngine()))->getInt(0, 123));
} catch (\RuntimeException $e) {
    echo $e, PHP_EOL;
}

echo PHP_EOL, "-------", PHP_EOL, PHP_EOL;

try {
    var_dump(bin2hex((new Randomizer(new HeavilyBiasedEngine()))->getBytes(1)));
} catch (\RuntimeException $e) {
    echo $e, PHP_EOL;
}

echo PHP_EOL, "-------", PHP_EOL, PHP_EOL;

try {
    var_dump((new Randomizer(new HeavilyBiasedEngine()))->shuffleArray(\range(1, 10)));
} catch (\RuntimeException $e) {
    echo $e, PHP_EOL;
}

echo PHP_EOL, "-------", PHP_EOL, PHP_EOL;

try {
    var_dump((new Randomizer(new HeavilyBiasedEngine()))->shuffleBytes('foobar'));
} catch (\RuntimeException $e) {
    echo $e, PHP_EOL;
}

?>
--EXPECTF--
DomainException: A random engine must return a non-empty string in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->getInt(0, 123)
#1 {main}

Next RuntimeException: Random number generation failed in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->getInt(0, 123)
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

RuntimeException: Failed to generate an acceptable random number in 50 attempts in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->getInt(0, 123)
#1 {main}

Next RuntimeException: Random number generation failed in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->getInt(0, 123)
#1 {main}

-------

string(2) "ff"

-------

RuntimeException: Failed to generate an acceptable random number in 50 attempts in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->shuffleArray(Array)
#1 {main}

Next RuntimeException: Random number generation failed in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->shuffleArray(Array)
#1 {main}

-------

RuntimeException: Failed to generate an acceptable random number in 50 attempts in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->shuffleBytes('foobar')
#1 {main}

Next RuntimeException: Random number generation failed in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->shuffleBytes('foobar')
#1 {main}
