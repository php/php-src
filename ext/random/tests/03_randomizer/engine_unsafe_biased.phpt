--TEST--
Random: Randomizer: Heavily biased engines are detected and rejected
--FILE--
<?php

use Random\Engine;
use Random\Randomizer;

final class HeavilyBiasedEngine implements Engine
{
    public function generate(): string
    {
        return str_repeat("\xff", PHP_INT_SIZE);
    }
}

function randomizer(): Randomizer
{
    return new Randomizer(new HeavilyBiasedEngine());
}

try {
    var_dump(randomizer()->getInt(0, 1234));
} catch (Random\BrokenRandomEngineError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump(randomizer()->nextInt());
} catch (Random\BrokenRandomEngineError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump(bin2hex(randomizer()->getBytes(1)));
} catch (Random\BrokenRandomEngineError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump(randomizer()->shuffleArray(range(1, 1234)));
} catch (Random\BrokenRandomEngineError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump(randomizer()->pickArrayKeys(range(1, 1234), 1));
} catch (Random\BrokenRandomEngineError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump(randomizer()->pickArrayKeys(range(1, 1234), 10));
} catch (Random\BrokenRandomEngineError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump(randomizer()->shuffleBytes('foobar'));
} catch (Random\BrokenRandomEngineError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump(randomizer()->getBytesFromString('123', 10));
} catch (Random\BrokenRandomEngineError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump(randomizer()->getBytesFromString(str_repeat('a', 500), 10));
} catch (Random\BrokenRandomEngineError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
Random\BrokenRandomEngineError: Failed to generate an acceptable random number in 50 attempts
int(%d)
string(2) "ff"
Random\BrokenRandomEngineError: Failed to generate an acceptable random number in 50 attempts
Random\BrokenRandomEngineError: Failed to generate an acceptable random number in 50 attempts
Random\BrokenRandomEngineError: Failed to generate an acceptable random number in 50 attempts
Random\BrokenRandomEngineError: Failed to generate an acceptable random number in 50 attempts
Random\BrokenRandomEngineError: Failed to generate an acceptable random number in 50 attempts
Random\BrokenRandomEngineError: Failed to generate an acceptable random number in 50 attempts
