--TEST--
Test Uri\Rfc3986\Uri parsing - port - integer overflow
--FILE--
<?php

try {
    if (PHP_INT_SIZE == 8) {
        new \Uri\Rfc3986\Uri('https://example.com:9223372036854775808');
    } else {
        new \Uri\Rfc3986\Uri('https://example.com:2147483648');
    }
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidUriException: The port is out of range
