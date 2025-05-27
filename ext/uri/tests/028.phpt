--TEST--
Test property mutation - path
--EXTENSIONS--
uri
--FILE--
<?php

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
string(4) "/foo"
string(1) "/"
string(7) "/t%65st"
string(10) "/foo%2Fbar"
string(4) "/%23"
string(1) "/"
string(4) "/foo"
