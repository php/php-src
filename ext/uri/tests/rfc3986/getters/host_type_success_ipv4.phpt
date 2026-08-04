--TEST--
Test Uri\Rfc3986\Uri component retrieval - host type - IPv4
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://192.168.0.1");

var_dump($uri->getHostType());

?>
--EXPECT--
enum(Uri\Rfc3986\UriHostType::IPv4)
