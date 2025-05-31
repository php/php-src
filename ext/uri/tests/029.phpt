--TEST--
Test property mutation - query
--EXTENSIONS--
uri
--FILE--
<?php

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
string(7) "foo=baz"
NULL
NULL
string(15) "foo=bar&foo=baz"
string(15) "foo=bar&foo=baz"
string(6) "t%65st"
string(25) "foo=foo%26bar&baz=/qux%3D"
string(3) "%23"
