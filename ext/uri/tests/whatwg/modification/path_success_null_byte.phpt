--TEST--
Test Uri\WhatWg\Url component modification - path - null byte
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withPath("/p\0th\0");

var_dump($url1->getPath());
var_dump($url2->getPath());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(1) "/"
string(10) "/p%00th%00"
string(29) "https://example.com/p%00th%00"
