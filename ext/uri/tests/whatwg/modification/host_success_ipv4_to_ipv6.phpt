--TEST--
Test Uri\WhatWg\Url component modification - host - changing IPv4 to IPv6
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withHost("192.168.0.1");
$url3 = $url2->withHost("[2001:db8:3333:4444:5555:6666:7777:8888]");

var_dump($url2->getUnicodeHost());
var_dump($url2->getAsciiHost());
var_dump($url2->toUnicodeString());
var_dump($url2->toAsciiString());

var_dump($url3->getUnicodeHost());
var_dump($url3->getAsciiHost());
var_dump($url3->toUnicodeString());
var_dump($url3->toAsciiString());

?>
--EXPECT--
string(11) "192.168.0.1"
string(11) "192.168.0.1"
string(20) "https://192.168.0.1/"
string(20) "https://192.168.0.1/"
string(40) "[2001:db8:3333:4444:5555:6666:7777:8888]"
string(40) "[2001:db8:3333:4444:5555:6666:7777:8888]"
string(49) "https://[2001:db8:3333:4444:5555:6666:7777:8888]/"
string(49) "https://[2001:db8:3333:4444:5555:6666:7777:8888]/"
