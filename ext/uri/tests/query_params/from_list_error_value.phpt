--TEST--
Test Uri\QueryParams::fromList() - error - invalid parameter value
--FILE--
<?php

try {
    Uri\QueryParams::fromList([["ok", "value"], ["flag", false]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Uri\QueryParams::fromList(): Argument #1 ($queryParams) contains an invalid value for query parameter "flag": expected BackedEnum|string|int|float|null, false given
