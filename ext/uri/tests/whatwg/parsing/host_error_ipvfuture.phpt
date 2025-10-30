--TEST--
Test Uri\WhatWg\Url parsing - host - IPvFuture
--FILE--
<?php

try {
    new Uri\WhatWg\Url("https://[v7.host]");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed (Ipv6InvalidCodePoint)
