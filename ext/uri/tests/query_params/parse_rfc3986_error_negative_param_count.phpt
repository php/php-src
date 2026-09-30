--TEST--
Test Uri\QueryParams::parseRfc3986() - error - negative param count
--FILE--
<?php

try {
    Uri\QueryParams::parseRfc3986("", maxParamCount: -1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: Uri\QueryParams::parseRfc3986(): Argument #2 ($maxParamCount) must be greater than or equal to 0
