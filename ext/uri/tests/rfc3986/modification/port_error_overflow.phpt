--TEST--
Test Uri\Rfc3986\Uri component modification - port - too large port numbers don't overflow
--EXTENSIONS--
uri
--FILE--
<?php

$uri = new \Uri\Rfc3986\Uri('https://example.com');

try {
    if (PHP_INT_SIZE == 8) {
        $uri->withPort(9223372036854775808);
    } else {
        $uri->withPort(2147483648);
    }
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Uri\Rfc3986\Uri::withPort(): Argument #1 ($port) must be of type ?int, float given
