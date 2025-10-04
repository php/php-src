--TEST--
Test Uri\WhatWg\Url component modification - query - URL encoded characters
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withQuery("foo%3dbar"); // foo=bar

var_dump($url1->getQuery());
var_dump($url2->getQuery());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
string(9) "foo%3dbar"
string(30) "https://example.com/?foo%3dbar"
