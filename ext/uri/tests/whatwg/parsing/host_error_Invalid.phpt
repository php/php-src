--TEST--
Test Uri\WhatWg\Url parsing - host - invalid code point
--FILE--
<?php

try {
    new Uri\WhatWg\Url("https://ex[a]mple.com");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (DomainInvalidCodePoint)
