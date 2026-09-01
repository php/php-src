--TEST--
Test Uri\Rfc3986\Uri component retrieval - URI type - URI with empty authority
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://");

var_dump($uri->getUriType());

?>
--EXPECT--
enum(Uri\Rfc3986\UriType::Uri)
