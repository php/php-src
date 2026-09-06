--TEST--
Test Uri\WhatWg\Url::resolve() - error - null byte in host
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://example.com");

try {
    $url->resolve("https://ex\0mple.com");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (DomainInvalidCodePoint)
