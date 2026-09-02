--TEST--
Test Uri\WhatWg\Url::withHost() - error - forbidden domain code point
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://example.com");

try {
    $url = $url->withHost("ex@mple.com");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified host is malformed (DomainInvalidCodePoint)
