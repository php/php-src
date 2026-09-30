--TEST--
Test Uri\QueryParams::parseRfc3986() - error - an unencoded space
--FILE--
<?php

try {
    Uri\QueryParams::parseRfc3986("a=foo bar");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidQueryParamsException: The specified query string is malformed
