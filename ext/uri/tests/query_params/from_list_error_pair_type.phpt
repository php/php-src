--TEST--
Test Uri\QueryParams::fromList() - error - list element is not an array
--FILE--
<?php

try {
    Uri\QueryParams::fromList([["ok", "value"], "flag"]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Uri\QueryParams::fromList(): Argument #1 ($queryParams) must contain an array at index 1, string given
