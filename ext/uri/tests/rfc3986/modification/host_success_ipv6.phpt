--TEST--
Test Uri\Rfc3986\Uri component modification - host - IPv6 address
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withHost("[2001:0db8:3333:4444:5555:6666:7777:8888]");

var_dump($uri1->getRawHost());
var_dump($uri2->getRawHost());
var_dump($uri2->toRawString());
var_dump($uri2->getHost());
var_dump($uri2->toString());

?>
--EXPECT--
string(11) "example.com"
string(41) "[2001:0db8:3333:4444:5555:6666:7777:8888]"
string(49) "https://[2001:0db8:3333:4444:5555:6666:7777:8888]"
string(41) "[2001:0db8:3333:4444:5555:6666:7777:8888]"
string(49) "https://[2001:0db8:3333:4444:5555:6666:7777:8888]"
