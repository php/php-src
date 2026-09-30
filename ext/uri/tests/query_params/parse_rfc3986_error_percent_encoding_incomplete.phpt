--TEST--
Test Uri\QueryParams::parseRfc3986() - error - an incomplete percent escape
--FILE--
<?php

try {
    Uri\QueryParams::parseRfc3986("a=%");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidQueryParamsException: The specified query string is malformed
