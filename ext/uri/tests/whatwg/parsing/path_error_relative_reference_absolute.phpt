--TEST--
Test Uri\WhatWg\Url parsing - path - relative reference
--FILE--
<?php

try {
    new Uri\WhatWg\Url("/foo");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (MissingSchemeNonRelativeUrl)
