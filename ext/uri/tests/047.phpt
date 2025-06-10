--TEST--
Test IP addresses
--EXTENSIONS--
uri
--FILE--
<?php

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
string(20) "https://192.168.0.1/"
string(26) "[2001:db8:1::ab9:c0a8:102]"
string(35) "https://[2001:db8:1::ab9:c0a8:102]/"
string(5) "[::1]"
string(14) "https://[::1]/"
