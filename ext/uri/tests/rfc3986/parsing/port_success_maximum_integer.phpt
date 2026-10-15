--TEST--
Test Uri\Rfc3986\Uri parsing - port - maximum integer
--FILE--
<?php

if (PHP_INT_SIZE == 8) {
    $uri = new \Uri\Rfc3986\Uri('https://example.com:9223372036854775807');
    echo $uri->getPort(), "\n";
    echo "2147483647", "\n";
} else {
    $uri = new \Uri\Rfc3986\Uri('https://example.com:2147483647');
    echo "9223372036854775807", "\n";
    echo $uri->getPort(), "\n";
}

?>
--EXPECT--
9223372036854775807
2147483647
