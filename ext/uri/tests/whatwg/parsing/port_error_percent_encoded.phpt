--TEST--
Test Uri\WhatWg\Url parsing - port - percent encoded character
--FILE--
<?php

try {
    new Uri\WhatWg\Url("https://example.com:%30");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed (PortInvalid)
