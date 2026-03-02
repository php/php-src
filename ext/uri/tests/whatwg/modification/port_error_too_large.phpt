--TEST--
Test Uri\WhatWg\Url component modification - host - larger than a 16-bit unsigned integer
--EXTENSIONS--
uri
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://example.com");

try {
    $url->withPort(65536);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified port is malformed (PortOutOfRange)
