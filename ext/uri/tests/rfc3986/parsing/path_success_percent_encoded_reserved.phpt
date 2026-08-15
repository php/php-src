--TEST--
Test Uri\Rfc3986\Uri parsing - path - percent-encoded reserved character
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri("https://example.com/foo/bar%2Fbaz");

var_dump($uri->toRawString());
var_dump($uri->getPath());
var_dump($uri->getRawPath());

?>
--EXPECT--
string(33) "https://example.com/foo/bar%2Fbaz"
string(14) "/foo/bar%2Fbaz"
string(14) "/foo/bar%2Fbaz"
