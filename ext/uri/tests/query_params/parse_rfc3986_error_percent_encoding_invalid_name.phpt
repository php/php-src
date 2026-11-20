--TEST--
Test Uri\QueryParams::parseRfc3986() - error - an invalid percent escape in a parameter name
--FILE--
<?php

try {
    Uri\QueryParams::parseRfc3986("ok=1&%G0=value");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidQueryParamsException: The specified query string is malformed
