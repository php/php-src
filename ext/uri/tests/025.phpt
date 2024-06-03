--TEST--
Test property mutation - password
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986Uri::create("https://example.com");
$uri2 = $uri1->withPassword("pass");
$uri3 = $uri2->withPassword(null);

var_dump($uri1->__toString());
var_dump($uri2->__toString());
var_dump($uri3->__toString());

$uri1 = Uri\WhatWgUri::create("https://example.com");
$uri2 = $uri1->withPassword("pass");
$uri3 = $uri2->withPassword(null);

var_dump($uri1->__toString());
var_dump($uri2->__toString());
var_dump($uri3->__toString());

?>
--EXPECT--
string(19) "https://example.com"
string(25) "https://:pass@example.com"
string(19) "https://example.com"
string(20) "https://example.com/"
string(20) "https://example.com/"
string(20) "https://example.com/"
