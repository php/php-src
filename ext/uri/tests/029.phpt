--TEST--
Test property mutation - query
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986Uri::parse("https://example.com?foo=bar");
$uri2 = $uri1->withQuery("?foo=baz");
$uri3 = $uri2->withQuery(null);

var_dump($uri1->__toString());
var_dump($uri2->__toString());
var_dump($uri3->__toString());

$uri1 = Uri\Rfc3986Uri::parse("https://example.com");
$uri2 = $uri1->withQuery("?foo=bar&foo=baz");
$uri3 = $uri1->withQuery("foo=bar&foo=baz");

var_dump($uri1->__toString());
var_dump($uri2->__toString());
var_dump($uri3->__toString());

$uri1 = Uri\WhatWgUri::parse("https://example.com?foo=bar");
$uri2 = $uri1->withQuery("?foo=baz");
$uri3 = $uri2->withQuery(null);

var_dump($uri1->__toString());
var_dump($uri2->__toString());
var_dump($uri3->__toString());

$uri1 = Uri\WhatWgUri::parse("https://example.com");
$uri2 = $uri1->withQuery("?foo=bar&foo=baz");
$uri3 = $uri1->withQuery("foo=bar&foo=baz");

var_dump($uri1->__toString());
var_dump($uri2->__toString());
var_dump($uri3->__toString());

?>
--EXPECT--
string(27) "https://example.com?foo=bar"
string(27) "https://example.com?foo=baz"
string(19) "https://example.com"
string(19) "https://example.com"
string(35) "https://example.com?foo=bar&foo=baz"
string(35) "https://example.com?foo=bar&foo=baz"
string(28) "https://example.com/?foo=bar"
string(28) "https://example.com/?foo=baz"
string(20) "https://example.com/"
string(20) "https://example.com/"
string(36) "https://example.com/?foo=bar&foo=baz"
string(36) "https://example.com/?foo=bar&foo=baz"
