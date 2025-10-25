--TEST--
Test Uri\WhatWg\Url component modification - query - context-sensitive reserved character
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withQuery("?foo=bar");

var_dump($url1->getQuery());
var_dump($url2->getQuery());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
string(7) "foo=bar"
string(28) "https://example.com/?foo=bar"
