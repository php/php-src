--TEST--
Test Uri\Rfc3986\Uri component modification - host - changing IPv4 to IPv6
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withHost(null);
$uri3 = $uri2->withHost("192.168.0.1");
$uri4 = $uri3->withHost("[2001:db8:3333:4444:5555:6666:7777:8888]");

var_dump($uri3->getRawHost());
var_dump($uri3->getHost());
var_dump($uri4->getRawHost());
var_dump($uri4->getHost());

?>
--EXPECT--
string(11) "192.168.0.1"
string(11) "192.168.0.1"
string(40) "[2001:db8:3333:4444:5555:6666:7777:8888]"
string(40) "[2001:db8:3333:4444:5555:6666:7777:8888]"
