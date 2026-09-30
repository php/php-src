--TEST--
Test Uri\QueryParams::parseRfc3986() - error - unencoded valid UTF-8
--FILE--
<?php

try {
    Uri\QueryParams::parseRfc3986("a=\xC3\xA9");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidQueryParamsException: The specified query string is malformed
