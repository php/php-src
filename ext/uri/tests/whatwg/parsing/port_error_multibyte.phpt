--TEST--
Test Uri\WhatWg\Url parsing - port - multibyte
--FILE--
<?php

try {
    new Uri\WhatWg\Url("https://example.com:Ȏ");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed (PortInvalid)
