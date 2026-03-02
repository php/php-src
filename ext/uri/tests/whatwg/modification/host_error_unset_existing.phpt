--TEST--
Test Uri\WhatWg\Url component modification - host - unsetting existing
--EXTENSIONS--
uri
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://example.com");

try {
    $url = $url->withHost(null);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified host is malformed (HostMissing)
