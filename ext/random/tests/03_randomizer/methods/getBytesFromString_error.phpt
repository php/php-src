--TEST--
Random: Randomizer: getBytesFromString(): Parameters are correctly validated
--FILE--
<?php

use Random\Randomizer;

function randomizer(): Randomizer
{
    return new Randomizer();
}

try {
    var_dump(randomizer()->getBytesFromString("", 2));
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump(randomizer()->getBytesFromString("abc", 0));
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
ValueError: Random\Randomizer::getBytesFromString(): Argument #1 ($string) must not be empty
ValueError: Random\Randomizer::getBytesFromString(): Argument #2 ($length) must be greater than 0
