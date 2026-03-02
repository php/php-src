--TEST--
Test Uri\WhatWg\Url component modification - path - without leading slash
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withPath("foo/bar");

var_dump($url1->getPath());
var_dump($url2->getPath());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(1) "/"
string(8) "/foo/bar"
string(27) "https://example.com/foo/bar"
