--TEST--
Test property mutation - path
--EXTENSIONS--
uri
--XFAIL--
uriparser percent-encoded paths
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com/foo/bar");
$uri2 = $uri1->withPath("/foo");
$uri3 = $uri2->withPath("");
$uri4 = $uri3->withPath("t%65st");
$uri5 = $uri4->withPath("/foo%2Fbar");
$uri6 = $uri5->withPath("/#");

var_dump($uri1->getRawPath());
var_dump($uri2->getRawPath());
var_dump($uri3->getRawPath());
var_dump($uri4->getRawPath());
var_dump($uri5->getRawPath());
var_dump($uri6->getRawPath());

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withPath("/foo");

var_dump($uri1->getRawPath());
var_dump($uri2->getRawPath());

$uri1 = Uri\Rfc3986\Uri::parse("/path?query=abc#foo");
$uri2 = $uri1->withPath("/");

var_dump($uri1->getRawPath());
var_dump($uri2->getRawPath());

$url1 = Uri\WhatWg\Url::parse("https://example.com/foo/bar");
$url2 = $url1->withPath("/foo");
$url3 = $url2->withPath("");
$url4 = $url3->withPath("t%65st");
$url5 = $url4->withPath("/foo%2Fbar");
$url6 = $url5->withPath("/#");

var_dump($url1->getRawPath());
var_dump($url2->getRawPath());
var_dump($url3->getRawPath());
var_dump($url4->getRawPath());
var_dump($url5->getRawPath());
var_dump($url6->getRawPath());

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$uri2 = $url1->withPath("/foo");

var_dump($url1->getRawPath());
var_dump($url2->getRawPath());

?>
--EXPECT--
string(8) "/foo/bar"
string(4) "/foo"
string(0) ""
string(0) ""
string(10) "/foo%2Fbar"
string(4) "/%3F"
string(0) ""
string(4) "/foo"
string(5) "/path"
string(1) "/"
string(8) "/foo/bar"
string(4) "/foo"
string(1) "/"
string(7) "/t%65st"
string(10) "/foo%2Fbar"
string(4) "/%23"
string(1) "/"
string(4) "/foo"
