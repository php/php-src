--TEST--
Test Uri\QueryParams::parseRfc3986() - error - param count exceeds the limit
--FILE--
<?php

try {
    Uri\QueryParams::parseRfc3986("x=1&x=2", maxParamCount: 1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidQueryParamsException: The specified query string contains too many parameters
