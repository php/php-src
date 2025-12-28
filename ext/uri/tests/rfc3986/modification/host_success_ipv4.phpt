--TEST--
Test Uri\Rfc3986\Uri component modification - host - IPv4 address
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withHost("192.168.0.1");

var_dump($uri1->getRawHost());
var_dump($uri2->getRawHost());
var_dump($uri2->toRawString());
var_dump($uri2->getHost());
var_dump($uri2->toString());

?>
--EXPECT--
string(11) "example.com"
string(11) "192.168.0.1"
string(19) "https://192.168.0.1"
string(11) "192.168.0.1"
string(19) "https://192.168.0.1"
