--TEST--
Test property mutation - path
--EXTENSIONS--
uri
--XFAIL--
Cloning Rfc3986Uris doesn't copy the path properly yet
--FILE--
<?php

$uri1 = Uri\Rfc3986Uri::parse("https://example.com/foo/bar");
$uri2 = $uri1->withPath("/foo");
$uri3 = $uri2->withPath(null);

var_dump($uri1->toString());
var_dump($uri2->toString());
var_dump($uri3->toString());

$uri1 = Uri\Rfc3986Uri::parse("https://example.com");
$uri2 = $uri1->withPath("/foo");

var_dump($uri1->toString());
var_dump($uri2->toString());

$uri1 = Uri\Rfc3986Uri::parse("/path?query=abc#foo");
$uri2 = $uri1->withPath("/");

var_dump($uri1->toString());
var_dump($uri2->toString());

$uri1 = Uri\WhatWgUri::parse("https://example.com/foo/bar");
$uri2 = $uri1->withPath("/foo");
$uri3 = $uri2->withPath(null);

var_dump($uri1->toString());
var_dump($uri2->toString());
var_dump($uri3->toString());

$uri1 = Uri\WhatWgUri::parse("https://example.com");
$uri2 = $uri1->withPath("/foo");

var_dump($uri1->toString());
var_dump($uri2->toString());

?>
--EXPECT--
string(27) "https://example.com/foo/bar"
string(23) "https://example.com/foo"
string(19) "https://example.com"
string(19) "https://example.com"
string(23) "https://example.com/foo"
string(27) "https://example.com/foo/bar"
string(23) "https://example.com/foo"
string(20) "https://example.com/"
string(20) "https://example.com/"
string(23) "https://example.com/foo"
