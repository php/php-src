--TEST--
Test Uri\WhatWg\Url component modification - host - unsetting existing
--EXTENSIONS--
uri
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://example.com");

try {
    $url = $url->withHost(null);
} catch (Uri\WhatWg\InvalidUrlException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified host is malformed (HostMissing)
