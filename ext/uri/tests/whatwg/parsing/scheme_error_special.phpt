--TEST--
Test Uri\WhatWg\Url parsing - scheme - invalid character
--FILE--
<?php

try {
    new Uri\WhatWg\Url("hÁttp://example.com");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed (MissingSchemeNonRelativeUrl)
