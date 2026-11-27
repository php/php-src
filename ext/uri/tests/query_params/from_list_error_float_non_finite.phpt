--TEST--
Test Uri\QueryParams::fromList() - error - non-finite float
--FILE--
<?php

try {
    Uri\QueryParams::fromList([["ok", 1], ["x", NAN]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: Uri\QueryParams::fromList(): Argument #1 ($queryParams) contains a non-finite float for query parameter "x"
