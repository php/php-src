--TEST--
Test property mutation - query
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com?foo=bar");
var_dump($uri1->getRawQuery());
var_dump($uri1->getQuery());

$uri2 = $uri1->withQuery("foo=baz");
var_dump($uri2->getRawQuery());
var_dump($uri2->getQuery());

$uri3 = $uri2->withQuery(null);
var_dump($uri3->getRawQuery());
var_dump($uri3->getQuery());

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
var_dump($uri1->getRawQuery());
var_dump($uri1->getQuery());

$uri2 = $uri1->withQuery("?foo=bar&foo=baz");
var_dump($uri2->getRawQuery());
var_dump($uri2->getQuery());

$uri3 = $uri1->withQuery("foo=bar&foo=baz");
var_dump($uri3->getRawQuery());
var_dump($uri3->getQuery());

$uri4 = $uri3->withQuery("t%65st");
var_dump($uri4->getRawQuery());
var_dump($uri4->getQuery());

$uri5 = $uri4->withQuery("foo=foo%26bar&baz=/qux%3D");
var_dump($uri5->getRawQuery());
var_dump($uri5->getQuery());

try {
    $uri5->withQuery("#");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

$url1 = Uri\WhatWg\Url::parse("https://example.com?foo=bar");
$url2 = $url1->withQuery("?foo=baz");
$url3 = $url2->withQuery(null);

var_dump($url1->getQuery());
var_dump($url2->getQuery());
var_dump($url3->getQuery());

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withQuery("?foo=bar&foo=baz");
$url3 = $url1->withQuery("foo=bar&foo=baz");
$url4 = $url3->withQuery("t%65st");
$url5 = $url4->withQuery("foo=foo%26bar&baz=/qux%3D");
$url6 = $url5->withQuery("#");

var_dump($url1->getQuery());
var_dump($url2->getQuery());
var_dump($url3->getQuery());
var_dump($url4->getQuery());
var_dump($url5->getQuery());
var_dump($url6->getQuery());

?>
--EXPECT--
string(7) "foo=bar"
string(7) "foo=bar"
string(7) "foo=baz"
string(7) "foo=baz"
NULL
NULL
NULL
NULL
string(16) "?foo=bar&foo=baz"
string(16) "?foo=bar&foo=baz"
string(15) "foo=bar&foo=baz"
string(15) "foo=bar&foo=baz"
string(6) "t%65st"
string(4) "test"
string(25) "foo=foo%26bar&baz=/qux%3D"
string(25) "foo=foo%26bar&baz=/qux%3D"
The specified query is malformed
string(7) "foo=bar"
string(7) "foo=baz"
NULL
NULL
string(15) "foo=bar&foo=baz"
string(15) "foo=bar&foo=baz"
string(6) "t%65st"
string(25) "foo=foo%26bar&baz=/qux%3D"
string(3) "%23"
