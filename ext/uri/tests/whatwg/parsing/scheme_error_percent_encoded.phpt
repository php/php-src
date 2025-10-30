--TEST--
Test Uri\WhatWg\Url parsing - scheme - percent encoded character
--FILE--
<?php

try {
    new Uri\WhatWg\Url("http%2F://example.com");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (MissingSchemeNonRelativeUrl)
