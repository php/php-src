--TEST--
Test Uri\WhatWg\Url component modification - host - forbidden host codepoint
--EXTENSIONS--
uri
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("foo://example.com");

try {
    $url = $url->withHost("ex:mple.com");
} catch (Uri\WhatWg\InvalidUrlException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified host is malformed
