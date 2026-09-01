--TEST--
Test Uri\Rfc3986\Uri component retrieval - URI type - network-path reference
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("//example.com/foo/bar");

var_dump($uri->getUriType());

?>
--EXPECT--
enum(Uri\Rfc3986\UriType::NetworkPathReference)
