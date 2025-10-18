--TEST--
Test Uri\WhatWg\Url component modification - host - larger than a 16-bit unsigned integer
--EXTENSIONS--
uri
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://example.com");

try {
    $url->withPort(65536);
} catch (Uri\WhatWg\InvalidUrlException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified port is malformed (PortOutOfRange)
