--TEST--
Test Uri\WhatWg\Url component modification - path - URL encoded characters
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withPath("/foo%2Fb%61r"); // /foo/bar

var_dump($url1->getPath());
var_dump($url2->getPath());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(1) "/"
string(12) "/foo%2Fb%61r"
string(31) "https://example.com/foo%2Fb%61r"
