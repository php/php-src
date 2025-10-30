--TEST--
Test Uri\WhatWg\Url parsing - path - relative reference
--FILE--
<?php

try {
    new Uri\WhatWg\Url("foo");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
The specified URI is malformed (MissingSchemeNonRelativeUrl)
