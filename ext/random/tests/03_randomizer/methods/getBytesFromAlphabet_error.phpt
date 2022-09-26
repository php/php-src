--TEST--
Random: Randomizer: getBytesFromAlphabet(): Parameters are correctly validated
--FILE--
<?php

use Random\Randomizer;

function randomizer(): Randomizer
{
    return new Randomizer();
}

try {
    var_dump(randomizer()->getBytesFromAlphabet("", 2));
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    var_dump(randomizer()->getBytesFromAlphabet("abc", 0));
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
Random\Randomizer::getBytesFromAlphabet(): Argument #1 ($alphabet) cannot be empty
Random\Randomizer::getBytesFromAlphabet(): Argument #2 ($length) must be greater than 0
