--TEST--
Test Uri\QueryParams::fromList() - error - missing value in a pair
--FILE--
<?php

try {
    Uri\QueryParams::fromList([["flag"]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: Uri\QueryParams::fromList(): Argument #1 ($queryParams) must contain a [name, value] pair at index 0
