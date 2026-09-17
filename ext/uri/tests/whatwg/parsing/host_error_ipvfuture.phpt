--TEST--
Test Uri\WhatWg\Url parsing - host - IPvFuture
--FILE--
<?php

try {
    new Uri\WhatWg\Url("https://[v7.host]");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (Ipv6InvalidCodePoint)
