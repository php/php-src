--TEST--
Test Uri\Rfc3986\Uri component modification - host - URL encoded characters
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withHost("%65xample.net"); // example.net

var_dump($uri1->getRawHost());
var_dump($uri2->getRawHost());
var_dump($uri2->toRawString());
var_dump($uri2->getHost());
var_dump($uri2->toString());

?>
--EXPECT--
string(11) "example.com"
string(13) "%65xample.net"
string(21) "https://%65xample.net"
string(11) "example.net"
string(19) "https://example.net"
