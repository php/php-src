--TEST--
Test Uri\QueryParams::parseRfc3986() - error - a percent escape with only one hexadecimal digit
--FILE--
<?php

try {
    Uri\QueryParams::parseRfc3986("a=%2");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidQueryParamsException: The specified query string is malformed
