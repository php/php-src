--TEST--
Test property mutation - scheme
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986Uri::create("https://example.com");
$uri2 = $uri1->withScheme("http");
$uri3 = $uri2->withScheme(null);

var_dump($uri1->__toString());
var_dump($uri2->__toString());
var_dump($uri3->__toString());

$uri1 = Uri\WhatWgUri::create("https://example.com");
$uri2 = $uri1->withScheme("http");
$uri3 = $uri2->withScheme(null);

var_dump($uri1->__toString());
var_dump($uri2->__toString());
var_dump($uri3->__toString());

?>
--EXPECT--
string(19) "https://example.com"
string(18) "http://example.com"
string(11) "example.com"
string(20) "https://example.com/"
string(20) "https://example.com/"
string(20) "https://example.com/"
