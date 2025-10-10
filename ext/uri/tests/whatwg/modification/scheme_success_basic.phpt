--TEST--
Test Uri\WhatWg\Url component modification - scheme - basic case
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withScheme("HTTP");

var_dump($url1->getScheme());
var_dump($url2->getScheme());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(5) "https"
string(4) "http"
string(19) "http://example.com/"
