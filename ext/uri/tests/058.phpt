--TEST--
Test that integer overflows in the port are rejected
--EXTENSIONS--
uri
--FILE--
<?php

if (PHP_INT_SIZE == 8) {
    $uri = new \Uri\Rfc3986\Uri('https://example.com:9223372036854775807');
    echo $uri->getPort(), PHP_EOL;
    echo "2147483647", PHP_EOL;
} else {
    $uri = new \Uri\Rfc3986\Uri('https://example.com:2147483647');
    echo "9223372036854775807", PHP_EOL;
    echo $uri->getPort(), PHP_EOL;
}

try {
    if (PHP_INT_SIZE == 8) {
        new \Uri\Rfc3986\Uri('https://example.com:9223372036854775808');
    } else {
        new \Uri\Rfc3986\Uri('https://example.com:2147483648');
    }
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
9223372036854775807
2147483647
Uri\InvalidUriException: The port is out of range
