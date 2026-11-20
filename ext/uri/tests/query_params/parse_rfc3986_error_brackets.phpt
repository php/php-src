--TEST--
Test Uri\QueryParams::parseRfc3986() - error - unencoded brackets
--FILE--
<?php

try {
    Uri\QueryParams::parseRfc3986("a[]=1");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidQueryParamsException: The specified query string is malformed
