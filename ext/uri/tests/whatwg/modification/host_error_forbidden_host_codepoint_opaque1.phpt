--TEST--
Test Uri\WhatWg\Url component modification - host - error - forbidden host code point
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("foo://example.com");

try {
    $url = $url->withHost("ex@mple.com");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified host is malformed (HostInvalidCodePoint)
