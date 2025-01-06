--TEST--
Test property mutation - user
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withUser("user");
$uri3 = $uri2->withUser(null);

var_dump($uri1->toString());
var_dump($uri2->toString());
var_dump($uri3->toString());

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withUser("user");
$url3 = $url2->withUser(null);

var_dump($url1->toMachineFriendlyString());
var_dump($url2->toMachineFriendlyString());
var_dump($url3->toMachineFriendlyString());

?>
--EXPECT--
string(19) "https://example.com"
string(24) "https://user@example.com"
string(19) "https://example.com"
string(20) "https://example.com/"
string(25) "https://user@example.com/"
string(20) "https://example.com/"
