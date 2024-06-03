--TEST--
Test property mutation - host
--EXTENSIONS--
uri
--XFAIL--
Cloning Rfc3986Uris doesn't copy the path properly yet
--FILE--
<?php

$uri1 = Uri\Rfc3986Uri::create("https://example.com");
$uri2 = $uri1->withHost("test.com");
$uri3 = $uri2->withHost(null);

var_dump($uri1->__toString());
var_dump($uri2->__toString());
var_dump($uri3->__toString());

$uri1 = Uri\Rfc3986Uri::create("ftp://foo.com?query=abc#foo");
$uri2 = $uri1->withHost("test.com");

var_dump($uri1->__toString());
var_dump($uri2->__toString());

$uri1 = Uri\Rfc3986Uri::create("/path?query=abc#foo");
$uri2 = $uri1->withHost("test.com");

var_dump($uri1->__toString());
var_dump($uri2->__toString());

$uri1 = Uri\WhatWgUri::create("https://example.com");
$uri2 = $uri1->withHost("test.com");
$uri3 = $uri2->withHost(null);

var_dump($uri1->__toString());
var_dump($uri2->__toString());
var_dump($uri3->__toString());

$uri1 = Uri\WhatWgUri::create("ftp://foo.com?query=abc#foo");
$uri2 = $uri1->withHost("test.com");

var_dump($uri1->__toString());
var_dump($uri2->__toString());

$uri1 = Uri\WhatWgUri::create("/path?query=abc#foo");
$uri2 = $uri1->withHost("test.com");

var_dump($uri1->__toString());
var_dump($uri2->__toString());

?>
--EXPECT--
string(19) "https://example.com"
string(16) "https://test.com"
string(8) "https://"
string(27) "ftp://foo.com?query=abc#foo"
string(28) "ftp://test.com?query=abc#foo"
