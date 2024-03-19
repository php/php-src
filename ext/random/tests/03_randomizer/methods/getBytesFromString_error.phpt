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
    echo $e->getMessage(), PHP_EOL;
}

try {
    var_dump(randomizer()->getBytesFromString("abc", -1));
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Random\Randomizer::getBytesFromString(): Argument #1 ($string) cannot be empty
Random\Randomizer::getBytesFromString(): Argument #2 ($length) must be greater than or equal to 0
