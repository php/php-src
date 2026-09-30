--TEST--
Test Uri\QueryParams::fromList() - error - outer array is not a list
--FILE--
<?php

try {
    Uri\QueryParams::fromList([1 => ["tag", "value"]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: Uri\QueryParams::fromList(): Argument #1 ($queryParams) must be a list of [name, value] pairs
