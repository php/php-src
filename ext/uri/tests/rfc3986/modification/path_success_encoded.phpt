--TEST--
Test Uri\Rfc3986\Uri component modification - path - URL encoded characters
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withPath("/foo%2Fbar"); // /foo/bar

var_dump($uri1->getRawPath());
var_dump($uri2->getRawPath());
var_dump($uri2->getPath());

?>
--EXPECT--
string(0) ""
string(10) "/foo%2Fbar"
string(10) "/foo%2Fbar"
