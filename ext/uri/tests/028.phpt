--TEST--
Test property mutation - path
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com/foo/bar/");
var_dump($uri1->getRawPath());
var_dump($uri1->getPath());

$uri2 = $uri1->withPath("/foo");
var_dump($uri2->getRawPath());
var_dump($uri2->getPath());

$uri3 = $uri2->withPath("/t%65st");
var_dump($uri3->getRawPath());
var_dump($uri3->getPath());

$uri4 = $uri3->withPath("/foo%2Fbar");
var_dump($uri4->getRawPath());
var_dump($uri4->getPath());

$uri5 = $uri4->withPath("");
var_dump($uri5->getRawPath());
var_dump($uri5->getPath());

try {
    $uri5->withPath("test");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

try {
    $uri5->withPath("/#");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

$uri1 = Uri\Rfc3986\Uri::parse("/foo");
$uri2 = $uri1->withPath("bar");

var_dump($uri1->getPath());
var_dump($uri2->getPath());

$url1 = Uri\WhatWg\Url::parse("https://example.com/foo/bar/");
$url2 = $url1->withPath("/foo");
$url3 = $url2->withPath("");
$url4 = $url3->withPath("t%65st");
$url5 = $url4->withPath("/foo%2Fbar");
$url6 = $url5->withPath("/#");

var_dump($url1->getPath());
var_dump($url2->getPath());
var_dump($url3->getPath());
var_dump($url4->getPath());
var_dump($url5->getPath());
var_dump($url6->getPath());

$url1 = Uri\WhatWg\Url::parse("https://example.com/");
$uri2 = $url1->withPath("/foo");

var_dump($url1->getPath());
var_dump($url2->getPath());

?>
--EXPECT--
string(9) "/foo/bar/"
string(9) "/foo/bar/"
string(4) "/foo"
string(4) "/foo"
string(7) "/t%65st"
string(5) "/test"
string(10) "/foo%2Fbar"
string(10) "/foo%2Fbar"
string(0) ""
string(0) ""
The specified path is malformed
The specified path is malformed
string(4) "/foo"
string(3) "bar"
string(9) "/foo/bar/"
string(4) "/foo"
string(1) "/"
string(7) "/t%65st"
string(10) "/foo%2Fbar"
string(4) "/%23"
string(1) "/"
string(4) "/foo"
