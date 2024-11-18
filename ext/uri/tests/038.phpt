--TEST--
Test toNormalizedString()
--EXTENSIONS--
uri
--XFAIL--
Cloning Rfc3986Uris doesn't copy the path properly yet
--FILE--
<?php

$uri1 = Uri\Rfc3986Uri::parse("HTTPS://////EXAMPLE.com");
$uri2 = Uri\Rfc3986Uri::parse("https://example.com");
$uri3 = Uri\Rfc3986Uri::parse("https://example.com/foo/..");

var_dump($uri1->toNormalizedString());
var_dump($uri1->__toString());
var_dump($uri2->toNormalizedString());
var_dump($uri2->__toString());
var_dump($uri3->toNormalizedString());
var_dump($uri3->__toString());

$uri1 = Uri\WhatWgUri::parse("HTTPS://////EXAMPLE.com");
$uri2 = Uri\WhatWgUri::parse("https://example.com");
$uri3 = Uri\WhatWgUri::parse("https://example.com/foo/..");

var_dump($uri1->toNormalizedString());
var_dump($uri1->__toString());
var_dump($uri2->toNormalizedString());
var_dump($uri2->__toString());
var_dump($uri3->toNormalizedString());
var_dump($uri3->__toString());

?>
--EXPECT--
string(23) "HTTPS://////EXAMPLE.com"
string(19) "https://example.com"
string(19) "https://example.com"
string(19) "https://example.com"
string(19) "https://example.com"
string(26) "https://example.com/foo/.."
string(20) "https://example.com/"
string(20) "https://example.com/"
string(20) "https://example.com/"
string(20) "https://example.com/"
string(20) "https://example.com/"
string(20) "https://example.com/"
