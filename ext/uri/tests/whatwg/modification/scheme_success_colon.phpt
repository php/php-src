--TEST--
Test Uri\WhatWg\Url component modification - scheme - trailing colon
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withScheme("http:");

var_dump($url1->getScheme());
var_dump($url2->getScheme());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(5) "https"
string(4) "http"
string(19) "http://example.com/"
