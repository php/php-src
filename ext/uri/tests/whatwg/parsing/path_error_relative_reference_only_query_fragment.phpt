--TEST--
Test Uri\WhatWg\Url parsing - path - relative reference
--FILE--
<?php

try {
    new Uri\WhatWg\Url("?query#fragment");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed (MissingSchemeNonRelativeUrl)
