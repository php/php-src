--TEST--
Test Uri\WhatWg\Url parsing - scheme - only scheme
--FILE--
<?php

try {
    new Uri\WhatWg\Url("http://");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (HostMissing)
