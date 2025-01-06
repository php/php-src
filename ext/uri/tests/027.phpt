--TEST--
Test property mutation - port
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com:8080");
$uri2 = $uri1->withPort(22);
$uri3 = $uri2->withPort(null);

var_dump($uri1->toString());
var_dump($uri2->toString());
var_dump($uri3->toString());

$uri1 = Uri\Rfc3986\Uri::parse("ftp://foo.com:443?query=abc#foo");
$uri2 = $uri1->withPort(8080);

var_dump($uri1->toString());
var_dump($uri2->toString());

$url1 = Uri\WhatWg\Url::parse("https://example.com:8080");
$url2 = $url1->withPort(22);
$url3 = $url2->withPort(null);

var_dump($url1->toMachineFriendlyString());
var_dump($url2->toMachineFriendlyString());
var_dump($url3->toMachineFriendlyString());

$url1 = Uri\WhatWg\Url::parse("ftp://foo.com:443?query=abc#foo");
$url2 = $url1->withPort(8080);

var_dump($url1->toMachineFriendlyString());
var_dump($url2->toMachineFriendlyString());

?>
--EXPECT--
string(24) "https://example.com:8080"
string(22) "https://example.com:22"
string(19) "https://example.com"
string(31) "ftp://foo.com:443?query=abc#foo"
string(32) "ftp://foo.com:8080?query=abc#foo"
string(25) "https://example.com:8080/"
string(23) "https://example.com:22/"
string(20) "https://example.com/"
string(32) "ftp://foo.com:443/?query=abc#foo"
string(33) "ftp://foo.com:8080/?query=abc#foo"
