--TEST--
Test Uri\Rfc3986\Uri component retrieval - URI type - URI
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://example.com");

var_dump($uri->getUriType());

?>
--EXPECT--
enum(Uri\Rfc3986\UriType::Uri)
