--TEST--
GH-20668 lobal buffer overflow in url.c
--FILE--
<?php

$str = 'file:///datafoo:test';
$url = Uri\WhatWg\Url::parse($str);
$host = $url->getAsciiHost();

try {
    $url->withHost($host);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified host is malformed (DomainInvalidCodePoint)
