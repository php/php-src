--TEST--
Test Uri\QueryParams::parseRfc3986() - error - an unencoded NUL byte
--FILE--
<?php

try {
    Uri\QueryParams::parseRfc3986("a=x\0y");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidQueryParamsException: The specified query string is malformed
