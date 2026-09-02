--TEST--
Test Uri\WhatWg\Url::withHost() - error - unsetting existing
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://example.com");

try {
    $url = $url->withHost(null);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified host is malformed (HostMissing)
