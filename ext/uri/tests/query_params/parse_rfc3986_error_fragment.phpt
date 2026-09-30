--TEST--
Test Uri\QueryParams::parseRfc3986() - error - an unencoded fragment delimiter
--FILE--
<?php

try {
    Uri\QueryParams::parseRfc3986("ok=1&a=#value");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidQueryParamsException: The specified query string is malformed
