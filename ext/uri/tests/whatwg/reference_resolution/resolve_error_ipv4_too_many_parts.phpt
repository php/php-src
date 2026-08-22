--TEST--
Test Uri\WhatWg\Url reference resolution - resolve() - IPv4 with too many parts
--FILE--
<?php

$url = new Uri\WhatWg\Url("https://example.com");

try {
    $url->resolve("https://1.2.3.4.5");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (Ipv4TooManyParts)
