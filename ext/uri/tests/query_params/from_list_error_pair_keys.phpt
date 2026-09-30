--TEST--
Test Uri\QueryParams::fromList() - error - associative pair
--FILE--
<?php

try {
    Uri\QueryParams::fromList([["name" => "tag", "value" => "first"]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: Uri\QueryParams::fromList(): Argument #1 ($queryParams) must contain a [name, value] pair at index 0
