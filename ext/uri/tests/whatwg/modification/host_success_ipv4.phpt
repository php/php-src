--TEST--
Test Uri\WhatWg\Url component modification - host - IPv4 address
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withHost("192.168.0.1");

var_dump($url1->getAsciiHost());
var_dump($url2->getAsciiHost());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(11) "example.com"
string(11) "192.168.0.1"
string(20) "https://192.168.0.1/"
