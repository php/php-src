--TEST--
Test Uri\WhatWg\Url parsing - scheme - invalid character
--FILE--
<?php

try {
    new Uri\WhatWg\Url("http&://example.com");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (MissingSchemeNonRelativeUrl)
