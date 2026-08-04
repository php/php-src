--TEST--
Test Uri\Rfc3986\Uri component retrieval - host type - registered name
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://example.com");

var_dump($uri->getHostType());

?>
--EXPECT--
enum(Uri\Rfc3986\UriHostType::RegisteredName)
