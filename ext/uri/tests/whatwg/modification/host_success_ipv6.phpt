--TEST--
Test Uri\WhatWg\Url component modification - host - IPv6 address
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withHost("[2001:0db8:3333:4444:5555:6666:7777:8888]");

var_dump($url1->getAsciiHost());
var_dump($url2->getAsciiHost());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(11) "example.com"
string(40) "[2001:db8:3333:4444:5555:6666:7777:8888]"
string(49) "https://[2001:db8:3333:4444:5555:6666:7777:8888]/"
