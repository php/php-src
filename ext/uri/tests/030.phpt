--TEST--
Test property mutation - fragment
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com#fragment1");
$uri2 = $uri1->withFragment("#fragment2");
$uri3 = $uri2->withFragment(null);

var_dump($uri1->getRawFragment());
var_dump($uri2->getRawFragment());
var_dump($uri3->getRawFragment());

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com?abc=def");
$uri2 = $uri1->withFragment("#fragment");

var_dump($uri1->getRawFragment());
var_dump($uri2->getRawFragment());

$url1 = Uri\WhatWg\Url::parse("https://example.com#fragment1");
$url2 = $url1->withFragment("#fragment2");
$url3 = $url2->withFragment(null);
$url4 = $url3->withFragment(" ");

var_dump($url1->getFragment());
var_dump($url2->getFragment());
var_dump($url3->getFragment());
var_dump($url4->getFragment());

$url1 = Uri\WhatWg\Url::parse("https://example.com?abc=def");
$url2 = $url1->withFragment("#fragment");

var_dump($url1->getFragment());
var_dump($url2->getFragment());

?>
--EXPECT--
string(9) "fragment1"
string(9) "fragment2"
NULL
NULL
string(8) "fragment"
string(9) "fragment1"
string(9) "fragment2"
NULL
string(3) "%20"
NULL
string(8) "fragment"
