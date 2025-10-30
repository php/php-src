--TEST--
Test Uri\WhatWg\Url parsing - scheme - percent encoded character
--FILE--
<?php

try {
    new Uri\WhatWg\Url("http%2F://example.com");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed (MissingSchemeNonRelativeUrl)
