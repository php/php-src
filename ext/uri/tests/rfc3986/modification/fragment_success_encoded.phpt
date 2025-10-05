--TEST--
Test Uri\Rfc3986\Uri component modification - fragment - URL encoded characters
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withFragment("foo%3db%61r"); // foo=bar

var_dump($uri1->getRawFragment());
var_dump($uri2->getRawFragment());
var_dump($uri2->toRawString());
var_dump($uri2->getFragment());
var_dump($uri2->toString());

?>
--EXPECT--
NULL
string(11) "foo%3db%61r"
string(31) "https://example.com#foo%3db%61r"
string(9) "foo%3Dbar"
string(29) "https://example.com#foo%3Dbar"
