--TEST--
Test property mutation - path
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com/foo/bar");
$uri2 = $uri1->withPath("/foo");
$uri3 = $uri2->withPath(null);

var_dump($uri1->toString());
var_dump($uri2->toString());
var_dump($uri3->toString());

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withPath("/foo");

var_dump($uri1->toString());
var_dump($uri2->toString());

$uri1 = Uri\Rfc3986\Uri::parse("/path?query=abc#foo");
$uri2 = $uri1->withPath("/");

var_dump($uri1->toString());
var_dump($uri2->toString());

$url1 = Uri\WhatWg\Url::parse("https://example.com/foo/bar");
$url2 = $url1->withPath("/foo");
$url3 = $url2->withPath(null);

var_dump($url1->toMachineFriendlyString());
var_dump($url2->toMachineFriendlyString());
var_dump($url3->toMachineFriendlyString());

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$uri2 = $url1->withPath("/foo");

var_dump($url1->toMachineFriendlyString());
var_dump($url2->toMachineFriendlyString());

?>
--EXPECT--
string(27) "https://example.com/foo/bar"
string(23) "https://example.com/foo"
string(19) "https://example.com"
string(19) "https://example.com"
string(23) "https://example.com/foo"
string(19) "/path?query=abc#foo"
string(15) "/?query=abc#foo"
string(27) "https://example.com/foo/bar"
string(23) "https://example.com/foo"
string(20) "https://example.com/"
string(20) "https://example.com/"
string(23) "https://example.com/foo"
