--TEST--
Test Uri\QueryParams::fromArray() - error - nested non-finite float reference
--FILE--
<?php

$value = INF;

try {
    Uri\QueryParams::fromArray(["ok" => 1, "nested" => [&$value]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: Uri\QueryParams::fromArray(): Argument #1 ($queryParams) contains a non-finite float for query parameter "nested[]"
