--TEST--
Test Uri\WhatWg\Url parsing - host - invalid code point
--FILE--
<?php

try {
    new Uri\WhatWg\Url("https://ex[a]mple.com");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed (DomainInvalidCodePoint)
