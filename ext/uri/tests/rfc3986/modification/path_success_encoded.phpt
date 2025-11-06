--TEST--
Test Uri\Rfc3986\Uri component modification - path - URL encoded characters
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withPath("/foo%2Fb%61r"); // /foo/bar

var_dump($uri1->getRawPath());
var_dump($uri2->getRawPath());
var_dump($uri2->toRawString());
var_dump($uri2->getPath());
var_dump($uri2->toString());

?>
--EXPECT--
string(0) ""
string(12) "/foo%2Fb%61r"
string(31) "https://example.com/foo%2Fb%61r"
string(10) "/foo%2Fbar"
string(29) "https://example.com/foo%2Fbar"
