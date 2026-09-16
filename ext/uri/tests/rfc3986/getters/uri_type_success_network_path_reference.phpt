--TEST--
Test Uri\Rfc3986\Uri getter - uri type - Network path reference
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("//example.com/foo/bar");

var_dump($uri->getUriType());

?>
--EXPECT--
enum(Uri\Rfc3986\UriType::NetworkPathReference)
