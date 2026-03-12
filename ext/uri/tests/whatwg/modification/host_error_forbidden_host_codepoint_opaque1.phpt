--TEST--
Test Uri\WhatWg\Url component modification - host - forbidden host code point
--EXTENSIONS--
uri
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("foo://example.com");

try {
    $url = $url->withHost("ex@mple.com");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified host is malformed (HostInvalidCodePoint)
