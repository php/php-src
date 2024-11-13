--TEST--
Test normalization
--EXTENSIONS--
uri
--XFAIL--
Cloning Rfc3986Uris doesn't copy the path properly yet
--FILE--
<?php

$uri1 = Uri\Rfc3986Uri::create("HTTPS://////EXAMPLE.com");
$uri2 = $uri1->normalize();
$uri3 = Uri\Rfc3986Uri::create("https://example.com")->normalize();
$uri4 = Uri\Rfc3986Uri::create("https://example.com/foo/..")->normalize();

var_dump($uri1->__toString());
var_dump($uri2->__toString());
var_dump($uri3->__toString());
var_dump($uri4->__toString());

$uri1 = Uri\WhatWgUri::create("HTTPS://////EXAMPLE.com");
$uri2 = $uri1->normalize();
$uri3 = Uri\WhatWgUri::create("https://example.com")->normalize();
$uri4 = Uri\WhatWgUri::create("https://example.com/foo/..")->normalize();

var_dump($uri1->__toString());
var_dump($uri2->__toString());
var_dump($uri3->__toString());
var_dump($uri4->__toString());

?>
--EXPECT--
string(23) "HTTPS://////EXAMPLE.com"
string(19) "https://example.com"
string(19) "https://example.com"
string(19) "https://example.com"
string(20) "https://example.com/"
string(20) "https://example.com/"
string(20) "https://example.com/"
string(20) "https://example.com/"
