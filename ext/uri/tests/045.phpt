--TEST--
Test percent-decoding of reserved characters in the path
--EXTENSIONS--
uri
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri("https://example.com/foo/bar%2Fbaz");

var_dump($uri->toString());
var_dump($uri->getPath());
var_dump($uri->getRawPath());

$url = new Uri\Whatwg\Url("https://example.com/foo/bar%2Fbaz");

var_dump($url->toString());
var_dump($url->getPath());
var_dump($url->getRawPath());

?>
--EXPECTF--
string(33) "https://example.com/foo/bar%2Fbaz"
string(14) "/foo/bar%2Fbaz"
string(14) "/foo/bar%2Fbaz"
string(33) "https://example.com/foo/bar%2Fbaz"
string(12) "/foo/bar/baz"
string(14) "/foo/bar%2Fbaz"
