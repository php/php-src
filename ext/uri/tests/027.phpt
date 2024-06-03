--TEST--
Test property mutation - port
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986Uri::create("https://example.com:8080");
$uri2 = $uri1->withPort(22);
$uri3 = $uri2->withPort(null);

var_dump($uri1->__toString());
var_dump($uri2->__toString());
var_dump($uri3->__toString());

$uri1 = Uri\Rfc3986Uri::create("ftp://foo.com:443?query=abc#foo");
$uri2 = $uri1->withPort(8080);

var_dump($uri1->__toString());
var_dump($uri2->__toString());

$uri1 = Uri\WhatWgUri::create("https://example.com:8080");
$uri2 = $uri1->withPort(22);
$uri3 = $uri2->withPort(null);

var_dump($uri1->__toString());
var_dump($uri2->__toString());
var_dump($uri3->__toString());

$uri1 = Uri\WhatWgUri::create("ftp://foo.com:443?query=abc#foo");
$uri2 = $uri1->withPort(8080);

var_dump($uri1->__toString());
var_dump($uri2->__toString());

?>
--EXPECT--
string(24) "https://example.com:8080"
string(22) "https://example.com:22"
string(19) "https://example.com"
string(31) "ftp://foo.com:443?query=abc#foo"
string(32) "ftp://foo.com:8080?query=abc#foo"
string(25) "https://example.com:8080/"
string(25) "https://example.com:8080/"
string(25) "https://example.com:8080/"
string(32) "ftp://foo.com:443/?query=abc#foo"
string(32) "ftp://foo.com:443/?query=abc#foo"
