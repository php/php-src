--TEST--
Test Uri\WhatWg\Url component modification - path - changing an existing one
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com/foo/bar");
$url2 = $url1->withPath("/baz");

var_dump($url1->getPath());
var_dump($url2->getPath());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(8) "/foo/bar"
string(4) "/baz"
string(23) "https://example.com/baz"
