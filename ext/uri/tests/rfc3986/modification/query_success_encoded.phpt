--TEST--
Test Uri\Rfc3986\Uri component modification - query - URL encoded characters
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withQuery("foo%3dbar"); // foo=bar

var_dump($uri1->getRawQuery());
var_dump($uri2->getRawQuery());
var_dump($uri2->toRawString());
var_dump($uri2->getQuery());
var_dump($uri2->toString());

?>
--EXPECT--
NULL
string(9) "foo%3dbar"
string(29) "https://example.com?foo%3dbar"
string(9) "foo%3Dbar"
string(29) "https://example.com?foo%3Dbar"
