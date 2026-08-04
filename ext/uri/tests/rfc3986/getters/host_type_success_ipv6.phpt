--TEST--
Test Uri\Rfc3986\Uri component retrieval - host type - IPv6
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://[2001:0db8:3333:4444:5555:6666:7777:8888]");

var_dump($uri->getHostType());

?>
--EXPECT--
enum(Uri\Rfc3986\UriHostType::IPv6)
