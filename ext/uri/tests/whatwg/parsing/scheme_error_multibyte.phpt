--TEST--
Test Uri\WhatWg\Url parsing - scheme - multibyte code point
--FILE--
<?php

try {
    new Uri\WhatWg\Url("ƕŢŢƤƨ://example.com");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (MissingSchemeNonRelativeUrl)
