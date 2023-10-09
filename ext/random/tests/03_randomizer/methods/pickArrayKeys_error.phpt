--TEST--
Random: Randomizer: pickArrayKeys(): Parameters are correctly validated
--FILE--
<?php

use Random\Randomizer;

function randomizer(): Randomizer
{
    return new Randomizer();
}

try {
    var_dump(randomizer()->pickArrayKeys("foo", 2));
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    var_dump(randomizer()->pickArrayKeys([], 0));
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    var_dump(randomizer()->pickArrayKeys(range(1, 3), 0));
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    var_dump(randomizer()->pickArrayKeys(range(1, 3), -1));
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    var_dump(randomizer()->pickArrayKeys(range(1, 3), 10));
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
Random\Randomizer::pickArrayKeys(): Argument #1 ($array) must be of type array, string given
Random\Randomizer::pickArrayKeys(): Argument #1 ($array) cannot be empty
Random\Randomizer::pickArrayKeys(): Argument #2 ($num) must be between 1 and the number of elements in argument #1 ($array)
Random\Randomizer::pickArrayKeys(): Argument #2 ($num) must be between 1 and the number of elements in argument #1 ($array)
Random\Randomizer::pickArrayKeys(): Argument #2 ($num) must be between 1 and the number of elements in argument #1 ($array)
