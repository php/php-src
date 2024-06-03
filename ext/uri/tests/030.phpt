--TEST--
Test property mutation - fragment
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986Uri::create("https://example.com#fragment1");
$uri2 = $uri1->withFragment("#fragment2");
$uri3 = $uri2->withFragment(null);

var_dump($uri1->__toString());
var_dump($uri2->__toString());
var_dump($uri3->__toString());

$uri1 = Uri\Rfc3986Uri::create("https://example.com?abc=def");
$uri2 = $uri1->withFragment("#fragment");

var_dump($uri1->__toString());
var_dump($uri2->__toString());

$uri1 = Uri\WhatWgUri::create("https://example.com#fragment1");
$uri2 = $uri1->withFragment("#fragment2");
$uri3 = $uri2->withFragment(null);

var_dump($uri1->__toString());
var_dump($uri2->__toString());
var_dump($uri3->__toString());

$uri1 = Uri\WhatWgUri::create("https://example.com?abc=def");
$uri2 = $uri1->withFragment("#fragment");

var_dump($uri1->__toString());
var_dump($uri2->__toString());

?>
--EXPECT--
string(29) "https://example.com#fragment1"
string(29) "https://example.com#fragment2"
string(19) "https://example.com"
string(27) "https://example.com?abc=def"
string(36) "https://example.com?abc=def#fragment"
string(30) "https://example.com/#fragment1"
string(30) "https://example.com/#fragment1"
string(30) "https://example.com/#fragment1"
string(28) "https://example.com/?abc=def"
string(28) "https://example.com/?abc=def"
