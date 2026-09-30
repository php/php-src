--TEST--
Test Uri\QueryParams::fromList() - error - non-string parameter name
--FILE--
<?php

try {
    Uri\QueryParams::fromList([[42, "value"]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Uri\QueryParams::fromList(): Argument #1 ($queryParams) must contain a string parameter name at index 0, int given
