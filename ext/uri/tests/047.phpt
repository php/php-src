--TEST--
Test IP addresses
--EXTENSIONS--
uri
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri("https://192.168.0.1");
var_dump($uri->getRawHost());
var_dump($uri->getHost());
var_dump($uri->toRawString());
var_dump($uri->toString());

$uri = new Uri\Rfc3986\Uri("https://[2001:0db8:0001:0000:0000:0ab9:C0A8:0102]");
var_dump($uri->getRawHost());
var_dump($uri->getHost());
var_dump($uri->toRawString());
var_dump($uri->toString());

$uri = new Uri\Rfc3986\Uri("//[v7.host]/source");
var_dump($uri->getRawHost());
var_dump($uri->getHost());
var_dump($uri->toRawString());
var_dump($uri->toString());

$url = new Uri\WhatWg\Url("https://192.168.0.1");
var_dump($url->getAsciiHost());
var_dump($url->toAsciiString());

$url = new Uri\WhatWg\Url("https://[2001:0db8:0001:0000:0000:0ab9:C0A8:0102]");
var_dump($url->getAsciiHost());
var_dump($url->toAsciiString());

$url = new Uri\WhatWg\Url("https://[0:0::1]");
var_dump($url->getAsciiHost());
var_dump($url->toAsciiString());

?>
--EXPECT--
string(11) "192.168.0.1"
string(11) "192.168.0.1"
string(19) "https://192.168.0.1"
string(19) "https://192.168.0.1"
string(41) "[2001:0db8:0001:0000:0000:0ab9:C0A8:0102]"
string(41) "[2001:0db8:0001:0000:0000:0ab9:c0a8:0102]"
string(49) "https://[2001:0db8:0001:0000:0000:0ab9:c0a8:0102]"
string(49) "https://[2001:0db8:0001:0000:0000:0ab9:c0a8:0102]"
string(9) "[v7.host]"
string(9) "[v7.host]"
string(18) "//[v7.host]/source"
string(18) "//[v7.host]/source"
string(11) "192.168.0.1"
string(20) "https://192.168.0.1/"
string(26) "[2001:db8:1::ab9:c0a8:102]"
string(35) "https://[2001:db8:1::ab9:c0a8:102]/"
string(5) "[::1]"
string(14) "https://[::1]/"
