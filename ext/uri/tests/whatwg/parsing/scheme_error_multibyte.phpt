--TEST--
Test Uri\WhatWg\Url parsing - scheme - multibyte codepoint
--FILE--
<?php

try {
    new Uri\WhatWg\Url("ƕŢŢƤƨ://example.com");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed (MissingSchemeNonRelativeUrl)
