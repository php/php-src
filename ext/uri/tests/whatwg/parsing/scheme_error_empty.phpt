--TEST--
Test Uri\WhatWg\Url parsing - scheme - empty
--FILE--
<?php

try {
    new Uri\WhatWg\Url("://example.com");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed (MissingSchemeNonRelativeUrl)
