--TEST--
Test Uri\WhatWg\Url parsing - basic - empty string
--FILE--
<?php

try {
    new Uri\WhatWg\Url("");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed (MissingSchemeNonRelativeUrl)
