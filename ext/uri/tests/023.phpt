--TEST--
Test property mutation - scheme
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withScheme("http");

var_dump($url1->getScheme());
var_dump($url2->getScheme());

try {
    $url2->withScheme("");
} catch (Uri\WhatWg\InvalidUrlException $e) {
    echo $e->getMessage() . "\n";
}

try {
    $url2->withScheme("http%73");
} catch (Uri\WhatWg\InvalidUrlException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
string(5) "https"
string(4) "http"
URL parsing failed
URL parsing failed
