--TEST--
Test Uri\Rfc3986\Uri component retrieval - URI type - absolute-path reference
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("/foo/bar");

var_dump($uri->getUriType());

?>
--EXPECT--
enum(Uri\Rfc3986\UriType::AbsolutePathReference)
