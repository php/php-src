--TEST--
Test that out of range ports are rejected
--EXTENSIONS--
uri
--FILE--
<?php

try {
    new \Uri\Rfc3986\Uri('https://example.com:987654321987654321987654321987654321');
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
} 

?>
--EXPECT--
Uri\InvalidUriException: The port is out of range
