--TEST--
Test Uri\WhatWg\Url component modification - host - success - changing IPv4 to IPv6
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://192.168.0.1");
$url2 = $url1->withHost("[2001:db8:3333:4444:5555:6666:7777:8888]");

var_dump($url1->getAsciiHost());

var_dump($url2->getAsciiHost());
var_dump($url2->toAsciiString());

?>
--EXPECT--
string(11) "192.168.0.1"
string(40) "[2001:db8:3333:4444:5555:6666:7777:8888]"
string(49) "https://[2001:db8:3333:4444:5555:6666:7777:8888]/"
