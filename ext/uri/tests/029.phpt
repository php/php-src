--TEST--
Test property mutation - query
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com?foo=bar");
$uri2 = $uri1->withQuery("?foo=baz");
$uri3 = $uri2->withQuery(null);
$uri4 = $uri3->withQuery("t%65st");
$uri5 = $uri4->withQuery("foo=foo%26bar&baz=/qux%3D");
$uri6 = $uri5->withQuery("#");

var_dump($uri1->getRawQuery());
var_dump($uri2->getRawQuery());
var_dump($uri3->getRawQuery());
var_dump($uri4->getRawQuery());
var_dump($uri5->getRawQuery());
var_dump($uri6->getRawQuery());

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withQuery("?foo=bar&foo=baz");
$uri3 = $uri1->withQuery("foo=bar&foo=baz");

var_dump($uri1->getRawQuery());
var_dump($uri2->getRawQuery());
var_dump($uri3->getRawQuery());

$url1 = Uri\WhatWg\Url::parse("https://example.com?foo=bar");
$url2 = $url1->withQuery("?foo=baz");
$url3 = $url2->withQuery(null);

var_dump($url1->getRawQuery());
var_dump($url2->getRawQuery());
var_dump($url3->getRawQuery());

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withQuery("?foo=bar&foo=baz");
$url3 = $url1->withQuery("foo=bar&foo=baz");
$url4 = $url3->withQuery("t%65st");
$url5 = $url4->withQuery("foo=foo%26bar&baz=/qux%3D");
$url6 = $url5->withQuery("#");

var_dump($url1->getRawQuery());
var_dump($url2->getRawQuery());
var_dump($url3->getRawQuery());
var_dump($url4->getRawQuery());
var_dump($url5->getRawQuery());
var_dump($url6->getRawQuery());

?>
--EXPECT--
string(7) "foo=bar"
string(7) "foo=baz"
NULL
string(6) "t%65st"
string(25) "foo=foo%26bar&baz=/qux%3D"
string(0) "#"
NULL
string(15) "foo=bar&foo=baz"
string(15) "foo=bar&foo=baz"
string(7) "foo=bar"
string(7) "foo=baz"
NULL
NULL
string(15) "foo=bar&foo=baz"
string(15) "foo=bar&foo=baz"
string(6) "t%65st"
string(25) "foo=foo%26bar&baz=/qux%3D"
string(3) "%23"
