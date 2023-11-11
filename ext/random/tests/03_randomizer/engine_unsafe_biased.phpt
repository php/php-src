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
    echo $e->getMessage(), PHP_EOL;
}

try {
    var_dump(randomizer()->nextInt());
} catch (Random\BrokenRandomEngineError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    var_dump(bin2hex(randomizer()->getBytes(1)));
} catch (Random\BrokenRandomEngineError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    var_dump(randomizer()->shuffleArray(range(1, 1234)));
} catch (Random\BrokenRandomEngineError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    var_dump(randomizer()->shuffleBytes('foobar'));
} catch (Random\BrokenRandomEngineError $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
Failed to generate an acceptable random number in 50 attempts
int(%d)
string(2) "ff"
Failed to generate an acceptable random number in 50 attempts
Failed to generate an acceptable random number in 50 attempts
