--TEST--
Test Uri\WhatWg\Url parsing - port - percent encoded character
--FILE--
<?php

try {
    new Uri\WhatWg\Url("https://example.com:%30");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (PortInvalid)
