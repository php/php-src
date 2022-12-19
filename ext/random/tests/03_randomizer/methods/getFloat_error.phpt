--TEST--
Random: Randomizer: getFloat(): Parameters are correctly validated
--FILE--
<?php

use Random\IntervalBoundary;
use Random\Randomizer;

function randomizer(): Randomizer
{
    return new Randomizer();
}

foreach ([
    IntervalBoundary::ClosedClosed,
    IntervalBoundary::ClosedOpen,
    IntervalBoundary::OpenClosed,
    IntervalBoundary::OpenOpen,
] as $boundary) {
    echo $boundary->name, PHP_EOL;

    try {
        var_dump(randomizer()->getFloat(NAN, 0.0, $boundary));
    } catch (ValueError $e) {
        echo $e->getMessage(), PHP_EOL;
    }

    try {
        var_dump(randomizer()->getFloat(INF, 0.0, $boundary));
    } catch (ValueError $e) {
        echo $e->getMessage(), PHP_EOL;
    }

    try {
        var_dump(randomizer()->getFloat(-INF, 0.0, $boundary));
    } catch (ValueError $e) {
        echo $e->getMessage(), PHP_EOL;
    }

    try {
        var_dump(randomizer()->getFloat(0.0, NAN, $boundary));
    } catch (ValueError $e) {
        echo $e->getMessage(), PHP_EOL;
    }

    try {
        var_dump(randomizer()->getFloat(0.0, INF, $boundary));
    } catch (ValueError $e) {
        echo $e->getMessage(), PHP_EOL;
    }

    try {
        var_dump(randomizer()->getFloat(0.0, -INF, $boundary));
    } catch (ValueError $e) {
        echo $e->getMessage(), PHP_EOL;
    }

    try {
        var_dump(randomizer()->getFloat(0.0, -0.1, $boundary));
    } catch (ValueError $e) {
        echo $e->getMessage(), PHP_EOL;
    }

    try {
        var_dump(randomizer()->getFloat(0.0, 0.0, $boundary));
    } catch (ValueError $e) {
        echo $e->getMessage(), PHP_EOL;
    }

    try {
        // Both values round to the same float.
        var_dump(randomizer()->getFloat(100_000_000_000_000_000.0, 100_000_000_000_000_000.1, $boundary));
    } catch (ValueError $e) {
        echo $e->getMessage(), PHP_EOL;
    }
}

?>
--EXPECT--
ClosedClosed
Random\Randomizer::getFloat(): Argument #1 ($min) must be finite
Random\Randomizer::getFloat(): Argument #1 ($min) must be finite
Random\Randomizer::getFloat(): Argument #1 ($min) must be finite
Random\Randomizer::getFloat(): Argument #2 ($max) must be finite
Random\Randomizer::getFloat(): Argument #2 ($max) must be finite
Random\Randomizer::getFloat(): Argument #2 ($max) must be finite
Random\Randomizer::getFloat(): Argument #2 ($max) must be greater than or equal to argument #1 ($min)
float(0)
float(1.0E+17)
ClosedOpen
Random\Randomizer::getFloat(): Argument #1 ($min) must be finite
Random\Randomizer::getFloat(): Argument #1 ($min) must be finite
Random\Randomizer::getFloat(): Argument #1 ($min) must be finite
Random\Randomizer::getFloat(): Argument #2 ($max) must be finite
Random\Randomizer::getFloat(): Argument #2 ($max) must be finite
Random\Randomizer::getFloat(): Argument #2 ($max) must be finite
Random\Randomizer::getFloat(): Argument #2 ($max) must be greater than argument #1 ($min)
Random\Randomizer::getFloat(): Argument #2 ($max) must be greater than argument #1 ($min)
Random\Randomizer::getFloat(): Argument #2 ($max) must be greater than argument #1 ($min)
OpenClosed
Random\Randomizer::getFloat(): Argument #1 ($min) must be finite
Random\Randomizer::getFloat(): Argument #1 ($min) must be finite
Random\Randomizer::getFloat(): Argument #1 ($min) must be finite
Random\Randomizer::getFloat(): Argument #2 ($max) must be finite
Random\Randomizer::getFloat(): Argument #2 ($max) must be finite
Random\Randomizer::getFloat(): Argument #2 ($max) must be finite
Random\Randomizer::getFloat(): Argument #2 ($max) must be greater than argument #1 ($min)
Random\Randomizer::getFloat(): Argument #2 ($max) must be greater than argument #1 ($min)
Random\Randomizer::getFloat(): Argument #2 ($max) must be greater than argument #1 ($min)
OpenOpen
Random\Randomizer::getFloat(): Argument #1 ($min) must be finite
Random\Randomizer::getFloat(): Argument #1 ($min) must be finite
Random\Randomizer::getFloat(): Argument #1 ($min) must be finite
Random\Randomizer::getFloat(): Argument #2 ($max) must be finite
Random\Randomizer::getFloat(): Argument #2 ($max) must be finite
Random\Randomizer::getFloat(): Argument #2 ($max) must be finite
Random\Randomizer::getFloat(): Argument #2 ($max) must be greater than argument #1 ($min)
Random\Randomizer::getFloat(): Argument #2 ($max) must be greater than argument #1 ($min)
Random\Randomizer::getFloat(): Argument #2 ($max) must be greater than argument #1 ($min)
