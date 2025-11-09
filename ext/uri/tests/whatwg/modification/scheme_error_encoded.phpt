--TEST--
Test Uri\WhatWg\Url component modification - scheme - URL encoded characters
--EXTENSIONS--
uri
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://example.com");

try {
    $url->withScheme("http%73");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified scheme is malformed
