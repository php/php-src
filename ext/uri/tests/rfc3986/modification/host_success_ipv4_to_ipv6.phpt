--TEST--
Test Uri\Rfc3986\Uri component modification - host - changing IPv4 to IPv6
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://192.168.0.1");
$uri2 = $uri1->withHost("[2001:0db8:3333:4444:5555:6666:7777:8888]");

var_dump($uri1->getRawHost());

var_dump($uri2->getRawHost());
var_dump($uri2->toRawString());
var_dump($uri2->getHost());
var_dump($uri2->toString());

?>
--EXPECT--
string(11) "192.168.0.1"
string(41) "[2001:0db8:3333:4444:5555:6666:7777:8888]"
string(49) "https://[2001:0db8:3333:4444:5555:6666:7777:8888]"
string(41) "[2001:0db8:3333:4444:5555:6666:7777:8888]"
string(49) "https://[2001:0db8:3333:4444:5555:6666:7777:8888]"
