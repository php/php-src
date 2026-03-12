--TEST--
Test Uri\WhatWg\Url parsing - port - negative value
--FILE--
<?php

try {
    new Uri\WhatWg\Url("http://example.com:-1");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (PortInvalid)
