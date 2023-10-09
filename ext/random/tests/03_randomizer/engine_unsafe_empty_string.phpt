--TEST--
Random: Randomizer: Engines returning an empty string are detected and rejected
--FILE--
<?php

use Random\Engine;
use Random\Randomizer;

final class EmptyStringEngine implements Engine
{
    public function generate(): string
    {
        return '';
    }
}

function randomizer(): Randomizer
{
    return new Randomizer(new EmptyStringEngine());
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

try {
    var_dump(randomizer()->getBytesFromString('123', 10));
} catch (Random\BrokenRandomEngineError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    var_dump(randomizer()->getBytesFromString(str_repeat('a', 500), 10));
} catch (Random\BrokenRandomEngineError $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
A random engine must return a non-empty string
A random engine must return a non-empty string
A random engine must return a non-empty string
A random engine must return a non-empty string
A random engine must return a non-empty string
A random engine must return a non-empty string
A random engine must return a non-empty string
