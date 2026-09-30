--TEST--
Test Uri\QueryParams::parseRfc3986() - error - an invalid hexadecimal digit in a parameter value
--FILE--
<?php

try {
    Uri\QueryParams::parseRfc3986("a=%0G");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidQueryParamsException: The specified query string is malformed
