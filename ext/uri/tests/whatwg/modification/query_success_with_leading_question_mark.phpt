--TEST--
Test Uri\WhatWg\Url component modification - query - with leading question mark
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com/foo/bar");
$url2 = $url1->withQuery("?foo=bar");

var_dump($url1->getQuery());
var_dump($url2->getQuery());
var_dump($url2->toAsciiString());

?>
--EXPECT--
NULL
string(7) "foo=bar"
string(35) "https://example.com/foo/bar?foo=bar"
