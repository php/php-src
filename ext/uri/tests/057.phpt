--TEST--
Test normalization after using RFC 3986 withers
--EXTENSIONS--
uri
--FILE--
<?php

$uri = \Uri\Rfc3986\Uri::parse('https://example.com/path-%65xample/');
var_dump($uri->getPath());
var_dump($uri->getRawPath());

$newUri = $uri->withPath('/updated-path');
var_dump($newUri->getPath());
var_dump($newUri->getRawPath());

?>
--EXPECT--
string(14) "/path-example/"
string(16) "/path-%65xample/"
string(13) "/updated-path"
string(13) "/updated-path"
