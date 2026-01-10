--TEST--
Test Uri\WhatWg\Url parsing - port - multibyte
--FILE--
<?php

try {
    new Uri\WhatWg\Url("https://example.com:Ȏ");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (PortInvalid)
