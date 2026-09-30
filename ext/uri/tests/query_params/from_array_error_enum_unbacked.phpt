--TEST--
Test Uri\QueryParams::fromArray() - error - nested unbacked enum
--FILE--
<?php

enum TestEnum { case Value; }
try {
    Uri\QueryParams::fromArray(["nested" => [TestEnum::Value]]);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: Uri\QueryParams::fromArray(): Argument #1 ($queryParams) contains an invalid value for query parameter "nested[]": expected BackedEnum|string|int|float|null, TestEnum given
