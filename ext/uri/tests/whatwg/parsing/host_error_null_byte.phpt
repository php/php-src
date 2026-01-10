--TEST--
Test Uri\WhatWg\Url parsing - basic - URL contains null byte
--FILE--
<?php

try {
    new Uri\WhatWg\Url("https://exam\0ple.com");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (DomainInvalidCodePoint)
