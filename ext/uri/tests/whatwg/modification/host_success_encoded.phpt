--TEST--
Test Uri\WhatWg\Url component modification - host - URL encoded characters
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withHost("%65xample.net"); // example.net

var_dump($url1->getAsciiHost());
var_dump($url2->getAsciiHost());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(11) "example.com"
string(11) "example.net"
string(20) "https://example.net/"
