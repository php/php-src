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

final class HeavilyBiasedEngine implements \Random\Engine {
    public function generate(): string
    {
        return "\xff\xff\xff\xff\xff\xff\xff\xff";
    }
}

echo "=====================", PHP_EOL;

foreach ([
    EmptyStringEngine::class,
    HeavilyBiasedEngine::class,
] as $engine) {
    echo $engine, PHP_EOL, "=====================", PHP_EOL, PHP_EOL;

    try {
        var_dump((new Randomizer(new $engine()))->getInt(0, 123));
    } catch (\RuntimeException $e) {
        echo $e, PHP_EOL;
    }
    
    echo PHP_EOL, "-------", PHP_EOL, PHP_EOL;
    
    try {
        var_dump(bin2hex((new Randomizer(new $engine()))->getBytes(1)));
    } catch (\RuntimeException $e) {
        echo $e, PHP_EOL;
    }
    
    echo PHP_EOL, "-------", PHP_EOL, PHP_EOL;
    
    try {
        var_dump((new Randomizer(new $engine()))->shuffleArray(\range(1, 10)));
    } catch (\RuntimeException $e) {
        echo $e, PHP_EOL;
    }
    
    echo PHP_EOL, "-------", PHP_EOL, PHP_EOL;
    
    try {
        var_dump((new Randomizer(new $engine()))->shuffleBytes('foobar'));
    } catch (\RuntimeException $e) {
        echo $e, PHP_EOL;
    }

    echo PHP_EOL, "=====================", PHP_EOL;
}

?>
--EXPECTF--
=====================
EmptyStringEngine
=====================

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

=====================
HeavilyBiasedEngine
=====================

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

=====================
