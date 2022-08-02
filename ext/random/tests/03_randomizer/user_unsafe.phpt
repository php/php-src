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
        return \str_repeat("\xff", PHP_INT_SIZE);
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
    } catch (Throwable $e) {
        echo $e, PHP_EOL;
    }

    echo PHP_EOL, "-------", PHP_EOL, PHP_EOL;

    try {
        var_dump((new Randomizer(new $engine()))->nextInt());
    } catch (Throwable $e) {
        echo $e, PHP_EOL;
    }

    echo PHP_EOL, "-------", PHP_EOL, PHP_EOL;

    try {
        var_dump(bin2hex((new Randomizer(new $engine()))->getBytes(1)));
    } catch (Throwable $e) {
        echo $e, PHP_EOL;
    }

    echo PHP_EOL, "-------", PHP_EOL, PHP_EOL;

    try {
        var_dump((new Randomizer(new $engine()))->shuffleArray(\range(1, 10)));
    } catch (Throwable $e) {
        echo $e, PHP_EOL;
    }

    echo PHP_EOL, "-------", PHP_EOL, PHP_EOL;

    try {
        var_dump((new Randomizer(new $engine()))->shuffleBytes('foobar'));
    } catch (Throwable $e) {
        echo $e, PHP_EOL;
    }

    echo PHP_EOL, "=====================", PHP_EOL;
}

?>
--EXPECTF--
=====================
EmptyStringEngine
=====================

Error: A random engine must return a non-empty string in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->getInt(0, 123)
#1 {main}

-------

Error: A random engine must return a non-empty string in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->nextInt()
#1 {main}

-------

Error: A random engine must return a non-empty string in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->getBytes(1)
#1 {main}

-------

Error: A random engine must return a non-empty string in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->shuffleArray(Array)
#1 {main}

-------

Error: A random engine must return a non-empty string in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->shuffleBytes('foobar')
#1 {main}

=====================
HeavilyBiasedEngine
=====================

Error: Failed to generate an acceptable random number in 50 attempts in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->getInt(0, 123)
#1 {main}

-------

int(%d)

-------

string(2) "ff"

-------

Error: Failed to generate an acceptable random number in 50 attempts in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->shuffleArray(Array)
#1 {main}

-------

Error: Failed to generate an acceptable random number in 50 attempts in %s:%d
Stack trace:
#0 %s(%d): Random\Randomizer->shuffleBytes('foobar')
#1 {main}

=====================
