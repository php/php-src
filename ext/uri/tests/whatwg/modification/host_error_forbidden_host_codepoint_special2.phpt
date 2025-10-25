--TEST--
Test Uri\WhatWg\Url component modification - host - forbidden domain code point
--EXTENSIONS--
uri
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://example.com");

try {
    $url = $url->withHost("ex:mple.com");
} catch (Uri\WhatWg\InvalidUrlException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified host is malformed
