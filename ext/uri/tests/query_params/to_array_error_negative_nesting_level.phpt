--TEST--
Test Uri\QueryParams::toArray() - error - negative nesting level
--FILE--
<?php

$params = new Uri\QueryParams();

try {
    $params->toArray(-1);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: Uri\QueryParams::toArray(): Argument #1 ($maxNestingLevel) must be greater than or equal to 0
