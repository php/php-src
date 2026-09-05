--TEST--
Test Uri\Rfc3986\Uri parsing - port - excessive number of digits
--FILE--
<?php

try {
    new \Uri\Rfc3986\Uri('https://example.com:4242424269424242426942424242694242424269');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidUriException: The port is out of range
