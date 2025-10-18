--TEST--
Test Uri\WhatWg\Url component modification - port - negative number
--EXTENSIONS--
uri
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://example.com");

try {
    $url->withPort(-1);
} catch (Uri\WhatWg\InvalidUrlException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified port is malformed
